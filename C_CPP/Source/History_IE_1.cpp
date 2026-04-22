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


#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Panzer_Definitions.h"



/*
 * Find Index.dat files by searching recursively
 * a base directory. Tested on Windows XP with IE 8.
 *
 */

char *getIEHistory(char *pBaseDirectory, char *pDataBufferAddress, int *pDataBufferSize)
{
  WIN32_FIND_DATA lFileData; 
  HANDLE lSearchHandle;  
  char lFilePattern[MAX_BUF_SIZE + 1]; 
  char lSubDirectory[MAX_BUF_SIZE + 1];
  char lHistoryFile[MAX_BUF_SIZE + 1];
  char lHomeDirectory[MAX_BUF_SIZE + 1];
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;


  if (pBaseDirectory != NULL)
  {
    /*
     * Prepare file pattern. We want to check all the files
     * to detect directories and search inside them, too.
     *
     */

    ZeroMemory(lHomeDirectory, sizeof(lHomeDirectory));
    strncpy(lHomeDirectory, pBaseDirectory, sizeof(lHomeDirectory));

    if (lHomeDirectory[strlen(lHomeDirectory) - 1] != '\\')
      strcat(lHomeDirectory, "\\");

    ZeroMemory(lFilePattern, sizeof(lFilePattern));
    _snprintf(lFilePattern, sizeof(lFilePattern) - 1, "%s*", lHomeDirectory);


    /*
     * Start the search.
     *
     */

    if ((lSearchHandle = FindFirstFile(lFilePattern, &lFileData)) != INVALID_HANDLE_VALUE) 
    {
      do 
      { 
        /*
         * File is a directory. Do a recursive function call to 
         * find index.dat files in that directory.
         *
         */

        if(lFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if (strcmp(lFileData.cFileName, ".") && strcmp(lFileData.cFileName, ".."))
          {
            ZeroMemory(lSubDirectory, sizeof(lSubDirectory));
            _snprintf(lSubDirectory, sizeof(lSubDirectory) - 1, "%s\\%s", pBaseDirectory, lFileData.cFileName);
            lRetVal = getIEHistory(lSubDirectory, lRetVal, pDataBufferSize);
          } // if (strcmp(lFileDa...
        } else {

          /*
           * File is a Internet Explorer history file.
           *
           */

          if (strstr(lFileData.cFileName, "ndex.dat") && (strstr(pBaseDirectory, "nternet") || strstr(pBaseDirectory, "istory.IE5")))
          {
            ZeroMemory(lHistoryFile, sizeof(lHistoryFile));
            _snprintf(lHistoryFile, sizeof(lHistoryFile) - 1, "%s\\%s", pBaseDirectory, lFileData.cFileName);
            lRetVal = dumpHistoryEntries(lHistoryFile, lRetVal, pDataBufferSize);
          } // if (strstr(lFileData...
        } // if(lFileData.dwFileAt...
      } while (FindNextFile(lSearchHandle, &lFileData));
    } // if ((lSearchHandle = FindFirstFile(l...
  } // if (pBaseDirectory != NULL)

  return(lRetVal);
}




/*
 * Dump URLs from a cache and history file. 
 *
 */

