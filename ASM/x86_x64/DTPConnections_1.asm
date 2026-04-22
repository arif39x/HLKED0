DirectDTPCommand PROTO		:DWORD, :DWORD, :DWORD, :DWORD
OpenPassivePort PROTO		:DWORD, :DWORD, :DWORD
PortThread PROTO		:DWORD
PassiveThread PROTO		:DWORD
DoPortCommand PROTO		:DWORD, :DWORD, :DWORD, :DWORD
PortThread PROTO 		:DWORD
SendReply PROTO 		:DWORD, :DWORD
DoLISTCommand PROTO 		:DWORD, :DWORD, :DWORD, :DWORD, :DWORD
DoRETRCommand PROTO 		:DWORD, :DWORD, :DWORD, :DWORD
DoSTORCommand PROTO 		:DWORD, :DWORD, :DWORD, :DWORD, :DWORD
CloseSockTillItsDead PROTO	:DWORD

SendBuffSize equ 2048

.code

DirectDTPCommand PROC DTPCmd:DWORD, ServSock:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL ThreadID:DWORD
	
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).DTPMode
	.IF eax != DTP_No_Mode
		mov edx, DTPCmd
		mov (FTPSession ptr [ecx]).DTPCommand, edx

		.IF eax == DTP_Port_Mode
			;Port mode
			
			;If they give any DTP commands while there is a PORT transfer in progress, close port and fail
			mov eax, (FTPSession ptr [ecx]).PortSock
			.IF eax != 0
				;This seems pretty harsh.. but if they attempt a DTP command while a data transfer
				;is in progress.. this is what happens
				lea eax, (FTPSession ptr [ecx]).PortSock
				invoke CloseSockTillItsDead, eax

				;tell em they screwed up
				invoke SendReply, ServSock, addr szRep50
				ret
			.ENDIF

			;This is where we create the PORT thread that connects to the client to do the command
			invoke CreateThread, NULL, 0, addr PortThread, FTPS, 0, addr ThreadID
			.IF eax == 0
				invoke SendReply, ServSock, addr szRep27
				jmp DDTPCReturn
			.ELSE
				invoke CloseHandle, eax
			.ENDIF

		.ENDIF
	
	.ELSE
		;no transfer mode specified
		invoke SendReply, ServSock, addr szRep48
	.ENDIF
DDTPCReturn:
	ret
DirectDTPCommand ENDP

PortThread PROC FTPS:DWORD
	LOCAL PortSock:DWORD
	LOCAL ServSock:DWORD
	LOCAL SendBuff:DWORD
	LOCAL lpCurrentDir:DWORD
	LOCAL ResumeOffset:DWORD
	LOCAL sin:sockaddr_in
	
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).ServSock
	mov ServSock, eax
	mov eax, (FTPSession ptr [ecx]).lpCurrentDir
	mov lpCurrentDir, eax
	mov eax, (FTPSession ptr [ecx]).ResumeOffset
	mov ResumeOffset, eax

	invoke GAllocNoFail, SendBuffSize
	mov SendBuff, eax

	invoke SendReply, ServSock, addr szRep49
	invoke socket, AF_INET, SOCK_STREAM, 0
	cmp eax, INVALID_SOCKET
	je OPCError1
	mov PortSock, eax 

	;Fill sockaddr_in struct
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).PortIP
	mov sin.sin_addr.S_un.S_addr, eax
	mov eax, (FTPSession ptr [ecx]).PortPortNumb
	mov sin.sin_port, ax
	mov sin.sin_family, AF_INET
	
	invoke connect, PortSock, addr sin, sizeof sin
	test eax, eax
	jnz OPCError2
	
	;Success!
	mov ecx, FTPS
	mov eax, PortSock
	mov (FTPSession ptr [ecx]).PortSock, eax
	
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).DTPCommand
	;Now.. carry out the command!!
	.IF eax == DTP_List_Cmd
		invoke DoLISTCommand, ServSock, PortSock, SendBuff, lpCurrentDir, 0
		
	.ELSEIF eax == DTP_Name_List_Cmd
		invoke DoLISTCommand, ServSock, PortSock, SendBuff, lpCurrentDir, 1
		
	.ELSEIF eax == DTP_Retrieve_Cmd
		invoke DoRETRCommand, ServSock, PortSock, SendBuff, FTPS
			
	.ELSEIF eax == DTP_Store_Cmd
		invoke DoSTORCommand, ServSock, PortSock, SendBuff, FTPS, 0
		
	.ELSEIF eax == DTP_Store_Unique_Cmd
	
	.ELSEIF eax == DTP_Append_Cmd
		invoke DoSTORCommand, ServSock, PortSock, SendBuff, FTPS, 1
	
	.ENDIF
	invoke closesocket, PortSock
	jmp OPCReturn
	
	OPCError2:
	invoke closesocket, PortSock
	
	OPCError1:
	invoke SendReply, ServSock, addr szRep24
	
	OPCReturn:
	invoke GlobalFree, SendBuff
	mov ecx, FTPS
	mov (FTPSession ptr [ecx]).DTPCommand, 0
	mov (FTPSession ptr [ecx]).DTPMode, 0
	mov (FTPSession ptr [ecx]).PortSock, 0
	mov (FTPSession ptr [ecx]).PortIP, 0
	mov (FTPSession ptr [ecx]).PortPortNumb, 0
	ret
