MakeFullPath	PROTO  	:DWORD, :DWORD, :DWORD, :DWORD, :DWORD
GAllocNoFail	PROTO  	:DWORD
DoCWDCommand	PROTO  	:DWORD, :DWORD, :DWORD, :DWORD
DoCDUPCommand	PROTO 	:DWORD, :DWORD
DoSIZECommand	PROTO 	:DWORD, :DWORD, :DWORD, :DWORD
DoDELECommand	PROTO 	:DWORD, :DWORD, :DWORD, :DWORD
DoEXECCommand	PROTO 	:DWORD, :DWORD, :DWORD, :DWORD
DoMKDCommand	PROTO  	:DWORD, :DWORD, :DWORD, :DWORD
DoRMDCommand	PROTO  	:DWORD, :DWORD, :DWORD, :DWORD
DoRenameCommand PROTO 	:DWORD, :DWORD, :DWORD, :DWORD

CurrentDirBuffSize	equ 1024
SendBuffSize 		equ 2048

.code

MakeFullPath PROC Path:DWORD, FTPS:DWORD, DestBuff:DWORD, CheckIfExists:DWORD, IsDir:DWORD
	mov eax, Path
	.IF byte ptr [eax] == '/'
		.IF byte ptr [eax+1] == 0
			;It is root
			mov eax, DestBuff
			mov word ptr [eax], '/'
			je MFCRetTrue
		.ENDIF
		
		;It is full path	
		;Make sure it aint too long
 		invoke lstrlen, eax
 		inc eax
 		cmp eax, CurrentDirBuffSize
		jge MFCRetFalse

		;Copy it to DestBuff
		invoke lstrcpy, DestBuff, Path

	.ELSE
		;It is a relative path
		mov eax, FTPS
		invoke lstrlen, (FTPSession ptr [eax]).lpCurrentDir
		
		push eax
		invoke lstrlen, Path
		pop ecx
		add eax, ecx
		inc eax			
		cmp eax, CurrentDirBuffSize
		jge MFCRetFalse

		;Copy to DestBuff
		mov eax, FTPS
		invoke lstrcpy, DestBuff, (FTPSession ptr [eax]).lpCurrentDir
		invoke lstrcat, DestBuff, Path

	.ENDIF
	
MFCCheckIfItExists:

	.IF CheckIfExists != 0
		mov eax, DestBuff
		inc eax
		invoke GetFileAttributes, eax
		cmp eax, 0FFFFFFFFh
		je MFCRetFalse
		and eax, FILE_ATTRIBUTE_DIRECTORY
		mov ecx, IsDir
		.IF eax != 0
			;It is a directory
			mov dword ptr [ecx], 1
			
			;make sure it ends with a '/'
			invoke lstrlen, DestBuff
			dec eax
			add eax, DestBuff
			cmp byte ptr [eax], '/'
			je MFCRetTrue
			
			mov word ptr [eax+1], '/'
		.ELSE
			;It is a file
			mov dword ptr [ecx], 0
		.ENDIF
	.ENDIF
	
MFCRetTrue:
	;may as well return something usefull
	invoke lstrlen, DestBuff
	ret
	
MFCRetFalse:
	xor eax, eax
	ret
MakeFullPath ENDP

DoCWDCommand PROC ServSock:DWORD, SendBuff:DWORD, NewDir:DWORD, FTPS:DWORD
	LOCAL CurrentDir:DWORD
	LOCAL IsDir:DWORD
	
	;Put Current Dir in local var cuz I use it a lot
	mov eax, FTPS
	mov eax, (FTPSession ptr [eax]).lpCurrentDir
	mov CurrentDir, eax

	invoke MakeFullPath, NewDir, FTPS, SendBuff, 1, addr IsDir
	.IF eax != 0
		mov eax, SendBuff
		.IF word ptr [eax] == '/'
			;It is root
			mov eax, CurrentDir
			mov word ptr [eax], '/'
			invoke SendReply, ServSock, addr szRep51
			
		.ELSE
			;Its a full path
			.IF IsDir != 0
				;copy it to the Current Dir buffer 
				invoke lstrcpy, CurrentDir, SendBuff
				invoke SendReply, ServSock, addr szRep51
				
			.ELSE
				;They specified a file not a directory
				invoke SendReply, ServSock, addr szRep52
				
			.ENDIF
		.ENDIF
	.ELSE
		;error
		invoke SendReply, ServSock, addr szRep52
		
	.ENDIF

	ret
DoCWDCommand ENDP


