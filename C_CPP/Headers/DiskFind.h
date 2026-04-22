//version 2.5 (12.10.2000)

#ifndef __DiskFind_h
#define __DiskFind_h

#include <windows.h>

#define MT_UPCASE_MASK		0x80000000
#define MT_UPCASE_STRING	0x40000000
#define MT_UPCASE_BOTH		0xC0000000


#define DF_FILES			0x01
#define DF_SDIRS_AFTER		0x02
#define DF_SDIRS_OVER		0x04
#define DF_SDIRS_BEFORE		0x08
#define DF_USE_DIRS			0x10
#define DF_SDIRS_TERM		0x20 //not for use!!!
#define DF_RECURSE			0x80

#define DF_SDIRS_REC_AFTER	DF_RECURSE|DF_SDIRS_AFTER
#define DF_SDIRS_REC_OVER	DF_RECURSE|DF_SDIRS_OVER
#define DF_SDIRS_REC_BEFORE	DF_RECURSE|DF_SDIRS_BEFORE


extern "C" BYTE MaskTest(char* Mask, char* InString);
extern "C" BYTE MaskTest_Selective(char* Mask, char* InString, DWORD Params);

extern "C" char* UpCase_Win(char* InString);

extern "C" DWORD DF_SDirFunctor; //(char* InDirectory)
extern "C" DWORD DF_FileFunctor; //(char* FullPath_with_FileName, char* FileName, char* LocalPath, char* FullPath)