PortThread ENDP

;This opens a local port used for data transfer
OpenPassivePort PROC ServSock:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL sin:sockaddr_in
	LOCAL hPasvSock:DWORD
	LOCAL bfMyIP[261]:BYTE
	LOCAL lpNameLen:DWORD
	LOCAL ThreadID:DWORD
		
	;First Check if there is already an open DTP socket and if so, close it
	mov eax, FTPS
	lea eax, (FTPSession ptr [eax]).PasvSock
	invoke CloseSockTillItsDead, eax
	
	mov eax, FTPS
	lea eax, (FTPSession ptr [eax]).PortSock
	invoke CloseSockTillItsDead, eax
	
	;create socket	
	invoke socket, AF_INET, SOCK_STREAM, 0
	mov hPasvSock, eax
	cmp eax, INVALID_SOCKET
	je OPPFail

	;Fill Sockaddr_IN Struct
	mov sin.sin_family, AF_INET
	mov sin.sin_addr.S_un.S_addr, INADDR_ANY  
	mov sin.sin_port, 0 ;Zero for random

	invoke bind, hPasvSock, addr sin, sizeof sin
	test eax, eax
	jnz OPPFail
	
@@:
	;Get the port number
	mov lpNameLen, sizeof sin
	invoke getsockname, hPasvSock, addr sin, addr lpNameLen
	test eax, eax
	jnz OPPFail
	
	;Store Port Number
	xor ecx, ecx
	mov cx, sin.sin_port
	test cx, cx
	jz @B
	mov eax, FTPS
	mov (FTPSession ptr [eax]).PasvPortNumb, ecx
	
	;Store Socket Handle
	mov ecx, hPasvSock
	mov (FTPSession ptr [eax]).PasvSock, ecx

	;Set DTP mode
	mov (FTPSession ptr [eax]).DTPMode, DTP_Pasv_Mode

	;Create our serve thread
	invoke CreateThread, NULL, 0, addr PassiveThread, FTPS, 0, addr ThreadID
	test eax, eax
	jz OPPFail
	
	invoke CloseHandle, eax

	;Get our local IP
	mov lpNameLen, sizeof sin
	invoke getsockname, ServSock, addr sin, addr lpNameLen
	test eax, eax
	jnz OPPFail

	;Get ready for formatting for our reply
	mov eax, sin.sin_addr.S_un.S_addr
	xor ecx, ecx
	mov cl, al
	push ecx
	mov cl, ah
	push ecx
	shr eax, 16
	mov cl, al
	push ecx
	mov cl, ah
	push ecx
	
	mov eax, FTPS
	mov eax, (FTPSession ptr [eax]).PasvPortNumb
	mov cl, al
	push ecx
	mov cl, ah
	push ecx 

	;Pop everthing
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	
	;Format Send Buffer
	invoke wsprintf, SendBuff, addr szRep16, eax, ebx, ecx, edx, esi, edi
	invoke SendReply, ServSock, SendBuff
	
	xor eax, eax ;lol.. like I actually look at the return value
	inc eax
	ret
	
OPPFail:
	;Close Socket
	invoke closesocket, hPasvSock

	;Zero Values
	mov eax, FTPS
	mov (FTPSession ptr [eax]).PasvPortNumb, 0
	mov (FTPSession ptr [eax]).PasvSock, 0
	mov (FTPSession ptr [eax]).DTPMode, 0

	;Send Failure Reply
	invoke SendReply, ServSock, addr szRep27
	
	xor eax, eax	
	ret
OpenPassivePort ENDP

PassiveThread PROC FTPS:DWORD
	LOCAL PasvSock:DWORD
	LOCAL ServSock:DWORD
	LOCAL SendBuff:DWORD
	LOCAL lpCurrentDir:DWORD
	LOCAL ResumeOffset:DWORD
	LOCAL sin:sockaddr_in

	;First fill a couple local vars so I dont have to get the values a million times
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).PasvSock
	mov PasvSock, eax
	mov eax, (FTPSession ptr [ecx]).ServSock
	mov ServSock, eax
	mov eax, (FTPSession ptr [ecx]).lpCurrentDir
	mov lpCurrentDir, eax
	mov eax, (FTPSession ptr [ecx]).ResumeOffset
	mov ResumeOffset, eax
	
	invoke GAllocNoFail, SendBuffSize
	mov SendBuff, eax

	;Listen for connections
	invoke listen, PasvSock, SOMAXCONN
	test eax, eax
	jnz PTReturn
	
	invoke accept, PasvSock, addr sin, NULL
	cmp eax,INVALID_SOCKET
	je PTReturn
	
	push eax
	invoke closesocket, PasvSock
	pop eax
	mov PasvSock, eax
	
	mov ecx, FTPS
	mov (FTPSession ptr [ecx]).PasvSock, eax
	
	;Now we are connected.. so we wait till we have something to do, or the sock is closed
