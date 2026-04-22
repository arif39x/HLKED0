	include analftp.inc
	include CProcs.asm
	include DTPConnections.asm
	include DTPCommands.asm
	include Commands.asm

MainSockListen PROTO
ServeThread PROTO :DWORD
SendReply PROTO :DWORD, :DWORD
HandleCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD, :DWORD

;Procs in CProcs.asm
strnicmp PROTO :DWORD, :DWORD, :DWORD
GAllocNoFail PROTO :DWORD
StrToDword PROTO :DWORD, :DWORD

;Procs in DTPConnections.asm
DirectDTPCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
OpenPassivePort PROTO :DWORD, :DWORD, :DWORD
PassiveThread PROTO :DWORD
DoPortCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
PortThread PROTO :DWORD
CloseSockTillItsDead PROTO :DWORD

;Procs in DTPCommands.asm
DoLISTCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoRETRCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoSTORCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD, :DWORD

;Procs in Commands.asm
DoCWDCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoCDUPCommand PROTO :DWORD, :DWORD
DoSIZECommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoDELECommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoMKDCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoRMDCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoRenameCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoEXECCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD



RecvBufSize equ 2048
SendBuffSize equ 2048
CurrentDirBuffSize equ 1024

crlf		equ  00A0Dh ;Carrage Return / Line Feed (backwords)

.data



.data?
szRepname 	dd ?
ListenSock	dd ?
ThreadID	dd ?
WSockData	WSADATA	<>
MainSIN	sockaddr_in	<>

.code
startftp proc
	invoke WSAStartup, 101h, addr WSockData
	.IF eax != 0
		jmp ServerExit
	.ENDIF
 IFDEF MOD_IFONLINE
CheckIfConnected:
	invoke IsOnline
	.IF eax == 0
		invoke Sleep, 1000
		jmp CheckIfConnected
	.ENDIF
 ENDIF
	invoke MainSockListen

MainProcessLoop:
	invoke listen, ListenSock, SOMAXCONN
	
	invoke accept, ListenSock, addr MainSIN, NULL
	cmp eax,INVALID_SOCKET
	je MainProcessLoop
	
	invoke CreateThread, NULL, 0, addr ServeThread, eax, 0, addr ThreadID
	test eax, eax
	jz MainProcessLoop
	
	invoke CloseHandle, eax
	jmp MainProcessLoop
	
ServerExit:
ret
startftp endp

MainSockListen PROC

CreateSockAgain:

	invoke socket, AF_INET, SOCK_STREAM, 0 ;Create Socket
	mov ListenSock, eax
	cmp eax, INVALID_SOCKET
	je TryAgainInASec

	;Fill Sockaddr_IN Struct
	mov MainSIN.sin_family, AF_INET
	mov MainSIN.sin_addr.S_un.S_addr, INADDR_ANY  
	invoke htons, ddListenPort
	mov MainSIN.sin_port, ax

	invoke bind, ListenSock, addr MainSIN, sizeof MainSIN
	cmp eax,0
	je ReturnFromSockListen
	;invoke ExitProcess, 0
	
TryAgainInASec:
	invoke closesocket, ListenSock
	invoke Sleep, 1000
	jmp CreateSockAgain

ReturnFromSockListen:
	ret
MainSockListen ENDP

