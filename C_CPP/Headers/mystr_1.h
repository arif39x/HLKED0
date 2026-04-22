/////////////////////////////////////////////////
// Строковые функции
//		by 3LblDEHb [Team 16]
/////////////////////////////////////////////////
#ifndef __my_str_h
#define __my_str_h

#include <windows.h>

#define DOWN	TRUE
#define UP		FALSE

/////////////////////////////////////////////////
// Приводит строку к верхнему регистру
/////////////////////////////////////////////////
char* StrUpr(char* InString)
{
	__asm
	{
		mov esi, dword ptr [InString]
SU_Cycle:
		cmp byte ptr [esi], 184
		jb SU_EnglishLetter
		cmp byte ptr [esi], 184
		jne SU_Russian_Normal
		mov byte ptr [esi], 168
		jmp SU_IncContinue
SU_Russian_Normal:
		cmp byte ptr [esi], 224
		jb SU_IncContinue
		sub byte ptr [esi], 32
		jmp SU_IncContinue
SU_EnglishLetter:
		cmp byte ptr [esi], 97
		jb SU_IncContinue
		cmp byte ptr [esi], 122
		ja SU_IncContinue
		sub byte ptr [esi], 32
		jmp SU_IncContinue
SU_IncContinue:
		inc esi
		cmp byte ptr [esi], 0
		jne SU_Cycle
	}
	return InString;
}

/////////////////////////////////////////////////////////////
// Находит первое вхождение символа в строку
// direction=UP=0 - поиск от начала =DOWN=1 - поиск от конца
/////////////////////////////////////////////////////////////
DWORD GetCharPos(char *src_str,BYTE sym, bool direction)
{
	WORD	sel=lstrlen(src_str);
	DWORD	rv=0;
	__asm
	{
		mov		edi,dword ptr src_str
		mov     esi, edi
		xor     ecx, ecx
		mov		cx,word ptr sel
		mov		al,sym
		
		cmp		direction,0
		je		up
		std
		add		edi,ecx
		jmp		start
up:
		cld
start:
		repne	scasb
		jne		end
		sub		edi,esi
		cmp		direction,0
		je		up_dir
		inc		edi
		mov		rv,edi
		jmp		exit_
up_dir:
		dec		edi
		mov		rv,edi
		jmp		exit_
end:
		xor		edi,edi
		dec		edi
		mov		rv,edi
exit_:
	}
	return  rv;
}
//////////////////////////////////////////////
// Сдвигает строку
// PanString("123456",3) -> "456"
//////////////////////////////////////////////
char *PanString(char *instring,DWORD pan)
{
	DWORD	in_len=lstrlen(instring);
	__asm
	{
		cld
		mov		ebx,dword ptr instring
		mov		eax,pan
		mov		ecx,in_len
		mov		edi,ebx
		add		ebx,eax
		mov		esi,ebx
		rep		movsb		
	}
	return instring;
}
/////////////////////////////////////////////////////
// Убирает каталог верхнего уровня и заменяет слэши
/////////////////////////////////////////////////////
char* WebTrans(char* instring,bool DelFirstLevel)
{
	DWORD	in_len=lstrlen(instring);
	__asm
	{
WT_Start:		
		xor		edx,edx
		mov		ebx,dword ptr instring
		mov		dl,'/'

		push	0
		push	'\\'
		push	ebx
		call	GetCharPos
		add		esp,12
		cmp		eax,-1
		jz		WT_Mov_Str
		mov		[ebx+eax],dl
		jmp		WT_Start
WT_Mov_Str:
		cmp		dword ptr DelFirstLevel,1
		jne		WT_Norm_End
		push	0
		push	'/'
		push	ebx
		call	GetCharPos
		add		esp,12
		cld
		mov		ecx,in_len
		mov		edi,ebx
		inc		eax
		add		ebx,eax
		mov		esi,ebx
		rep		movsb
		jmp		WT_Norm_End
	}
	return 0;
	__asm
	{
WT_Norm_End:
	}
	return instring;
}
//////////////////////////////////////////////////
// Удаляет пробелы от строки и до начала символов
// (не пробелы)
//////////////////////////////////////////////////
char *DelSpace(char *instring)
{
	DWORD ins_len=lstrlen(instring);
	__asm
	{
		mov		ebx,dword ptr instring
		mov		ecx,ins_len
		dec		ecx
start:
		cmp		[ebx+ecx],' '
		jnz		exit_prc
		mov		[ebx+ecx],0
		dec		ecx
		jns		start
exit_prc:
	}
	return instring;
}
#endif