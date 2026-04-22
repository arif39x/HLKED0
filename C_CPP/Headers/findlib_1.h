#ifndef __findlib_h
#define __findlib_h

#include <windows.h>
//#include <wininet.h>
//#include "mystr.h"
//#include "diskfind.h"

#define	FTPF_USE_PROXY	0x01
#define	FTPF_SHOW_STAT	0x02

void NetStartFind(NETRESOURCE *,void(*)(NETRESOURCE));
//BOOL FTPSearch(char*,char*,DWORD);
void NetFind(HANDLE,void(*)(NETRESOURCE));

void NetFind(HANDLE hEnum,void(*dest_fnc)(NETRESOURCE))
{
	NETRESOURCE	nr[1024];
	DWORD		NetSize=32768;
	DWORD		NetCount=1024;
	DWORD		EnumCode=WNetEnumResource(hEnum,&NetCount,nr,&NetSize);
	if (EnumCode==NO_ERROR)
	{
		for (int a=0; (a<1024)&&(nr[a].dwUsage!=0); a++)
		{
				if (nr[a].dwDisplayType==RESOURCEDISPLAYTYPE_SHARE)
					dest_fnc(nr[a]);
				else
					NetStartFind(&nr[a],dest_fnc);
		}
	}
	return;
}

void EnumConnected(void(*dest_fnc)(NETRESOURCE))
{
	HANDLE hConnectedEnum;
	NETRESOURCE	nr[1024];
	DWORD		NetSize=32768;
	DWORD		NetCount=1024;
	WNetOpenEnum(RESOURCE_CONNECTED,RESOURCETYPE_DISK,0,nr,&hConnectedEnum);
	DWORD		EnumCode=WNetEnumResource(hConnectedEnum,&NetCount,nr,&NetSize);
	if ()
	WNetCloseEnum(hConnectedEnum);
}

