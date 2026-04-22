/*
 * Tool name   : DLLInjector
 * Description : This is a proof of concept tool to show how to inject a DLL
 *               into a remote process and proxying functions to intercept
 *               data.
 * Datum       : 27. September 2010
 * Author      : Ruben Unteregger
 * Webpage     : http://www.megapanzer.com
 * Version     : 0.3
 * OS          : Tested on Microsoft Windows 7
 * Todo        : -
 *
 * Changes     : - 
 * 
 *
 *
 * License     :
 *
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
 *
 */


#include <stdio.h>
#include <windows.h>
#include <Shlwapi.h>

#define snprintf _snprintf
#define MAX_BUF_SIZE 1024
#define MIOSTAR_DIRECTORY "MioStar\\"
#define MIOSTAR_RULES "Rules.txt"

#pragma comment(lib, "shlwapi.lib")



char *gEnvVariables[] = {"USERPROFILE", "TMP", "TEMP", "HOMEDRIVE", "windir", NULL};


/*
 * Function forward declarations
 *
 */

void startIt(char *pDLLPath);
char *findHomeDirectory (char *pDirectory, unsigned int pDirectorySize);






/*
 * Program entry point
 *
 */

int main(int argc, char **argv)
{
  int lRetVal = 0;
  STARTUPINFO lSI;
  PROCESS_INFORMATION  lPI;
  char lCmdLine[MAX_BUF_SIZE + 1];
  char lRulesFile[MAX_BUF_SIZE + 1];
  char lHomeDirectory[MAX_BUF_SIZE + 1];

  /*
   *
   *
   */

  ZeroMemory(lRulesFile, sizeof(lRulesFile));
  ZeroMemory(lHomeDirectory, sizeof(lHomeDirectory));

  findHomeDirectory(lHomeDirectory, sizeof(lHomeDirectory) - 1);
  snprintf(lRulesFile, sizeof(lRulesFile) - 1, "%s%s", lHomeDirectory, MIOSTAR_RULES);

  if (! PathFileExists(lRulesFile))
  {
    printf("No rule file (%s) file found!\n", MIOSTAR_RULES);
	printf("Create the file \"%s\" and start populating it with your rules.\n", lRulesFile);
    return(1);
  }



  /*
   *
   *
   */

  if (argc == 2) 
  {
    ZeroMemory(&lSI, sizeof(lSI));
    ZeroMemory(&lPI, sizeof(lPI));
    lSI.cb = sizeof(lSI);
    lSI.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    lSI.wShowWindow = SW_HIDE;



    /*
     * create the VR loader process
     *
     */

    ZeroMemory(lCmdLine, sizeof(lCmdLine));
    snprintf(lCmdLine, sizeof(lCmdLine) - 1, "\"%s\" \"%s\" 1", argv[0], argv[1]);
    CreateProcess(NULL, lCmdLine, NULL, NULL, FALSE, CREATE_NO_WINDOW|CREATE_NEW_CONSOLE, 0, NULL, &lSI, &lPI);

  } else if (argc == 3) {
     printf("Exists : %s\n", lRulesFile);
	startIt(argv[1]);
  } else
    printf("Usage : %s dll-path\n", argv[0]);



  return(lRetVal);
}




/*
 *
 *
 */

void startIt(char *pDLLPath)
{
	HINSTANCE lDLL = 0;
	

    if (PathFileExists(pDLLPath))
    {
      printf("Exists : %s\n", pDLLPath);

      if((lDLL = LoadLibrary(pDLLPath)) != NULL)
      {
        /*
         * Endless loop to permanently keep hooks in the remote process.
	     *
         */

		printf("Loaded : %s\n", pDLLPath);

        while(1)
	    {
          printf(".");
          Sleep(200);
	    }
	  } // if ((lDLL = LoadLibrary(...
	} else {
      printf("%s does not exist\n", pDLLPath);
	} // if (PathFileExi...
}





/*
 * Find MioStar home directory 
 *
 */

char *findHomeDirectory (char *pDirectory, unsigned int pDirectorySize)
{
  int mCounter = 0;
  char lTemp[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];

  ZeroMemory(pDirectory, pDirectorySize);

  for(mCounter = 0; gEnvVariables[mCounter] != NULL; mCounter++)
  {
    ZeroMemory(lTemp, sizeof(lTemp));
    if (GetEnvironmentVariable(gEnvVariables[mCounter], lTemp, sizeof(lTemp) - 1) > 0)
    {
      if (lTemp[strlen(lTemp) - 1] != '\\')
        strcat(lTemp, "\\");          

      if (strlen(lTemp) + strlen(MIOSTAR_DIRECTORY) < pDirectorySize)
      {
        strcat(lTemp, MIOSTAR_DIRECTORY);

        if (lTemp[strlen(lTemp) - 1] != '\\')
          strcat(lTemp, "\\");

        if (PathFileExists(lTemp))
        {
          strncpy(pDirectory, lTemp, pDirectorySize);
          goto END;
        }
      }
    }    
  } 


  /*
   * No home directory found. So we create a new one.
   *
   */

  for(mCounter = 0; gEnvVariables[mCounter] != NULL; mCounter++)
  {
    ZeroMemory(lTemp, sizeof(lTemp));
    if (GetEnvironmentVariable(gEnvVariables[mCounter], lTemp, sizeof(lTemp) - 1) > 0)
    {
      if (lTemp[strlen(lTemp) - 1] != '\\')
        strcat(lTemp, "\\");      

      if (PathFileExists(lTemp))
      {
        ZeroMemory(lTemp2, sizeof(lTemp2));
        snprintf(lTemp2, sizeof(lTemp2) - 1, "%s%s", lTemp, MIOSTAR_DIRECTORY);
        CreateDirectory(lTemp2, NULL);

        if (PathFileExists(lTemp2))
        {
          strncpy(pDirectory, lTemp2, pDirectorySize);
          goto END;
        } // if (PathFileExists(lTemp))
      } // if (PathFileExists(lTemp))
    } // if (GetEnvironmentVariable(gEnvVariables[mCounter], lTemp, sizeof(lTemp) - 1) > 0)
  } // for(mCounter = 0; gEnvVariables[mCounter] != NULL; mCounter++)

END:

  return(pDirectory);
}