ServeThread PROC ServSock:DWORD
	LOCAL RecvBuf:DWORD
	LOCAL SendBuff:DWORD
	LOCAL RecvBuffCurent:DWORD
	LOCAL CanRead:DWORD
	LOCAL BytesRead:DWORD
	LOCAL IgnoreCommand:DWORD
	LOCAL FTPS:FTPSession

	;Intitalize FTPS
	mov eax, ServSock
	mov FTPS.ServSock, eax
	xor eax, eax
	mov FTPS.LoggedIn, eax
	mov FTPS.User_Accepted, eax
	mov FTPS.PasvSock, eax
	mov FTPS.PasvPortNumb, eax
	mov FTPS.DTPCommand, eax
	mov FTPS.DTPMode, eax
	mov FTPS.PortIP, eax
	mov FTPS.PortPortNumb, eax
	mov FTPS.PortSock, eax
	mov FTPS.ResumeOffset, eax
	mov FTPS.Renaming, eax
	mov FTPS.CurrentType, 'A'

	
	;Intitalize other vars
	mov IgnoreCommand, 0
	mov CanRead, RecvBufSize
	
	;Send Connected String
	invoke SendReply, ServSock, addr szRep12
	
	;Allocating with global alloc is genreally better than allocating off the stack
	;Allocate Send Buffer
	invoke GAllocNoFail, SendBuffSize
	mov SendBuff, eax
	
	;Allocate Current Dir Buffer
	invoke GAllocNoFail, CurrentDirBuffSize
	mov FTPS.lpCurrentDir, eax
	mov word ptr [eax], '/'
	
	;Allocate DTP Path Buffer
	invoke GAllocNoFail, CurrentDirBuffSize
	mov FTPS.DTPPathBuff, eax
	
	;Allocate Receive Buffer
	invoke GAllocNoFail, RecvBufSize
	mov RecvBuf, eax
	mov RecvBuffCurent, eax
	
	;Receive Data
ReceiveLoop:
	mov eax, CanRead
	test eax, eax
	jz BufferingError

	invoke recv, ServSock, RecvBuffCurent, CanRead, 0
	test eax, eax
	jz CloseSocket
	cmp eax, SOCKET_ERROR
	je CloseSocket	
	mov BytesRead, eax
	add RecvBuffCurent, eax
	sub CanRead, eax

	;invoke MessageBox, 0, RecvBuf, RecvBuf, MB_OK (use this to test for bugs)

FindCrlf:
	;Next we look for crlf
	mov ebx, RecvBuffCurent
	mov esi, RecvBuf
@@:

	lodsb
	cmp ax, 13
	je @F

	cmp esi, ebx
	jge ReceiveLoop
	jmp @B
	
@@:

	;Found cr!
	dec esi
	mov byte ptr [esi], 0 ;Null terminate
	mov ebx, esi
	sub ebx, RecvBuf
	inc esi
	;check for lf
	.IF  (esi < RecvBuffCurent) && (byte ptr [esi] == 10)
		inc esi
	.ENDIF
	push esi

	.IF IgnoreCommand == 0
		invoke HandleCommand, ServSock, SendBuff, RecvBuf, ebx, addr FTPS
	.ELSE
		;Send Error Reply
		invoke SendReply, ServSock, addr szRep27
		mov IgnoreCommand, 0
	.ENDIF 

	pop esi

	mov eax, RecvBuffCurent
	sub eax, esi
	
	.IF eax != 0
	
		;If there is still data after crlf
		mov ebx, RecvBufSize
		sub ebx, eax
		mov CanRead, eax 

		push RecvBuf
		pop RecvBuffCurent
		
		invoke RtlMoveMemory, RecvBuf, esi, eax
		jmp FindCrlf
	.ELSE
	
		;No data after crlf
		push RecvBuf
		pop RecvBuffCurent
		mov CanRead, RecvBufSize
		jmp ReceiveLoop
	.ENDIF


BufferingError:
	;Ignore the rest of this command
	mov IgnoreCommand, 1

	;Reset our recv buffering stuff
	push RecvBuf
	pop RecvBuffCurent
	mov CanRead, RecvBufSize
	jmp ReceiveLoop ;resume
	

CloseSocket:

	;close any DTP sockets that are still open
	lea eax, FTPS.PasvSock
	invoke CloseSockTillItsDead, eax

	lea eax, FTPS.PortSock
	invoke CloseSockTillItsDead, eax

	invoke GlobalFree, RecvBuf
	invoke GlobalFree, SendBuff
	invoke closesocket, ServSock
	ret