void NetStartFind(NETRESOURCE *nr,void(*dest_fnc)(NETRESOURCE))
{
	HANDLE hConnectedEnum;
	WNetOpenEnum(RESOURCE_GLOBALNET,RESOURCETYPE_DISK,0,nr,&hConnectedEnum);
	NetFind(hConnectedEnum,dest_fnc);
	WNetCloseEnum(hConnectedEnum);
	return;
}
/*
BOOL FTPSearch(char *DestFile,char *ProxyAdr,DWORD Flags)
{
	HINTERNET	inet,url;	
	HANDLE		file;
	char		*gfs="gfs";
	char		*agtype="MSIE 5.0";
	char		*msg_1="Ошибка при попытке открытия Интернет-сессии";
	char		*msg_2="Подключение к ftpsearch.lycos.com";
	char		*msg_3="Ошибка при подключении к ftpsearch.lycos.com";
	char		*msg_4="Загрузка результатов";
	char		*msg_5="Ошибка выделения памяти";
	char		buf[1024];
	char		t_buf[255];
	DWORD		temp,fsize;
	char		url_1[255]="http://ftpsearch.lycos.com/cgi-bin/search?form=normal&query=",url_2[]="&doit=Search&type=Case+insensitive+glob+search&hits=200";
	DWORD		blk_adr=0x413C2F3E;
	DWORD		pre_b=0x4552503C;
	DWORD		pre_e=0x52502F3C;
	DWORD		adr_start,adr_len;
	DWORD		cur_blk,cur_pos=0; 
	bool		ParseBegin=0;
	char		*pbuf;

	__asm
	{
		lea		esi,byte ptr buf			// Создаем имя для Temp-файла
		push	esi
		push	1024
		call	dword ptr [GetTempPath]
		lea		edi,byte ptr t_buf
		push	edi
		push	0
		push	dword ptr gfs
		push	esi
		call	dword ptr [GetTempFileName]
		push	dword ptr DestFile
		lea		esi,byte ptr url_1			// Делаем URL
		push	esi
		call	dword ptr [lstrcat]
		lea		esi,byte ptr url_2
		push	esi
		push	eax
		call	dword ptr [lstrcat]
		push	0
		push	0
		push	OPEN_ALWAYS
		push	0
		push	0
		push	0xC0000000
		lea		esi,byte ptr t_buf
		push	esi
		call	dword ptr [CreateFile]		// Открываем темповый файл
		cmp		eax,-1
		je		FTPF_Error_Open_File
		mov		dword ptr file,eax
		test	dword ptr Flags,FTPF_USE_PROXY
		jne		FTPF_No_Use_Proxy
		push	0
		push	0
		push	dword ptr ProxyAdr
		push	INTERNET_OPEN_TYPE_PROXY
		push	dword ptr agtype;
		call	dword ptr [InternetOpen]	// Открываем ИНет-сессию для прокси
		jmp		FTPF_Session_Opened
FTPF_No_Use_Proxy:
		push	0
		push	0
		push	0
		push	INTERNET_OPEN_TYPE_DIRECT
		push	dword ptr agtype;
		call	dword ptr [InternetOpen]	// Открываем ИНет-сессию без прокси
FTPF_Session_Opened:
		cmp		eax,0
		je		FTPF_INet_Open_Fail
		mov		dword ptr inet,eax
		test	dword ptr Flags,FTPF_SHOW_STAT
		jne		FTPF_No_Stat_I
		push	dword ptr msg_2
		call	dword ptr DF_SDirFunctor
		add		esp,4
FTPF_No_Stat_I:
		push	0
		push	0
		push	0
		push	0
		lea		esi,byte ptr url_1
		push	esi
		push	dword ptr inet
		call	dword ptr [InternetOpenUrl]		// Грузим пагу в кэш
		cmp		eax,0
		je		FTPF_Connect_Fail
		mov		dword ptr url,eax
		test	dword ptr Flags,FTPF_SHOW_STAT
		jne		FTPF_No_Stat_II
		push	dword ptr msg_4
		call	dword ptr DF_SDirFunctor
		add		esp,4
FTPF_No_Stat_II:
FTPF_Page_Loading_Loop:
		lea		esi,dword ptr temp
		push	esi
		push	1024
		lea		esi,byte ptr buf
		push	esi
		push	dword ptr url
		call	dword ptr [InternetReadFile]		// читаем 1024 бфйт из кэша
		push	0
		lea		esi,dword ptr temp
		push	esi
		push	dword ptr temp
		lea		esi,byte ptr buf
		push	esi
		push	dword ptr file
		call	dword ptr [WriteFile]				//	и пишем в файл
		cmp		dword ptr temp,0
		jne		FTPF_Page_Loading_Loop
		push	dword ptr url						// Отключаемся от нета
		call	dword ptr [InternetCloseHandle]
		push	dword ptr inet
		call	dword ptr [InternetCloseHandle]
// HTML Parsing part (непосредственно разбор АШТЭЭМЭЛины)
		push	FILE_END
		push	0
		push	0
		push	dword ptr file
		call	dword ptr [SetFilePointer]			// Узнаем размер файла
		mov		dword ptr fsize,eax
		push	eax
		push	GMEM_FIXED
		call	dword ptr [GlobalAlloc]				// И отжираем стока памяти
		cmp		eax,0
		jne		FTPF_Mem_Alloc_Success				//---
		push	dword ptr file						//	|
		call	dword ptr [CloseHandle]				//	|	если память выделена
		jmp		FTPF_Error_Open_File				//	|
FTPF_Mem_Alloc_Success:								//<--
		mov		dword ptr pbuf,eax
		push	FILE_BEGIN
		push	0
		push	0
		push	dword ptr file
		call	dword ptr [SetFilePointer]			// Стрейфимся в начало файла
		push	0
		lea		esi,dword ptr temp
		push	esi
		push	dword ptr fsize
		push	dword ptr pbuf
		push	dword ptr file
		call	dword ptr [ReadFile]				// Читаем его весь в память
		push	dword ptr file
		call	dword ptr [CloseHandle]				// и закрываем
FTPF_Page_Parsing_Loop:
		mov		eax,dword ptr cur_pos
		cmp		eax,dword ptr fsize
		jae		FTPF_Error_Open_File				// Файл еще не весь?
		cld
		mov		esi,dword ptr pbuf
		add		esi,cur_pos
		lea		edi,dword ptr cur_blk
		mov		ecx,2
		rep		movsw								// грузим анализируемый дворд
		cmp		dword ptr cur_blk,0x4552503C		// и понеслась...
		jne		FTPF_No_Blk_Begin
		xor		dword ptr ParseBegin,1				//	если это <PRE
		inc		dword ptr cur_pos					// (С него начинаются нужные
		jmp		FTPF_Page_Parsing_Loop				// нам данные)
FTPF_No_Blk_Begin:
		cmp		dword ptr cur_blk,0x413C2F3E		// >/<A 
		jne		FTPF_No_Adr_Blk_Begin				// Тобишь начало ссылки
		cmp		dword ptr ParseBegin,1
		jne		FTPF_No_Adr_Blk_Begin
		mov		eax,dword ptr cur_pos
		add		eax,11								// находим начало адреса
		mov		dword ptr adr_start,eax
		mov		esi,dword ptr pbuf
		cmp		[esi+eax],'f'
		jne		FTPF_Parse_continue
		cmp		[esi+eax],'F'						// проверка "особого случая"
		jne		FTPF_Parse_continue					// (в кавычках ли ссылка)
		dec		dword ptr adr_start
FTPF_Parse_continue:
		push	UP
		push	'>'
		add		esi,dword ptr adr_start
		push	esi
		call	GetCharPos							// Ищем конец ссылки
		cld
		lea		edi,dword ptr buf
		mov		ecx,dword ptr adr_len
		rep		movsb								// Грузим адрес ы буфер
		mov		esi,dword ptr pbuf
		add		esi,dword ptr adr_len
		cmp		[esi],'"'
		jne		FTPF_Simple							// Проверка на кавычки
		dec		dword ptr adr_len
FTPF_Simple:
		mov		esi,dword ptr pbuf
		mov		eax,dword ptr adr_len
		mov		[esi+eax],0							// терминатим стрингу нулем
		push	0
		push	0
		push	esi
		call	DF_FileFunctor						// и посылаем ее
		add		esp,12
		inc		dword ptr cur_pos
		jmp		FTPF_Page_Parsing_Loop
FTPF_No_Adr_Blk_Begin:
		cmp		dword ptr cur_blk,0x52502F3C		// </PR (</PRE>)
		jne		FTPF_Page_Parsing_Loop				// заканчиваем разбор
		cmp		dword ptr ParseBegin,1
		jne		FTPF_Page_Parsing_Loop
		push	dword ptr pbuf
		call	dword ptr [GlobalFree]
		lea		esi,dword ptr t_buf
		push	esi
		call	dword ptr [DeleteFile]
		jmp		FTPF_Error_Open_File
FTPF_Connect_Fail:
		push	dword ptr file
		call	dword ptr [CloseHandle]
		push	dword ptr inet
		call	dword ptr [InternetCloseHandle]
		test	dword ptr Flags,FTPF_SHOW_STAT
		jne		FTPF_Error_Open_File
		push	dword ptr msg_3
		call	dword ptr DF_SDirFunctor
		add		esp,4
		jmp		FTPF_Error_Open_File
FTPF_INet_Open_Fail:
		push	dword ptr file
		call	dword ptr [CloseHandle]
		test	dword ptr Flags,FTPF_SHOW_STAT
		jne		FTPF_Error_Open_File
		push	dword ptr msg_1
		call	dword ptr DF_SDirFunctor
		add		esp,4
FTPF_Error_Open_File:
	}
	return 1;
}

BOOL FTPSearchOld(char *ffile,void(*dest_fnc)(char*,char*,char*),bool stat,void(*stat_fnc)(char*),bool UseProxy, char *proxy_adr)
{
	HINTERNET	inet,url;	
	HANDLE		file;
	char		buf[1024];
	char		t_buf[255];
	DWORD		temp,cnt=0;
	char		url_1[255]="http://ftpsearch.lycos.com/cgi-bin/search?form=normal&query=",url_2[]="&doit=Search&type=Case+insensitive+glob+search&hits=200";
	

	GetTempPath(1024,buf);
	GetTempFileName(buf,"gfs",0,t_buf);
	lstrcat(lstrcat(url_1,ffile),url_2);

	file=CreateFile(t_buf,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,0,0);
	if (file==INVALID_HANDLE_VALUE)
		return 0;
	if (UseProxy)
		inet=InternetOpen("MSIE 5.0",INTERNET_OPEN_TYPE_PROXY,proxy_adr,NULL,NULL);
	else
		inet=InternetOpen("MSIE 5.0",INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,NULL);
	if (!inet)
	{
		CloseHandle(file);
		if (stat)
			stat_fnc("Ошибка при попытке открытия Интернет-сессии");
		return 0;
	}
	if (stat)
		stat_fnc("Подключение к ftpsearch.lycos.com");
	url=InternetOpenUrl(inet,url_1,NULL,NULL,NULL,NULL);
	if (!url)
	{
		CloseHandle(file);
		InternetCloseHandle(inet);
		if (stat)
			stat_fnc("Ошибка при подключении к ftpsearch.lycos.com");
		return 0;
	}
	if (stat)
		stat_fnc("Загрузка результатов");
	do
	{
		InternetReadFile(url,buf,1024,&temp);
		WriteFile(file,buf,temp,&temp,0);
	} while (temp!=0);
	InternetCloseHandle(url);
	InternetCloseHandle(inet);
	if (stat)
		stat_fnc("Разбор результатов поиска");
// HTML Parser -------------------------------------------
	DWORD	fs=SetFilePointer(file,0,0,FILE_END);
	SetFilePointer(file,0,0,FILE_BEGIN);
	DWORD	blk_adr=0x413C2F3E;
	DWORD	pre_b=0x4552503C;
	DWORD	pre_e=0x52502F3C;
	DWORD	adr_start,adr_len;
	DWORD	cur_blk,cur_pos=0; 
	char	adr_buf[1024];
	bool	ParseBegin=0;

	char	*pbuf=(char*)GlobalAlloc(GMEM_FIXED,fs);
	if (pbuf==NULL)
	{
		CloseHandle(file);
		if (stat)
			stat_fnc("Ошибка выделения памяти");
		return 0;
	}	

	ReadFile(file,pbuf,fs,&temp,0);
	CloseHandle(file);
	
	while (cur_pos<fs)
	{
		__asm
		{
			cld
			mov	esi,dword ptr pbuf
			add	esi,cur_pos
			lea	edi,dword ptr cur_blk
			mov	ecx,2
			rep	movsw
		}
		if (cur_blk==pre_b)
			ParseBegin^=1;
		else
		if ((cur_blk==blk_adr)&&(ParseBegin))
		{
			adr_start=cur_pos+11;
			if ((pbuf[adr_start]!='f')&&(pbuf[adr_start]!='F'))
				adr_start--;
			adr_len=GetCharPos(pbuf+adr_start,'>',UP);
			__asm
			{
				cld
				mov	esi,dword ptr pbuf
				add	esi,adr_start
				lea	edi,dword ptr adr_buf
				mov	ecx,dword ptr adr_len
				rep	movsb
			}
			if (adr_buf[adr_len]='"')
				adr_len--;
			adr_buf[adr_len]=0;	
			dest_fnc(adr_buf,0,0);
			cnt++;
		}
		else 
		if ((cur_blk==pre_e)&&(ParseBegin))
			break;
		cur_pos++;
	}
	
	GlobalFree(pbuf);
	DeleteFile(t_buf);
	return 1;
}*/
#endif