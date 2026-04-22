DoLISTCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
DoRETRCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD
DoSTORCommand PROTO :DWORD, :DWORD, :DWORD, :DWORD, :DWORD
SendReply PROTO :DWORD, :DWORD

CurrentDirBuffSize equ 1024
SendBuffSize equ 2048

.code

;TODO: Make list work if they specify a path as a paremeter in the list command
DoLISTCommand PROC ServSock:DWORD, DTPSock:DWORD, SendBuff:DWORD, CurrentDir:DWORD, NameList:DWORD
	LOCAL bfReply[100]:BYTE
	LOCAL bpDriveLetter:DWORD
	LOCAL FindStruct:WIN32_FIND_DATA
	LOCAL FindBuff:DWORD
	LOCAL FormatBuff:DWORD
	LOCAL hFindFile:DWORD
	LOCAL TimeStruct:SYSTEMTIME
	LOCAL CurrentYear:WORD
	LOCAL LocalFileTime:FILETIME
	LOCAL TimeYearBuff[25]:BYTE
	
	invoke lstrcpy, addr bfReply, addr szDriveString
	lea eax, bfReply
	add eax, 54
	mov bpDriveLetter, eax
		
	mov ecx, CurrentDir
	mov ax, word ptr [ecx]
	.IF ax == '/'
		;List drives
		invoke GetLogicalDrives
		mov ebx, eax
		mov cl, 'A'
		.WHILE cl != 5Bh	
			mov eax, ebx
			and eax, 1
			.IF eax != 0
				push ebx
				push ecx
				mov eax, bpDriveLetter
				mov byte ptr [eax], cl
				lea ecx, bfReply
				.IF NameList == 0
					;Full info list
					mov eax, 58
				.ELSE
					;Name list
					mov eax, 4
					add ecx, 54
				.ENDIF
				invoke send, DTPSock, ecx, eax, 0
				pop ecx
				pop ebx
				.IF eax == SOCKET_ERROR
					ret
				.ENDIF
			.ENDIF
			inc cl
			shr ebx, 1
		.ENDW
		invoke SendReply, ServSock, addr szRep45
		
	.ELSE
		;List Files in current Dir
		inc CurrentDir 														;Get rid of the leading '/'

																							;First allocate some memory for our search string
		invoke GAllocNoFail, CurrentDirBuffSize+1 ;+1 cuz I gotta add a '*' to the end
		mov FindBuff, eax
	
		invoke lstrcpy, eax, CurrentDir 					;copy our current DIR

		invoke GAllocNoFail, SendBuffSize					;for wsprintf
		mov FormatBuff, eax
		
		invoke lstrlen, FindBuff
		add eax, FindBuff
		mov word ptr [eax], '*' 									;add the * to the end
		
		invoke GetLocalTime, addr TimeStruct			;Get Current Year
		mov ax, TimeStruct.wYear
		mov CurrentYear, ax
		
		;Find First File
		invoke FindFirstFile, FindBuff, addr FindStruct
		cmp eax, INVALID_HANDLE_VALUE
		je EndFindFileLoop2

		mov hFindFile, eax
		jmp HandleFirstFile
		
FindFileLoop:
		invoke FindNextFile, hFindFile, addr FindStruct
		test eax, eax
		jz EndFindFileLoop

HandleFirstFile:
		
		;Copy szFileString to send buffer
		invoke lstrcpy, FormatBuff, addr szFileString
		
		;Next do attributes!
		;Check for Directory
		mov eax, FindStruct.dwFileAttributes
		and eax, FILE_ATTRIBUTE_DIRECTORY
		.IF eax != 0
				;It is a Directory
				mov eax, FormatBuff
				mov byte ptr [eax], 'd'
				
				mov ecx, 3
				direxeloop:
				add eax, 3
				mov byte ptr [eax], 'x'
				loop direxeloop
				jmp DirectorySkip
   	.ENDIF

		;Check for Read Only
		mov eax, FindStruct.dwFileAttributes
		and eax, FILE_ATTRIBUTE_READONLY
		.IF eax != 0
				;It is a Read Only file
				mov ecx, 3
				mov eax, FormatBuff
				dec eax
				readonlyloop:
				add eax, 3
				mov byte ptr [eax], '-'
				loop readonlyloop
    .ENDIF
		
		;Check for Executable (takes a little more work than Read Only or Directory hehe)
		lea eax, FindStruct.cFileName
		push eax
		invoke lstrlen, eax
		pop edx
		.IF eax > 3
			add edx, eax
			sub edx, 4
			
			xor ebx, ebx
			mov ecx, offset szExeExtentions 
			.While ebx < NumbExeExtentions && eax != 0
				push ebx
				push ecx
				push edx
				invoke strnicmp, edx, ecx, 4
				pop edx
				pop ecx
				pop ebx
				inc ebx
				add ecx, 4
			.ENDW
			
			.IF eax == 0
				;It is an Executable file
				mov ecx, 3
				mov eax, FormatBuff
				executableloop:
				add eax, 3
				mov byte ptr [eax], 'x'
				loop executableloop
			.ENDIF
		.ENDIF
		
