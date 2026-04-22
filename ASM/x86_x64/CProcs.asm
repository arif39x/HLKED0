strnicmp PROTO :DWORD, :DWORD, :DWORD
StrToDword PROTO :DWORD, :DWORD
GAllocNoFail PROTO :DWORD

.code

strnicmp PROC String1:DWORD, String2:DWORD, Count:DWORD
	invoke CompareString, 0, NORM_IGNORECASE, String1, Count, String2, Count
	dec eax
	dec eax
	ret
strnicmp ENDP

;Ohh this one looks a lot like strtoul eh?
;Only diffrence is you cant choose the base
StrToDword PROC szString:DWORD, lpStoppedAddy:DWORD
	mov esi, szString
	xor eax, eax
@@:
	lodsb
	cmp al, '0'
	jl @F
	cmp al, '9'
	jg @F
	jmp @B
@@:
	dec esi
	mov byte ptr [esi], 0
	push eax
	mov eax, lpStoppedAddy
	mov [eax], esi
	invoke atodw, szString
	pop ebx
	mov byte ptr [esi], bl
	ret
StrToDword ENDP
	
;Not exactly like any C function.. but it is nice to know that Global Alloc aint gonna fail! :-)
GAllocNoFail PROC HowMuch:DWORD
@@:
	invoke GlobalAlloc, GMEM_FIXED, HowMuch
	test eax, eax
	jz @B
	ret
GAllocNoFail ENDP