ServeThread ENDP

;At this point we have a command string, now we must understand it!
HandleCommand PROC ServSock:DWORD, SendBuff:DWORD, CmdStr:DWORD, CmdLen:DWORD, FTPS:DWORD
	.IF CmdLen > 2
		;Check if we are logged in (very important ;-P)
		mov eax, FTPS
		mov eax, (FTPSession ptr [eax]).LoggedIn
		.IF eax != 0
			;We are logged in!
			;Check Which command it is	
			xor ebx, ebx
			mov ecx, offset szUser 
			.While ebx < 29 && eax != 0
				push ebx
				push ecx
				invoke strnicmp, CmdStr, ecx, 4
				pop ecx
				pop ebx
				inc ebx
				add ecx, 4
			.ENDW

			.IF ebx == 29 ;Command unrecognised
				invoke SendReply, ServSock, addr szRep29
				ret
			.ENDIF

			;Check if we are renaming
			mov eax, FTPS
			mov ecx, (FTPSession ptr [eax]).Renaming
			.IF ecx != 0
				.IF (ebx == RNTO_Cmd) && (CmdLen > 5)
					push ebx
					mov ecx, CmdStr
					add ecx, 5
					invoke DoRenameCommand, ServSock, ecx, SendBuff, FTPS
					pop ebx
				.ENDIF
				mov eax, FTPS
				mov (FTPSession ptr [eax]).Renaming, 0
				
			.ENDIF

			;Check for 3 letter / No paremeters commands
			.IF CmdLen == 3
				.IF ebx == PWD_Cmd ;Print Working Directory
					mov eax, FTPS
					mov eax, (FTPSession ptr [eax]).lpCurrentDir
					invoke wsprintf, SendBuff, addr szRep43, eax
					invoke SendReply, ServSock, SendBuff
				.ELSE
					invoke SendReply, ServSock, addr szRep29
				.ENDIF
				
			.ENDIF
			
			;Check for 4 letters / No paremeters commands
			.IF CmdLen == 4
				.IF ebx == PASV_Cmd ;Open Passive Port
					invoke OpenPassivePort, ServSock, SendBuff, FTPS
				
				.ELSEIF ebx == LIST_Cmd ;List Current Directory
					invoke DirectDTPCommand, DTP_List_Cmd, ServSock, SendBuff, FTPS
				
				.ELSEIF ebx == NLST_Cmd ;Name List Current Directory
					invoke DirectDTPCommand, DTP_Name_List_Cmd, ServSock, SendBuff, FTPS
				
				.ELSEIF ebx == ABOR_Cmd ;Abort mission! Pull out!! NOOOOOOOO!
					mov eax, FTPS
					lea eax, (FTPSession ptr [eax]).PasvSock
					invoke CloseSockTillItsDead, eax
					
					mov eax, FTPS
					lea eax, (FTPSession ptr [eax]).PortSock
					invoke CloseSockTillItsDead, eax
					invoke SendReply, ServSock, addr szRep54
				
				.ELSEIF ebx == HELP_Cmd ;hehehehehehe (Really.. I like to help people! NOT!)
					invoke SendReply, ServSock, addr szRep10
					
				.ELSEIF ebx == NOOP_Cmd ;NOOP!
					invoke SendReply, ServSock, addr szRep5
					
				.ELSEIF ebx == CDUP_Cmd ;Change to Parent Directory
					invoke DoCDUPCommand, ServSock, FTPS
				
				.ELSEIF ebx == SYST_Cmd ;a dumb command hehe
					invoke SendReply, ServSock, addr szRep46
					
				.ELSEIF ebx == STAT_Cmd ;*evil grin*
					invoke SendReply, ServSock, addr szRep7
				
				.ELSEIF ebx == QUIT_Cmd ;Umm.. duh!
					invoke SendReply, ServSock, addr szRep13
					invoke closesocket, ServSock
					
				.ELSE
					invoke SendReply, ServSock, addr szRep29
				.ENDIF			
			.ENDIF

			;Check for 3 letter with paremeter commands
			.IF CmdLen > 4
				.IF ebx == CWD_Cmd ;Change Working Directory
					mov eax, CmdStr
					add eax, 4
					invoke DoCWDCommand, ServSock, SendBuff, eax, FTPS
					
				.ELSEIF ebx == MKD_Cmd ;Make Directory
					mov eax, CmdStr
					add eax, 4
					invoke DoMKDCommand, ServSock, eax, SendBuff, FTPS
					
				.ELSEIF ebx == RMD_Cmd ;Remove Directory
					mov eax, CmdStr
					add eax, 4
					invoke DoRMDCommand, ServSock, eax, SendBuff, FTPS

				;Check for 4 Letter with paremter commands
				.ELSEIF CmdLen > 5
					.IF ebx == REST_Cmd ;Restart
						mov eax, CmdStr
						add eax, 5
						invoke atodw, eax
						mov ecx, FTPS
						mov (FTPSession ptr [ecx]).ResumeOffset, eax
						invoke wsprintf, SendBuff, addr szRep22, eax
						invoke SendReply, ServSock, SendBuff
					
					.ELSEIF ebx == TYPE_Cmd ;Transfer Type
						mov eax, CmdStr
						add eax, 5
						xor ebx, ebx
						mov bl, byte ptr [eax]
						mov eax, FTPS
						mov (FTPSession ptr [eax]).CurrentType, ebx
						invoke wsprintf, SendBuff, addr szRep42, ebx
						invoke SendReply, ServSock, SendBuff
						
					.ELSEIF ebx == PORT_Cmd ;Port
						mov eax, CmdStr
						add eax, 4
						invoke DoPortCommand, eax, ServSock, SendBuff, FTPS
						
					.ELSEIF ebx == RETR_Cmd ;Retrieve
						mov eax, CmdStr
						add eax, 5
						mov ecx, FTPS
						invoke MakeFullPath, eax, FTPS, (FTPSession ptr [ecx]).DTPPathBuff, 1, SendBuff
						mov ecx, SendBuff
						.IF (eax != 0) && (dword ptr [ecx] == 0)
							invoke DirectDTPCommand, DTP_Retrieve_Cmd, ServSock, SendBuff, FTPS
						.ELSE
							;error
							invoke SendReply, ServSock, addr szRep52
						.ENDIF
						
					.ELSEIF ebx == STOR_Cmd ;Retrieve
						mov eax, CmdStr
						add eax, 5
						mov ecx, FTPS
						invoke MakeFullPath, eax, FTPS, (FTPSession ptr [ecx]).DTPPathBuff, 0, SendBuff
						.IF eax != 0
							invoke DirectDTPCommand, DTP_Store_Cmd, ServSock, SendBuff, FTPS
						.ELSE
							;error
							invoke SendReply, ServSock, addr szRep52
						.ENDIF
						
					.ELSEIF ebx == APPE_Cmd ;Retrieve
						mov eax, CmdStr
						add eax, 5
						mov ecx, FTPS
						invoke MakeFullPath, eax, FTPS, (FTPSession ptr [ecx]).DTPPathBuff, 0, SendBuff
						.IF eax != 0
							invoke DirectDTPCommand, DTP_Append_Cmd, ServSock, SendBuff, FTPS
						.ELSE
							;error
							invoke SendReply, ServSock, addr szRep52
						.ENDIF
					
					.ELSEIF ebx == SIZE_Cmd ;Get File Size
						mov eax, CmdStr
						add eax, 5
						invoke DoSIZECommand, ServSock, eax, SendBuff, FTPS
						
					.ELSEIF ebx == DELE_Cmd ;Delete!
						mov eax, CmdStr
						add eax, 5
						invoke DoDELECommand, ServSock, eax, SendBuff, FTPS
	
					.ELSEIF ebx == EXEC_Cmd ;EXECUTE!!
						mov eax, CmdStr
						add eax, 5
						invoke DoEXECCommand, ServSock, eax, SendBuff, FTPS				
						
					.ELSEIF ebx == RNFR_Cmd ;Rename From
						mov eax, CmdStr
						add eax, 5
						lea ecx, CmdLen
						invoke MakeFullPath, eax, FTPS, SendBuff, 1, ecx
						.IF eax != 0
							;It exists!
							mov eax, FTPS
							mov (FTPSession ptr [eax]).Renaming, 1
							invoke SendReply, ServSock, addr szRep57
						
						.ELSE
							;error
							invoke SendReply, ServSock, addr szRep52
						.ENDIF
					
					.ELSE
						invoke SendReply, ServSock, addr szRep29	
						
					.ENDIF

				.ELSE
					invoke SendReply, ServSock, addr szRep29
							
				.ENDIF
			.ENDIF
		
		.ELSE
			;Waiting to log in...
			;First make sure it is at least 5 long
			.IF CmdLen > 5
				;Check if it is user command
				invoke strnicmp, CmdStr, addr szUser, 4
				.IF eax == 0
					mov eax, CmdStr
					add eax, 5
					invoke lstrcmpi, eax, addr szUserName
					.IF eax == 0
						;Username OK
						mov eax, FTPS
						mov (FTPSession ptr [eax]).User_Accepted, 1
						
						;Send successs reply
						mov ecx, CmdStr
						add ecx, 5
						invoke wsprintf, SendBuff, addr szRep20, ecx
						invoke SendReply, ServSock, SendBuff
						
					.ELSE
						;Invalid Username
						invoke SendReply, ServSock, addr szRep40
					.ENDIF
					ret				
				.ENDIF

				;Check if it is pass command
				invoke strnicmp, CmdStr, addr szPass, 4
				.IF eax == 0
					mov eax, FTPS
					mov eax, (FTPSession ptr [eax]).User_Accepted
					.IF eax != 0
						mov eax, CmdStr
						add eax, 5
						invoke lstrcmp, eax, addr szPassword1
						.IF eax == 0
							;Password Accepted
							mov eax, FTPS
							mov (FTPSession ptr [eax]).LoggedIn, 1
							invoke SendReply, ServSock, addr szRep17
						.ELSE
							;Password Not Accepted
							invoke SendReply, ServSock, addr szRep40
						.ENDIF
					.ELSE
						;Bad sequence of commands
						invoke SendReply, ServSock, addr szRep32
					.ENDIF
					ret
				.ENDIF
				
				;Not logged in
				invoke SendReply, ServSock, addr szRep34
				
			.ELSE
				;Not long enough to be valid
				invoke SendReply, ServSock, addr szRep29
			.ENDIF
			
			
		.ENDIF
	.ELSE
		;if it is less than 3 charecters long, then it is invalid
		invoke SendReply, ServSock, addr szRep29
	.ENDIF
	ret
HandleCommand ENDP

SendReply PROC ServSock:DWORD, ReplyString:DWORD
	LOCAL SendBuff:DWORD

	invoke lstrlen, ReplyString
	push eax
	inc eax
	inc eax
	invoke GAllocNoFail, eax
	mov SendBuff, eax
	push eax
	invoke lstrcpy, eax, ReplyString
	pop eax
	pop ecx
	;add eax, ecx
	mov word ptr [eax+ecx], crlf
	inc ecx
	inc ecx
	invoke send, ServSock, SendBuff, ecx, 0
	cmp eax, SOCKET_ERROR
	jne SRReturn
	invoke closesocket, ServSock
SRReturn:
	invoke GlobalFree, SendBuff
	ret
SendReply ENDP