DirectorySkip:
		;Now we do the date

		;Convert time
		invoke FileTimeToLocalFileTime, addr FindStruct.ftLastWriteTime, addr LocalFileTime
		invoke FileTimeToSystemTime, addr LocalFileTime, addr TimeStruct
		
		;Get Time or Year
		xor eax, eax
		mov ax, TimeStruct.wYear
		.IF ax == CurrentYear
			mov ax, TimeStruct.wHour
			xor ecx, ecx
			mov cx, TimeStruct.wMinute
			invoke wsprintf, addr TimeYearBuff, addr szTimeFormat, eax, ecx
		.ELSE
			invoke wsprintf, addr TimeYearBuff, addr szYearFormat, eax
		.ENDIF

		;Get Month
		xor eax, eax
		mov ax, TimeStruct.wMonth
		dec eax
		shl eax, 2
		mov ecx, offset szDateList
		add ecx, eax

		;Get Day
		xor edx, edx
		mov dx, TimeStruct.wDay
		
		;Format, and send!
		invoke wsprintf, SendBuff, FormatBuff, FindStruct.nFileSizeLow, ecx, edx, addr TimeYearBuff, addr FindStruct.cFileName
		mov ecx, SendBuff
		.IF NameList == 1
			;Name List
			add ecx, eax
			push ecx
			invoke lstrlen, addr FindStruct.cFileName
			pop ecx
			sub ecx, eax
			;Make up for crlf...
			dec ecx
			dec ecx
			inc eax
			inc eax
		.ENDIF
		
		invoke send, DTPSock, ecx, eax, 0
		jmp FindFileLoop
		
EndFindFileLoop:
		invoke FindClose, hFindFile
EndFindFileLoop2:
		invoke GlobalFree, FindBuff
		invoke GlobalFree, FormatBuff
		invoke SendReply, ServSock, addr szRep45

	.ENDIF
	ret
DoLISTCommand ENDP


;Send da file yo
DoRETRCommand PROC ServSock:DWORD, DTPSock:DWORD, SendBuff:DWORD, FTPS:DWORD
	LOCAL hFile:DWORD
	LOCAL ToSend:DWORD
	LOCAL BytesRead:DWORD
	
	;Open File
	mov eax, FTPS
	mov eax, (FTPSession ptr [eax]).DTPPathBuff
	inc eax
	invoke CreateFile, eax, GENERIC_READ, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	cmp eax, INVALID_HANDLE_VALUE
	je RETRFail1
	
	mov hFile, eax
	
	;Get Size
	invoke GetFileSize, eax, NULL
	
	;Support resume
	mov ecx, FTPS
	mov edx, (FTPSession ptr [ecx]).ResumeOffset
	sub eax, edx
	mov ToSend, eax
	mov (FTPSession ptr [ecx]).ResumeOffset, 0
	invoke SetFilePointer, hFile, edx, NULL, FILE_BEGIN
	cmp eax, -1
	je RETRFail2

	;Begin Sending
	.While ToSend != 0
		.IF ToSend < SendBuffSize
			mov ecx, ToSend
		.ELSE
			mov ecx, SendBuffSize
		.ENDIF
		
		sub ToSend, ecx
		push ecx
		invoke ReadFile, hFile, SendBuff, ecx, addr BytesRead, NULL
		pop ecx
		test eax, eax
		jz RETRFail2
		
		invoke send, DTPSock, SendBuff, ecx, 0
		cmp eax, SOCKET_ERROR
		je RETRFail2
	.ENDW
	
	;Success!
	invoke CloseHandle, hFile
	invoke SendReply, ServSock, addr szRep45
	ret

RETRFail2:
	invoke CloseHandle, hFile
RETRFail1:
	invoke SendReply, ServSock, addr szRep53
	ret
DoRETRCommand ENDP


;Just like send but backwords right? Well.. sorta hehe.
DoSTORCommand PROC ServSock:DWORD, DTPSock:DWORD, RecvBuff:DWORD, FTPS:DWORD, Append:DWORD
	LOCAL hFile:DWORD
	LOCAL BytesWritten:DWORD
	
	;Open/Create File
	mov eax, FTPS
	mov eax, (FTPSession ptr [eax]).DTPPathBuff
	inc eax
	invoke CreateFile, eax, GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
	cmp eax, INVALID_HANDLE_VALUE
	je STORFail1
	
	mov hFile, eax
	
	.IF Append != 0
		invoke SetFilePointer, hFile, 0, NULL, FILE_END
	.ELSE
		;Support resume
		mov eax, FTPS
		mov ecx, (FTPSession ptr [eax]).ResumeOffset
		mov (FTPSession ptr [eax]).ResumeOffset, 0
		invoke SetFilePointer, hFile, ecx, NULL, FILE_BEGIN
		cmp eax, -1
		je STORFail2
	.ENDIF

@@:
	;recv file
	invoke recv, DTPSock, RecvBuff, SendBuffSize, 0
	test eax, eax
	jz STORDone
	cmp eax, SOCKET_ERROR
	je STORDone	

	;Write
	lea ecx, BytesWritten
	invoke WriteFile, hFile, RecvBuff, eax, ecx, NULL
	test eax, eax
	jz STORFail2
	jmp @B

STORDone:
	;Success!
	invoke CloseHandle, hFile
	invoke SendReply, ServSock, addr szRep45
	ret

STORFail2:
	invoke CloseHandle, hFile
STORFail1:
	invoke SendReply, ServSock, addr szRep53
	ret
DoSTORCommand ENDP