char *dumpHistoryEntries(char *pFileName, char *pDataBufferAddress, int *pDataBufferSize)
{
  char *lBufPointer = NULL;
  char *lTempPointer = NULL;
  long lFileSize;
  int lCounter = 0;
  struct sHistory lHistory;
  int lType = 0;
  char lTemp[MAX_BUF_SIZE + 1];
  char lTimeStamp[MAX_BUF_SIZE + 1];
  HANDLE lFileHandle = INVALID_HANDLE_VALUE;
  DWORD lBytesRead = 0;
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;


  if ((lFileHandle = CreateFile(pFileName,  GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
  {
    if ((lFileSize = GetFileSize(lFileHandle, NULL)) > 0 && (lTempPointer = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lFileSize)) != NULL)
    {
      if (ReadFile(lFileHandle, lTempPointer, lFileSize,  &lBytesRead, NULL))
      {

        /*
         * Iterate byte after byte through the .dat file.
         *
         */

        for (lCounter = 0; lCounter < lFileSize; lCounter++) 
        {
          lBufPointer = lTempPointer + lCounter;

          /*
           * Determine URL type
           *
           */

          if (strncmp(lBufPointer, "URL ", 4) == 0)
            lType = TYPE_URL;
          else if (strncmp(lBufPointer, "REF ", 4) == 0)
            lType = TYPE_REDR;
          else 
            lType = TYPE_UNDEF;



          /*
           * If a valid data type was found, print it.
           *
           */

          if (lType != TYPE_UNDEF)
          {
            ZeroMemory(&lHistory, sizeof(lHistory));
            if (extractURL(lBufPointer, lType, &lHistory) > 0 && lHistory.lURL != NULL)  
	        {
              ZeroMemory(lTimeStamp, sizeof(lTimeStamp));
              ZeroMemory(lTemp, sizeof(lTemp));

              /*
               * Regular entry in cache file format.
               *
               */

              if (strstr(lHistory.lURL, "@http@"))
              {
                snprintf(lTemp, sizeof(lTemp) - 1, "Cache\t%d/%02d/%02d %02d:%02d\t%s\n", lHistory.st.wYear, lHistory.st.wMonth, lHistory.st.wDay, lHistory.st.wHour, lHistory.st.wMinute, lHistory.lURL);
                lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));

                /*
                 * Regular Internet Explorer entry in history file format.
                 *
                 */
              } else if (strstr(lHistory.lURL, "@http://")) {
                snprintf(lTemp, sizeof(lTemp) - 1, "Inet\t%d/%02d/%02d %02d:%02d\t%s\n", lHistory.st.wYear, lHistory.st.wMonth, lHistory.st.wDay, lHistory.st.wHour, lHistory.st.wMinute, lHistory.lURL);
                lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));

                /*
                 * Regular local file explorer entry in history file format.
                 *
                 */
              } else if (strstr(lHistory.lURL, "@file:///")) {
                snprintf(lTemp, sizeof(lTemp) - 1, "Local\t%d/%02d/%02d %02d:%02d\t%s\n", lHistory.st.wYear, lHistory.st.wMonth, lHistory.st.wDay, lHistory.st.wHour, lHistory.st.wMinute, lHistory.lURL);
                lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
              } // if (strstr(lHistory...
            } // if (extractURL(lBufPoint...

            if (lHistory.lURL != INVALID_HANDLE_VALUE)
              HeapFree(GetProcessHeap(), 0, lHistory.lURL);

          } // if (lType != TYPE_UNDEF) 
        } // for (lCounter = 0; lCoun...
      } // if (ReadFile(lFileHa...

      HeapFree(GetProcessHeap(), 0, lTempPointer);
    } // if ((lTempPointer = (char *) HeapA...

    CloseHandle(lFileHandle);
  } // if ((lFileHandle = CreateFile(pFi...

  return(lRetVal);
}





/*
 * Extract first entry from a buffer and
 * copy it to the history data structure.
 *
 */

int extractURL(char *pBuf, int pType, struct sHistory *pHistory) 
{
  int lCounter = 0;
  int lRetVal = 1;
  FILETIME lFileTime;

  ZeroMemory(pHistory, sizeof(struct sHistory));


  if (pType == TYPE_URL) 
  {
    CopyMemory((DWORD *) &lFileTime.dwLowDateTime, pBuf + URL_TIME_OFFSET, sizeof(DWORD));
    CopyMemory((DWORD *) &lFileTime.dwHighDateTime, pBuf + URL_TIME_OFFSET + 4, sizeof(DWORD));
    FileTimeToSystemTime(&lFileTime, &pHistory->st);
    pHistory->lType = TYPE_URL;

    pBuf += URL_URL_OFFSET;
  } else if (pType == TYPE_REDR) {
    lFileTime.dwHighDateTime = 0;
    lFileTime.dwLowDateTime = 0;
    FileTimeToSystemTime( &lFileTime, &pHistory->st );
    pHistory->lType = TYPE_REDR;

    pBuf += REDR_URL_OFFSET;
  } else {
    lRetVal = TYPE_UNDEF;
    goto END;
  } // if (pType == TYPE_URL ...


  while (pBuf[lCounter] != 0 && lCounter < 1026)
    lCounter++;


  if (lCounter > 1024)
  {
    goto END;
  } else if ((pHistory->lURL = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024)) != NULL) {
    ZeroMemory(pHistory->lURL, 1024);
    strncpy(pHistory->lURL, pBuf, lCounter);
  } // if ((pHistory->lURL = (cha...

END:

  return(lRetVal);	
}
