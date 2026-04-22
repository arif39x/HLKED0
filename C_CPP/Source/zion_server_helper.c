#include "zion_server_helper.h"


ULONG Safe_CopyMemFromUser(PVOID ptrBufferAddr,
						   ULONG size,
						   PVOID ptrDest)
{
	PMDL ptrUserMDL = IoAllocateMdl(ptrBufferAddr, 
		size, 
		FALSE, 
		FALSE, 
		NULL); 

	if(!ptrUserMDL) 
	{ 
		safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
		return -1; 
	} 
 
	MmProbeAndLockPages(ptrUserMDL,
						KernelMode,
						IoReadAccess
						);
	RtlCopyMemory(ptrDest,
					ptrBufferAddr,
					size);
	MmUnlockPages(ptrUserMDL);
	IoFreeMdl(ptrUserMDL);

	safe_trace("ZION_SERVER: AllocMdl succeed.\n");
	
	return 1;

}