extern "C" void DiskFind(char* InDirectory, char* Mask, DWORD Params);
extern "C" void RARList(char* RarFile, char* TextMask, void(*FileFunctor)(char*, char*, char*, char*), void(*SDirFunctor)(char*));
extern "C" void ZIPList(char* ZipFile, char* TextMask, void(*FileFunctor)(char*, char*, char*, char*), void(*SDirFunctor)(char*));
extern "C" void CABList(char* CabFile, char *TextMask, void(*FileFunctor)(char*, char*, char*, char*), void(*SDirFunctor)(char*));
/*
void RARList(char *rar,char *mask,void(*dest_fnc)(char*),void(*stat_fnc)(char*))
{
	HANDLE  rarfile;
	char	fname[255];
	rarfile=CreateFile(rar,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if (rarfile==INVALID_HANDLE_VALUE)
		return;
	if (stat_fnc!=NULL)
		stat_fnc(rar);
	DWORD dw_data,c_pos,pack_size,temp,f_size;
	BYTE  b_data;
	WORD  w_data,fname_len;
	ReadFile(rarfile,&dw_data,4,&temp,0);
	if (dw_data!=0x21726152)
	{
		CloseHandle(rarfile);
		return;
	}
	f_size=SetFilePointer(rarfile,0,0,FILE_END);
	SetFilePointer(rarfile,20,0,FILE_BEGIN);
	while (1)
	{
		c_pos=SetFilePointer(rarfile,2,0,FILE_CURRENT);
		if (c_pos>=f_size)
		{
			CloseHandle(rarfile);
			return;
		}
		ReadFile(rarfile,&b_data,1,&temp,0);
		if (b_data!=0x74)
		{
			CloseHandle(rarfile);
			return;
		}
		SetFilePointer(rarfile,2,0,FILE_CURRENT);
		ReadFile(rarfile,&w_data,2,&temp,0);
		ReadFile(rarfile,&pack_size,4,&temp,0);
		SetFilePointer(rarfile,15,0,FILE_CURRENT);
		ReadFile(rarfile,&fname_len,2,&temp,0);
		SetFilePointer(rarfile,4,0,FILE_CURRENT);
		ReadFile(rarfile,&fname,fname_len,&temp,0);
		fname[fname_len]=0;
		WebTrans(fname,0);
		if (MaskTest(StrUpr(mask),StrUpr(fname+GetCharPos(fname,'/',DOWN)+1)))
			dest_fnc(fname);
		SetFilePointer(rarfile,c_pos+w_data+pack_size-2,0,FILE_BEGIN);
	}
	CloseHandle(rarfile);
}
*//*
DWORD GetCharPos(char *src_str,BYTE sym, bool direction)
{
        WORD    sel=lstrlen(src_str);
        DWORD   rv=0;
        __asm
        {
                mov             edi,dword ptr src_str
                mov     esi, edi
                xor     ecx, ecx
                mov             cx,word ptr sel
                mov             al,sym
                
                cmp             direction,0
                je              up
                std
                add             edi,ecx
                jmp             start
up:
                cld
start:
                repne   scasb
                jne             end
                sub             edi,esi
                cmp             direction,0
                je              up_dir
                inc             edi
                mov             rv,edi
                jmp             exit_
up_dir:
                dec             edi
                mov             rv,edi
                jmp             exit_
end:
                xor             edi,edi
                dec             edi
                mov             rv,edi
exit_:
        }
        return  rv;
}*//*
void WebTrans(char* String)
{
	int counter=0;
	while(String[counter]!='\0')
	{
		if(String[counter]=='\\') String[counter]='/';
		counter++;
	}
}*/
/*
void RARList(char *rar,char *mask,void(*dest_fnc)(char*,char*,char*),void(*stat_fnc)(char*))
{
        HANDLE  rarfile;
        char    fname[255];
        DWORD dw_data=0,c_pos=0,pack_size=0,temp,f_size=0,fname_len=0;
        BYTE  b_data=0;
        WORD  w_data=0;

        __asm
        {
                push    0
                push    0
                push    OPEN_EXISTING
                push    0
                push    FILE_SHARE_READ
                push    GENERIC_READ
                push    dword ptr rar
                call    dword ptr [CreateFile]
                cmp             eax,-1
                je              RL_File_Open_Error
                mov             dword ptr rarfile,eax
                cmp             dword ptr stat_fnc,0
                je              RL_No_Stat
                push    dword ptr rar
                call    dword ptr [stat_fnc]
                add             esp,4
RL_No_Stat:
                push    0
                lea             ebx,dword ptr temp
                push    ebx
                push    4
                lea             ebx,dword ptr dw_data
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [ReadFile]
                cmp             dword ptr dw_data,21726152h
                jne             RL_Fatal_Error
                push    FILE_END
                push    0
                push    0
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
                mov             dword ptr f_size,eax
                push    FILE_BEGIN
                push    0
                push    20
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
RL_Main_Enum_Loop:
                push    FILE_CURRENT
                push    0
                push    2
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
                mov             dword ptr c_pos,eax
                cmp             eax,dword ptr f_size
                jae             RL_Fatal_Error
                push    0
                lea             ebx,dword ptr temp
                push    ebx
                push    1
                lea             ebx,byte ptr b_data
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [ReadFile]
                cmp             byte ptr b_data,74h
                jne             RL_Fatal_Error
                push    FILE_CURRENT
                push    0
                push    2
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
                push    0
                lea             ebx,dword ptr temp
                push    ebx
                push    2
                lea             ebx,word ptr w_data
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [ReadFile]
                push    0
                lea             ebx,dword ptr temp
                push    ebx
                push    4
                lea             ebx,dword ptr pack_size
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [ReadFile]
                push    FILE_CURRENT
                push    0
                push    15
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
                push    0
                lea             ebx,dword ptr temp
                push    ebx
                push    2
                lea             ebx,dword ptr fname_len
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [ReadFile]
                push    FILE_CURRENT
                push    0
                push    4
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
                push    0
                lea             ebx,dword ptr temp
                push    ebx
                push    dword ptr fname_len
                lea             ebx,dword ptr fname
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [ReadFile]
                lea             esi,dword ptr fname
                mov             ebx,dword ptr fname_len
                mov             [esi+ebx],0
                push    0
                push    esi
                call    WebTrans
                add             esp,8
                push    0//DOWN
                push    '/'
                push    esi
                call    GetCharPos
                add             esp,12
                inc             eax
                mov             ebx,esi
                add             ebx,eax

				push	MT_BOTH_UPCASE
                push    ebx
                push    dword ptr mask
                call    MaskTest_Selective
                add             esp,12
                
				cmp             al,1
                jne             RL_Wrong_Mask
                push    0
                push    0
                lea             esi,dword ptr fname
                push    esi
                call    dword ptr [dest_fnc]
                add             esp,12
RL_Wrong_Mask:
                push    FILE_BEGIN
                push    0
                mov             ebx,dword ptr c_pos
                xor             ecx,ecx
                mov             cx,word ptr w_data
                add             ebx,ecx
                add             ebx,dword ptr pack_size
                sub             ebx,2
                push    ebx
                push    dword ptr rarfile
                call    dword ptr [SetFilePointer]
                jmp             RL_Main_Enum_Loop
RL_Fatal_Error:
                push    dword ptr rarfile
                call    dword ptr [CloseHandle]
RL_File_Open_Error:
        }
        return;
}
*/