;This one is a little easier than CWD hehe
DoCDUPCommand PROC ServSock:DWORD, FTPS:DWORD
	mov eax, FTPS
	mov eax, (FTPSession ptr [eax]).lpCurrentDir
	push eax
	invoke lstrlen, eax
	pop ecx
	
	dec eax
	test eax, eax ;Check if we are already at root
	jz CDUPJustReturn
	
	add eax, ecx
	dec eax
	
	.WHILE byte ptr [eax] != '/'
		dec eax
	.ENDW
	
	inc eax
	mov byte ptr [eax], 0

CDUPJustReturn:
	invoke SendReply, ServSock, addr szRep51
	ret
	
DoCDUPCommand ENDP

DoSIZECommand PROC ServSock:DWORD, Path:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL FindStruct:WIN32_FIND_DATA
	LOCAL IsDir:DWORD
	
	invoke MakeFullPath, Path, FTPS, SendBuff, 1, addr IsDir
	.IF eax != 0
		xor eax, eax
		cmp IsDir, eax
		jne SendDaSize  ;If it is a dir, size is always 0
		
		inc SendBuff
		invoke FindFirstFile, SendBuff, addr FindStruct
		invoke FindClose, eax
		mov eax, FindStruct.nFileSizeLow

SendDaSize:
		invoke wsprintf, SendBuff, addr szRep9, eax
		invoke SendReply, ServSock, SendBuff
	.ELSE
		invoke SendReply, ServSock, addr szRep52
	.ENDIF
	ret
DoSIZECommand ENDP

DoDELECommand PROC ServSock:DWORD, Path:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL IsDir:DWORD
	
	invoke MakeFullPath, Path, FTPS, SendBuff, 1, addr IsDir
	.IF (eax != 0) && (IsDir == 0)
		inc SendBuff
		invoke DeleteFile, SendBuff
		.IF eax != 0
			;Success
			invoke SendReply, ServSock, addr szRep55
		.ELSE
			;Error
			invoke SendReply, ServSock, addr szRep27
		.ENDIF
	.ELSE
	
		invoke SendReply, ServSock, addr szRep52
	.ENDIF
	ret
DoDELECommand ENDP

DoMKDCommand PROC ServSock:DWORD, Path:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL IsDir:DWORD
	
	invoke MakeFullPath, Path, FTPS, SendBuff, 0, addr IsDir
	.IF (eax != 0)
		inc SendBuff
		invoke CreateDirectory, SendBuff, NULL
		.IF eax != 0
			;Success
			invoke SendReply, ServSock, addr szRep19
		.ELSE
			;Error
			invoke SendReply, ServSock, addr szRep27
		.ENDIF
	.ELSE
	
		invoke SendReply, ServSock, addr szRep27
	.ENDIF
	ret
DoMKDCommand ENDP

DoRMDCommand PROC ServSock:DWORD, Path:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL IsDir:DWORD
	
	invoke MakeFullPath, Path, FTPS, SendBuff, 1, addr IsDir
	.IF (eax != 0) && (IsDir == 1)
		inc SendBuff
		invoke RemoveDirectory, SendBuff
		.IF eax != 0
			;Success
			invoke SendReply, ServSock, addr szRep56
		.ELSE
			;Error
			invoke SendReply, ServSock, addr szRep27
		.ENDIF
	.ELSE
	
		invoke SendReply, ServSock, addr szRep52
	.ENDIF
	ret
DoRMDCommand ENDP


DoRenameCommand PROC ServSock:DWORD, Path:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL IsDir:DWORD
	LOCAL NewNameBuff:DWORD
	
	invoke GAllocNoFail, CurrentDirBuffSize
	mov NewNameBuff, eax
	
	invoke MakeFullPath, Path, FTPS, NewNameBuff, 0, addr IsDir
	.IF eax != 0
		inc SendBuff
		mov eax, NewNameBuff
		inc eax
		invoke MoveFile, SendBuff, eax
		.IF eax != 0
			;Success
			invoke SendReply, ServSock, addr szRep58
		.ELSE
			;Error
			invoke SendReply, ServSock, addr szRep27
		.ENDIF
	.ELSE
	
		invoke SendReply, ServSock, addr szRep27
	.ENDIF
	
	invoke GlobalFree, NewNameBuff
	ret
DoRenameCommand ENDP

DoEXECCommand PROC ServSock:DWORD, Path:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL IsDir:DWORD
	
	invoke MakeFullPath, Path, FTPS, SendBuff, 1, addr IsDir
	.IF (eax != 0) && (IsDir == 0)
		inc SendBuff
		invoke ShellExecute, 0, 0, SendBuff, 0, 0, SW_SHOWNORMAL 
		.IF eax > 32
			;Success
			invoke SendReply, ServSock, addr szRep59
		.ELSE
			;Error
			invoke SendReply, ServSock, addr szRep27
		.ENDIF
	.ELSE
	
		invoke SendReply, ServSock, addr szRep52
	.ENDIF
	ret
DoEXECCommand ENDP