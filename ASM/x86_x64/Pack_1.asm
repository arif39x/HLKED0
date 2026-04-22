.data
	@AllocStr	<szPackHeader>, <"PRIM">

.code

PackStream proc uses edi lpStream: DWORD
	LOCAL	inMem, inLen, outMem, workMem: DWORD
	LOCAL	dwQWord: QWORD	
	
	lea	edi, dwQWord
	mov	dword ptr[edi], 0
	mov	dword ptr[edi+4], 0
	coinvoke lpStream, IStream, Seek, dwQWord, S_END, addr dwQWord
	m2m	inLen, dword ptr[edi]
	
	; Seek beginning
	mov	dword ptr[edi], 0
	coinvoke lpStream, IStream, Seek, dwQWord, S_BEGIN, NULL
	
	; Read data into memory
	invoke	LocalAlloc, LMEM_FIXED, inLen
	mov	inMem, eax
	coinvoke lpStream, IStream, Read, inMem, inLen, NULL
	
	; Alloc outmem
	invoke	aP_max_packed_size, inLen
	invoke	LocalAlloc, LMEM_FIXED, eax
	mov	outMem, eax
	
	; Alloc work mem
	invoke	aP_workmem_size, inLen
	invoke	LocalAlloc, LMEM_FIXED, eax
	mov	workMem, eax

	; Pack
	invoke	aP_pack, inMem, outMem, inLen, workMem, NULL
	
	; Write packed data
	push	eax
	coinvoke lpStream, IStream, SetSize, dwQWord
	coinvoke lpStream, IStream, Seek, dwQWord, S_BEGIN, NULL
	coinvoke lpStream, IStream, Write, offset szPackHeader, szPackHeaderLen, NULL ; Write header
	coinvoke lpStream, IStream, Write, addr inLen, 4, NULL ; Write unpacked size
	pop	eax
	coinvoke lpStream, IStream, Write, outMem, eax, NULL ; Write the data
	
	; Free used memory
	invoke	LocalFree, outMem
	invoke	LocalFree, inMem
	invoke	LocalFree, workMem
	
	ret
PackStream endp