/*
void RARList(char* RarFile, char* TextMask, void(*FileFunctor)(char*, char*, char*, char*), void(*SDirFunctor)(char*))
{
	HANDLE  RarHandle;
	char    PackFile_Name[1024];
//	DWORD	tmpData=0,PackFile_Size=0,BytesRead,RarFile_Size=0,PackFile_Name_Size=0,CurrentPosition=0, PackFile_Attrib, PackFile_Path_Size;
	DWORD	BytesRead,RarFile_Size,CurrentPosition=0,PackFile_Path_Size;

	DWORD	PackFile_Attrib;
	WORD	PackFile_Name_Size;
	BYTE	temp6;
	WORD	temp5;
	DWORD	temp4;
	DWORD	temp3;
	DWORD	temp2;
	DWORD	PackFile_Size;
	WORD	tmpData;
	WORD	temp1;

	__asm
	{
		mov		byte ptr [PackFile_Name], 0

		push	dword ptr [RarFile]
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call	dword ptr [lstrcpy]
		push	eax
		call	dword ptr [lstrlen]
		mov		dword ptr [PackFile_Path_Size], eax
		inc		dword ptr [PackFile_Path_Size]
		mov		byte ptr [PackFile_Name+eax], '/'
		mov		byte ptr [PackFile_Name+eax+1], 0

		push	0
		push	0
		push	OPEN_EXISTING
		push	0
		push	FILE_SHARE_READ
		push	GENERIC_READ
		push	dword ptr [RarFile]
		call	dword ptr [CreateFile]
		cmp		eax, -1
		je		RL_FileError
		mov		dword ptr [RarHandle], eax
		cmp		dword ptr [SDirFunctor], 0
		je		RL_StartAnalize
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call    dword ptr [SDirFunctor]
		add		esp, 4
RL_StartAnalize:
		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
		push	4
		lea		eax, dword ptr [tmpData]
		push	eax
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

		cmp		dword ptr [tmpData], '!raR'//21726152h //"Rar!"
		jne		RL_FileEnd

		push	FILE_END
		push	0
		push	0
		push	dword ptr [RarHandle]
		call	dword ptr [SetFilePointer]
		mov		dword ptr [RarFile_Size], eax

		push	FILE_BEGIN
		push	0
		push	22
		push	dword ptr [RarHandle]
		call	dword ptr [SetFilePointer]
		mov		dword ptr [CurrentPosition], eax

RL_MainCycle:

		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
		push	1
		lea		eax, byte ptr [tmpData]
		push	eax
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

		cmp		byte ptr [tmpData], 't'//74h
		jne		RL_FileEnd

		push	FILE_CURRENT
		push	0
		push	2
		push	dword ptr [RarHandle]
		call	dword ptr [SetFilePointer]

		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
		push	2
		lea		eax, word ptr [tmpData]
		push	eax
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
		push	4
		lea		eax, dword ptr [PackFile_Size]
		push	eax
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

		push	FILE_CURRENT
		push	0
		push	15
		push	dword ptr [RarHandle]
		call	dword ptr [SetFilePointer]

		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
		push	2
//		lea		eax, dword ptr [PackFile_Name_Size]
		lea		eax, word ptr [PackFile_Name_Size]
		push	eax
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
		push	4
		lea		eax, dword ptr [PackFile_Attrib]
		push	eax
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

		push	0
		lea		eax, dword ptr [BytesRead]
		push	eax
//		push	dword ptr [PackFile_Name_Size]

//		push	word ptr [PackFile_Name_Size]
		xor		eax, eax
		mov		ax, word ptr [PackFile_Name_Size]
		push	eax

//		mov		esi, dword ptr [PackFile_Path_Size]
		xor		esi, esi
		mov		si, word ptr [PackFile_Path_Size]
		lea		esi, dword ptr [PackFile_Name+esi]
		push	esi
		push	dword ptr [RarHandle]
		call	dword ptr [ReadFile]

//		mov		eax, dword ptr [PackFile_Name_Size]
		xor		eax, eax
		mov		ax, word ptr [PackFile_Name_Size]
		mov		byte ptr [esi+eax], 0

		mov		ebx, esi
		dec		ebx
		mov		eax, esi
RL_SlashCycle:
		cmp		byte ptr [eax], '\\'
		jne		RL_SlashCycle_Next
		mov		byte ptr [eax], '/'
		mov		ebx, eax
RL_SlashCycle_Next:
		inc		eax
		cmp		byte ptr [eax], 0
		jne		RL_SlashCycle

		test	dword ptr [PackFile_Attrib], 10h
		jz		RL_DirNotFound
//		mov		eax, dword ptr [PackFile_Path_Size]
//		add		eax, dword ptr [PackFile_Name_Size]
		xor		eax, eax
		mov		ax, word ptr [PackFile_Name_Size]
		add		eax, dword ptr [PackFile_Path_Size]
		mov		byte ptr [PackFile_Name+eax], '/'
		mov		byte ptr [PackFile_Name+eax+1], 0
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call	dword ptr [SDirFunctor]
		add		esp,4
RL_DirNotFound:

		inc		ebx
		push	MT_BOTH_UPCASE
		push	ebx
		push	dword ptr [TextMask]
		call	MaskTest_Selective
		add		esp, 12

		cmp		al, 1
		jne		RL_Wrong_Mask

		push	dword ptr [RarFile]
		mov		eax, dword ptr [PackFile_Path_Size]
		lea		eax, dword ptr [PackFile_Name+eax]
		push	eax
		push	ebx
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call	dword ptr [FileFunctor]
		add		esp, 16
RL_Wrong_Mask:
		push	FILE_BEGIN
		push	0
		mov		ebx, dword ptr [CurrentPosition]
		xor		ecx, ecx
		mov		cx, word ptr [tmpData]
		add		ebx, ecx
		add		ebx, dword ptr [PackFile_Size]
		push	ebx
		push	dword ptr [RarHandle]
		call	dword ptr [SetFilePointer]
		mov		dword ptr [CurrentPosition], eax

		cmp		eax, dword ptr [RarFile_Size]
		jb		RL_MainCycle
RL_FileEnd:
		push	dword ptr [RarHandle]
		call	dword ptr [CloseHandle]
RL_FileError:
        }
        return;
}
*/
/*
void ZIPList(char* ZipFile, char* TextMask, void(*FileFunctor)(char*, char*, char*, char*), void(*SDirFunctor)(char*))
{
	DWORD	PackFile_Name_Size=0, ExtentedInfo=0, PackFile_Path_Size;
	DWORD	tmpData, PackFile_Size=0, ZipFile_Size;
	HANDLE	ZipHandle;
	char	PackFile_Name[1024];

	__asm
	{
		mov		byte ptr [PackFile_Name], 0

		push	dword ptr [ZipFile]
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call	dword ptr [lstrcpy]
		push	eax
		call	dword ptr [lstrlen]
		mov		dword ptr [PackFile_Path_Size], eax
		inc		dword ptr [PackFile_Path_Size]
		mov		byte ptr [PackFile_Name+eax], '/'
		mov		byte ptr [PackFile_Name+eax+1], 0

		push	0
		push	0
		push	OPEN_EXISTING
		push	0
		push	FILE_SHARE_READ
		push	GENERIC_READ
		push	dword ptr [ZipFile]
		call	dword ptr [CreateFile]
		cmp		eax, -1
		je		ZL_FileError
		mov		dword ptr [ZipHandle], eax
		cmp		dword ptr [SDirFunctor], 0
		je		ZL_StartAnalize
		push	dword ptr [ZipFile]
		call	dword ptr [SDirFunctor]
		add		esp, 4
ZL_StartAnalize:
		push	FILE_END
		push	0
		push	0
		push	dword ptr [ZipHandle]
		call	dword ptr [SetFilePointer]
		mov		dword ptr [ZipFile_Size], eax
		push	FILE_BEGIN
		push	0
		push	0
		push	dword ptr [ZipHandle]
		call	dword ptr [SetFilePointer]
ZL_MainCycle:
		push	FILE_CURRENT
		push	0
		mov		eax, 18
		add		eax, dword ptr [ExtentedInfo]
		add		eax, dword ptr [PackFile_Size]
		push	eax
		push	dword ptr [ZipHandle]
		call	dword ptr [SetFilePointer]
		push	0
		lea		eax, dword ptr [tmpData]
		push	eax
		push	4
		lea		eax, dword ptr [PackFile_Size]
		push	eax
		push	dword ptr [ZipHandle]
		call	dword ptr [ReadFile]
		push	FILE_CURRENT
		push	0
		push	4
		push	dword ptr [ZipHandle]
		call	dword ptr [SetFilePointer]
		push	0
		lea		eax, dword ptr [tmpData]
		push	eax
		push	2
		lea		eax, dword ptr [PackFile_Name_Size]
		push	eax
		push	dword ptr [ZipHandle]
		call	dword ptr [ReadFile]
		push	0
		lea		eax, dword ptr [tmpData]
		push	eax
		push	2
		lea		eax, dword ptr [ExtentedInfo]
		push	eax
		push	dword ptr [ZipHandle]
		call	dword ptr [ReadFile]
		push	0
		lea		eax, dword ptr [tmpData]
		push	eax
		mov		eax, dword ptr [PackFile_Name_Size]
		push	eax
		mov		esi, dword ptr [PackFile_Path_Size]
		lea		esi, dword ptr [PackFile_Name+esi]
		mov		byte ptr [esi+eax], 0
		push	esi
		push	dword ptr [ZipHandle]
		call	dword ptr [ReadFile]

		mov		eax, dword ptr [ZipFile_Size]
		sub		eax, 18
		sub		eax, dword ptr [ExtentedInfo]
		sub		eax, dword ptr [PackFile_Size]
		mov		dword ptr [ZipFile_Size], eax
		cmp		dword ptr [ZipFile_Size], 0
		jle		ZL_FileEnd
		cmp		dword ptr [PackFile_Name_Size], 0
		jz		ZL_DataError
		add		esi, dword ptr [PackFile_Name_Size]
ZL_FindSlash:
		dec		esi
		cmp		byte ptr [esi], '/'
		jne		ZL_FindSlash
		inc		esi

		cmp		byte ptr [esi], 0
		je		ZL_DirFound

		push	MT_MASK_UPCASE or MT_STRING_UPCASE
		push	esi
		push	dword ptr [TextMask]
		call	[MaskTest_Selective]
		add		esp, 12
		cmp		al, 1
		jne		ZL_DataError

		push	dword ptr [ZipFile]
		mov		eax, dword ptr [PackFile_Path_Size]
		lea		eax, dword ptr [PackFile_Name+eax]
		push	eax
		push	esi
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call	dword ptr [FileFunctor]
		add		esp, 16
		jmp		ZL_DataError
ZL_DirFound:
		lea		eax, dword ptr [PackFile_Name]
		push	eax
		call	dword ptr [SDirFunctor]
		add		esp,4
ZL_DataError:
		jmp		ZL_MainCycle
ZL_FileEnd:
		push	dword ptr ZipHandle
		call	dword ptr [CloseHandle]
ZL_FileError:
	}
}
*/
/*
void CABList(char* CabFile,char *TextMask,void(*FileFunctor)(char*, char*, char*, char*),void(*SDirFunctor)(char*))
{
	HANDLE	CabHandle;
	DWORD	tmpData, tmpDataOffset, BytesRead, Path_Size;
	WORD	NumberOfFiles;
	char	FullName[255];
	char	Name_UpCased[255];
	BYTE	tmpByte;
	__asm
	{
		push	dword ptr [TextMask]
		call	UpCase_Win
		add		esp,4

		push	dword ptr [CabFile]
		lea		eax, dword ptr [FullName]
		push	eax
		call	dword ptr [lstrcpy]
		push	eax
		call	dword ptr [lstrlen]
		mov		dword ptr [Path_Size], eax
		inc		dword ptr [Path_Size]
		mov		byte ptr [FullName+eax], '/'
		mov		byte ptr [FullName+eax+1], 0

		push	0
		push	0
		push	OPEN_EXISTING
		push	0
		push	FILE_SHARE_READ
		push	GENERIC_READ
		push	dword ptr [CabFile]
		call	dword ptr [CreateFile]
		cmp		eax, -1
		je		CL_File_Error
		mov		dword ptr [CabHandle], eax;		
		cmp		dword ptr [SDirFunctor],0
		je		CL_No_Stat
		lea		eax, dword ptr [FullName]
		push	eax
		call	dword ptr [SDirFunctor]
		add		esp,4
CL_No_Stat:
		push	0
		lea		eax,dword ptr BytesRead
		push	eax
		push	4
		lea		eax,dword ptr tmpData
		push	eax
		push	dword ptr [CabHandle]
		call	dword ptr [ReadFile]
		cmp		dword ptr [tmpData],'FCSM' //"FCSM" =? "MSCF"
		jne		CL_Bad_Sign
		push	FILE_CURRENT
		push	0
		push	12
		push	dword ptr [CabHandle]
		call	dword ptr [SetFilePointer]
		push	0
		lea		eax,dword ptr [BytesRead]
		push	eax
		push	4
		lea		eax,dword ptr [tmpDataOffset]
		push	eax
		push	dword ptr [CabHandle]
		call	dword ptr [ReadFile]
		push	FILE_CURRENT
		push	0
		push	8
		push	dword ptr [CabHandle]
		call	dword ptr [SetFilePointer]
		push	0
		lea		eax,dword ptr BytesRead
		push	eax
		push	2
		lea		eax,word ptr NumberOfFiles
		push	eax
		push	dword ptr [CabHandle]
		call	dword ptr [ReadFile]
		push	FILE_BEGIN
		push	0
		push	dword ptr [tmpDataOffset]
		push	dword ptr [CabHandle]
		call	dword ptr [SetFilePointer]
CL_Main_Enum_Loop:
		cmp		word ptr NumberOfFiles,0
		je		CL_Loop_End
		push	FILE_CURRENT
		push	0
		push	16
		push	dword ptr [CabHandle]
		call	dword ptr [SetFilePointer]
        mov     esi, dword ptr [Path_Size]
        lea     esi, dword ptr [FullName+esi]
		xor		edi,edi
		mov		byte ptr tmpByte,1
CL_Internal_Loop:
		cmp		byte ptr tmpByte,0
		je		CL_Main_Continue
		push	0
		lea		eax,dword ptr [BytesRead]
		push	eax
		push	1
		lea		eax,byte ptr [tmpByte]
		push	eax
		push	dword ptr [CabHandle]
		call	dword ptr [ReadFile]
		mov		al,byte ptr [tmpByte]
		mov		[esi+edi],al
		inc		edi
		jmp		CL_Internal_Loop
CL_Main_Continue:
		push	esi
		lea		eax, dword ptr [Name_UpCased]
		push	eax
		call	dword ptr [lstrcpy]
		push	eax
		call	UpCase_Win
		add		esp,4
		push	eax
		push	dword ptr [TextMask]
		call	MaskTest
		add		esp,8
		cmp		al,1
		jne		CL_Bad_Mask
		push	dword ptr [CabFile]
		push	esi	
		push	esi	
        lea     eax, dword ptr [FullName]
        push    eax
		call	dword ptr [FileFunctor]
		add		esp, 16
CL_Bad_Mask:
		dec		word ptr [NumberOfFiles]
		jmp		CL_Main_Enum_Loop
CL_Loop_End:
CL_Bad_sign:
		push	dword ptr [CabHandle]
		call	dword ptr [CloseHandle]
CL_File_Error:
	}
	return;
}
*/

#endif