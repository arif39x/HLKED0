/*
    Screen Shot (Guptachar)
    Copyright (C) 2002 Ayan Chakrabarti

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    screen.cpp
    ==========

    Take a screenshot.
*/

#include <windows.h>

HBITMAP capturescreen(LPRECT lpRect) 
{ 
        HDC hScrDC, hMemDC; 
        HBITMAP hBitmap, hOldBitmap; 
        int nX, nY, nX2, nY2; 
        int nWidth, nHeight; 
	
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL); 
	hMemDC = CreateCompatibleDC(hScrDC); 
	
	nX = lpRect->left; 
	nY = lpRect->top; 
	nX2 = lpRect->right; 
	nY2 = lpRect->bottom; 
	
	nWidth = nX2 - nX; 
	nHeight = nY2 - nY; 
	
        
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight); 
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap); 
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY); 
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap); 
	
	DeleteDC(hScrDC); 
	DeleteDC(hMemDC); 
	
	return hBitmap; 
} 


BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, PSTR pszFileName) 
{ 
	int hFile; 
	OFSTRUCT ofReOpenBuff; 
	HBITMAP hTmpBmp, hBmpOld; 
	BOOL bSuccess; 
	BITMAPFILEHEADER bfh; 
	PBITMAPINFO pbmi; 
	PBYTE pBits; 
	BITMAPINFO bmi; 
	PBYTE pjTmp, pjTmpBmi; 
	ULONG sizBMI; 
	
	
	bSuccess = TRUE; 
        if (!hBmp)
                return FALSE; 
         
	
        bmi.bmiHeader.biSize = 0x28;
        bmi.bmiHeader.biBitCount = 0;
	if ((GetDIBits(hDC, hBmp, 0, 0, (LPSTR)NULL, &bmi, DIB_RGB_COLORS)) == 0)
                return FALSE; 
 
	
        if ((pBits = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,bmi.bmiHeader.biSizeImage)) == NULL)
                return FALSE; 

        pbmi = &bmi; 
	
        switch (bmi.bmiHeader.biBitCount)
        {
                case 24:
                                sizBMI = sizeof(BITMAPINFOHEADER);
                                break; 
                case 16: case 32: 
                                sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3; 
                                break; 
                default: 
                                sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(1<<bmi.bmiHeader.biBitCount); 
                                break; 
	} 
	
	if (sizBMI != sizeof(BITMAPINFOHEADER))
        {
                ULONG sizTmp; 

                if ((pbmi = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizBMI )) == NULL)
                {
                        bSuccess = FALSE; 
                        goto ErrExit1; 
                } 

                pjTmp = (PBYTE)pbmi;
                pjTmpBmi = (PBYTE)&bmi; 
                sizTmp = sizeof(BITMAPINFOHEADER); 
	
                while(sizTmp--) 
                        *(((PBYTE)pjTmp)++) = *((pjTmpBmi)++); 
	} 
	
        if ((hFile = OpenFile(pszFileName, (LPOFSTRUCT)&ofReOpenBuff,OF_CREATE | OF_WRITE)) == -1)
                goto ErrExit2; 
         
        bfh.bfType = 0x4D42; 
	bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizBMI+ 
	pbmi->bmiHeader.biSizeImage; 
        bfh.bfReserved1 = bfh.bfReserved2 = 0; 
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizBMI; 
        
        if (_lwrite(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER)) == -1)
        {
                bSuccess = FALSE; 
                goto ErrExit3; 
	} 
	
        if ((hTmpBmp = CreateCompatibleBitmap(hDC, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight)) != 0)
        {
                hBmpOld = (HBITMAP)SelectObject(hDC, hTmpBmp); 
                if ((GetDIBits(hDC, hBmp, 0, pbmi->bmiHeader.biHeight, (LPSTR)pBits, pbmi, DIB_RGB_COLORS))==0)
                {
                        bSuccess = FALSE; 
                        goto ErrExit4; 
                } 
	}
        else
        {
                bSuccess = FALSE; 
                goto ErrExit3; 
	} 
	
	if (_lwrite(hFile, (LPSTR)pbmi, sizBMI) == -1)
        {
                bSuccess = FALSE; 
                goto ErrExit4; 
	} 
	
	if (_lwrite(hFile, (LPSTR)pBits, pbmi->bmiHeader.biSizeImage) == -1)
        {
                bSuccess = FALSE; 
                goto ErrExit4; 
	} 
	
	
ErrExit4: 
	SelectObject(hDC, hBmpOld); 
	DeleteObject(hTmpBmp); 

ErrExit3:
	_lclose(hFile); 

ErrExit2:
	GlobalFree(pbmi); 

ErrExit1:
	GlobalFree(pBits); 

	return bSuccess;
} 

void dumpscreen(char * fname)
{
        HBITMAP     hBitmap;
        RECT rScreen;       
        HDC hDC;            
        MSG msg;            
        CHAR lpBuffer[128]; 

	hDC = CreateDC("DISPLAY", NULL, NULL, NULL); 
	rScreen.left = rScreen.top = 0; 
	rScreen.right = GetDeviceCaps(hDC, HORZRES); 
	rScreen.bottom = GetDeviceCaps(hDC, VERTRES); 

	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE) != 0) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 


        hBitmap = capturescreen(&rScreen); 

        SaveBitmapFile(hDC,hBitmap,fname);
        DeleteObject(hBitmap);
        DeleteDC(hDC);
}