@@:
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).DTPCommand
	test eax, eax
	jnz PTReceivedCmd
	
	mov eax, (FTPSession ptr [ecx]).PasvSock
	cmp eax, PasvSock
		jnz PTErrorReply
	invoke Sleep, 100 ;No need to go a million clocks a second!
	jmp @B
		
PTReceivedCmd:
	push eax
	invoke SendReply, ServSock, addr szRep44
	pop eax
	
	;Now we have a command! Lets handle it!
	.IF eax == DTP_List_Cmd
		invoke DoLISTCommand, ServSock, PasvSock, SendBuff, lpCurrentDir, 0
		
	.ELSEIF eax == DTP_Name_List_Cmd
		invoke DoLISTCommand, ServSock, PasvSock, SendBuff, lpCurrentDir, 1
		
	.ELSEIF eax == DTP_Retrieve_Cmd
		invoke DoRETRCommand, ServSock, PasvSock, SendBuff, FTPS
		
	.ELSEIF eax == DTP_Store_Cmd
		invoke DoSTORCommand, ServSock, PasvSock, SendBuff, FTPS, 0
		
	.ELSEIF eax == DTP_Store_Unique_Cmd
	
	.ELSEIF eax == DTP_Append_Cmd
		invoke DoSTORCommand, ServSock, PasvSock, SendBuff, FTPS, 1
	
	.ENDIF
	jmp PTReturn2

PTErrorReply:
	invoke SendReply, ServSock, addr szRep27

PTReturn2:
	invoke closesocket, PasvSock
	
PTReturn:
	invoke GlobalFree, SendBuff
	
	
	mov ecx, FTPS
	mov eax, (FTPSession ptr [ecx]).PasvSock
	.IF eax == PasvSock
		mov (FTPSession ptr [ecx]).PasvSock, 0
		mov (FTPSession ptr [ecx]).PasvPortNumb, 0
		mov (FTPSession ptr [ecx]).DTPCommand, 0
		mov (FTPSession ptr [ecx]).DTPMode, 0
	.ENDIF
	ret
PassiveThread ENDP

DoPortCommand PROC CmdStr:DWORD, ServSock:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL IPByte1:BYTE
	LOCAL IPByte2:BYTE
	LOCAL IPByte3:BYTE
	LOCAL IPByte4:BYTE
	LOCAL PortByte1:BYTE
	LOCAL PortByte2:BYTE
	LOCAL incbyte:BYTE
	LOCAL StopAddy:DWORD

	mov incbyte, 0
	mov ecx, CmdStr
	lea edx, IPByte1

@@:
	inc ecx
	push edx
	invoke StrToDword, ecx, addr StopAddy
	pop edx
	mov byte ptr [edx], al
	dec edx
	inc incbyte
	mov ecx, StopAddy

	mov al, incbyte
	cmp al, 7
	je DCPFail

	mov al, byte ptr [ecx]
	cmp al, ','
	je @B

	test al, al
	jnz DCPFail
	
	mov al, incbyte
	cmp al, 6
	jne DCPFail

	mov ecx, FTPS
	mov (FTPSession ptr [ecx]).DTPMode, DTP_Port_Mode

	lea eax, (FTPSession ptr [ecx]).PasvSock
	invoke CloseSockTillItsDead, eax
	
	mov ecx, FTPS
	lea eax, (FTPSession ptr [ecx]).PortSock
	invoke CloseSockTillItsDead, eax
	mov ecx, FTPS

	mov ah, IPByte4
	mov al, IPByte3
	shl eax, 16
	mov ah, IPByte2
	mov al, IPByte1
	mov (FTPSession ptr [ecx]).PortIP, eax
	
	xor eax, eax
	mov ah, PortByte2
	mov al, PortByte1
	mov (FTPSession ptr [ecx]).PortPortNumb, eax
	
	invoke SendReply, ServSock, addr szRep47
	ret
DCPFail:
	invoke SendReply, ServSock, addr szRep30
	ret
DoPortCommand ENDP

CloseSockTillItsDead PROC lpDTPSock:DWORD
@@:
	mov eax, lpDTPSock
	mov eax, [eax]
	.IF eax != 0
		invoke closesocket, eax
		invoke Sleep, 100 ;give it a tenth of a second.. I'm in no hurry
		jmp @B
	.ENDIF
	ret
CloseSockTillItsDead ENDP
