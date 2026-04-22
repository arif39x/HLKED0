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
#include <WtsApi32.h>
#include <Ntsecapi.h>
#include <shellapi.h>
#include <lm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Panzer_Definitions.h"



/*
 * Determine general system information. 
 *
 */

char *generalSystemInformation(char *pDataBufferAddress, int *pDataBufferSize)
{
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;
  char lTemp[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char *lTempPtr = NULL;
  SYSTEMTIME lSystemTime;




  /*
   * Determine computer name
   *
   */

  ZeroMemory(lTemp, sizeof(lTemp));
  _snprintf(lTemp, sizeof(lTemp) - 1, "%-25s :\t%s\n", "Computer name", getenv("COMPUTERNAME"));
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));


  /*
   * Determine computer name
   *
   */

  ZeroMemory(lTemp, sizeof(lTemp));
  _snprintf(lTemp, sizeof(lTemp) - 1, "%-25s :\t%s\n", "Logged in as user", getenv("USERNAME"));
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));



  /*
   * Determine computer name
   *
   */

  ZeroMemory(lTemp, sizeof(lTemp));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  getWindowsVersion(lTemp2, sizeof(lTemp2));
  _snprintf(lTemp, sizeof(lTemp) - 1, "%-25s :\t%s\n", "OS version", lTemp2);
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));



  /*
   * Determine computer name
   *
   */

  ZeroMemory(lTemp, sizeof(lTemp));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  getProcessorInformation(lTemp2, sizeof(lTemp2));
  _snprintf(lTemp, sizeof(lTemp) - 1, "%-25s :\t%s\n", "Processor information", lTemp2);
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));



  /*
   * Determine computer name
   *
   */

  ZeroMemory(lTemp, sizeof(lTemp));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  getMemoryInformation(lTemp2, sizeof(lTemp2));
  _snprintf(lTemp, sizeof(lTemp) - 1, "%-25s :\t%s\n", "Memory size", lTemp2);
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));


  /*
   * Determine time zone.
   *
   */
  ZeroMemory(lTemp, sizeof(lTemp));
  GetLocalTime(&lSystemTime);
  _snprintf(lTemp, sizeof(lTemp) - 1, "%-25s :\t%d.%02d.%02d %02d:%02d:%02d\n", "Local time", 
            lSystemTime.wYear, lSystemTime.wMonth, lSystemTime.wDay, lSystemTime.wHour, 
            lSystemTime.wMinute, lSystemTime.wSecond);
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));


  /*
   * Determine local user accounts
   *
   */

  lRetVal = enumerateLocalUsers(lRetVal, pDataBufferSize);


  return(lRetVal);
}




/*
 * Determine the exact Windows version.
 *
 */

int getWindowsVersion(char *pOSVersion, int pBufferLength)
{
  OSVERSIONINFO lOSVersionInfo;
  unsigned int lOSMajorNumber, lOSMinorNumber;
  eOSTypes os;
  int lRetVal = 0;

  ZeroMemory(pOSVersion, pBufferLength);
  lOSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);


  /*
   * Determine the exact OS version.
   *
   */

  if(GetVersionEx(&lOSVersionInfo))
  {
    lOSMajorNumber = lOSVersionInfo.dwMajorVersion;
    lOSMinorNumber = lOSVersionInfo.dwMinorVersion;

    if(lOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32s)
	{
      os = Win32s;
    } else if(lOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      if(lOSVersionInfo.dwMajorVersion == 4)
      {
        if(lOSVersionInfo.dwMinorVersion == 0)
          os = Windows95;
        else if(lOSVersionInfo.dwMinorVersion == 10)
          os = Windows98;
        else if(lOSVersionInfo.dwMinorVersion == 90)
          os = WindowsME;
      }
    } else if(lOSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      if(lOSVersionInfo.dwMajorVersion == 4)
        os = WindowsNT;

      if(lOSVersionInfo.dwMajorVersion == 5)
      {
        if(lOSVersionInfo.dwMinorVersion == 0)
          os = Windows2000;
        else if(lOSVersionInfo.dwMinorVersion == 1)
          os = WindowsXP;
        else if(lOSVersionInfo.dwMinorVersion == 2)
          os = Windows2003;
      }

      if(lOSVersionInfo.dwMajorVersion == 6)
        os = WindowsVista;
    } // 
  } else {/* assume Windows NT if GetVersionEx call fails */
    os = WindowsNT;
    lOSMajorNumber = 4;
    lOSMinorNumber = 0;
  }

  /*
   * Write output buffer
   *
   */
  
  if (os == Win32s) 
    _snprintf(pOSVersion, pBufferLength - 1, "Win32s");
  else if (os == Windows95)
    _snprintf(pOSVersion, pBufferLength - 1, "Window 95");
  else if (os == Windows98)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows 98");
  else if (os == WindowsME)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows ME");
  else if (os == WindowsNT)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows NT");
  else if (os == Windows2000)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows 2000");
  else if (os == WindowsXP)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows XP");
  else if (os == Windows2003)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows Server 2003");
  else if (os == WindowsVista)
    _snprintf(pOSVersion, pBufferLength - 1, "Windows Vista");
  else
    _snprintf(pOSVersion, pBufferLength - 1, "OS unknown");

  return(lRetVal);
}





/*
 * Get processor information.
 *
 */

int getProcessorInformation(char *pProcessorInformation, int pBufferSize)
{
  LONG lFunctionResult;
  HKEY hKey;
  char lDataBuffer[MAX_BUF_SIZE + 1];
  DWORD lDataSize = 0;
  int lRetVal = 0;


  ZeroMemory(pProcessorInformation, pBufferSize);

  if ((lFunctionResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey)) == ERROR_SUCCESS) 
  {
    lDataSize = sizeof(lDataBuffer) - 1;
    RegQueryValueEx (hKey, "ProcessorNameString", NULL, NULL, (LPBYTE) lDataBuffer, &lDataSize);
    _snprintf(pProcessorInformation, pBufferSize - 1, "%s", lDataBuffer);
  } // if ((lFunctionResult = RegOpenKeyEx(HKE...

  return(lRetVal);
}


/*
 * Get memory information
 *
 */

int getMemoryInformation(char *pMemoryInformation, int pBufferSize)
{
  MEMORYSTATUS lMemoryStat;
  int lRetVal = 0;


  ZeroMemory(pMemoryInformation, pBufferSize);
  GlobalMemoryStatus(&lMemoryStat);

  if (lMemoryStat.dwTotalPhys < 1000)
    _snprintf(pMemoryInformation, pBufferSize - 1, "%d bytes", lMemoryStat.dwTotalPhys);
  else if (lMemoryStat.dwTotalPhys < 1000000)
    _snprintf(pMemoryInformation, pBufferSize - 1, "%0.3f kb", (float) lMemoryStat.dwTotalPhys/1000);
  else if (lMemoryStat.dwTotalPhys < 1000000000) 
    _snprintf(pMemoryInformation, pBufferSize - 1, "%0.3f mb", (float) lMemoryStat.dwTotalPhys/1000000);
  else if (lMemoryStat.dwTotalPhys < 1000000000000)
    _snprintf(pMemoryInformation, pBufferSize - 1, "%0.3f gb", (float) lMemoryStat.dwTotalPhys/1000000000);
  else if (lMemoryStat.dwTotalPhys < 1000000000000000) 
    _snprintf(pMemoryInformation, pBufferSize - 1, "%0.3f tb", (float) lMemoryStat.dwTotalPhys/1000000000000);
  else
    _snprintf(pMemoryInformation, pBufferSize - 1, "Memory size unknown");

  return(lRetVal);
}



/*
 * Enumerate all local users.
 *
 */

char *enumerateLocalUsers(char *pDataBufferAddress, int *pBufferSize)
{
  char *lRetVal = (pDataBufferAddress != NULL)?pDataBufferAddress:NULL;
  LPUSER_INFO_10 lBuf = NULL;
  LPUSER_INFO_10 lTempBuffer;
  DWORD lLevel = 10;
  DWORD lPrefMaxLen = MAX_PREFERRED_LENGTH;
  DWORD lEntriesRead = 0;
  DWORD lTotalEntries = 0;
  DWORD lResumeHandle = 0;
  DWORD lCounter;
  NET_API_STATUS lStatus;
  LPCWSTR pszServerName = NULL;
  char lUserName[MAX_BUF_SIZE + 1];
  char lFullUserName[MAX_BUF_SIZE + 1];
  char lTemp[MAX_BUF_SIZE + 1];

  do 
  {
    lStatus = NetUserEnum(pszServerName, lLevel, FILTER_NORMAL_ACCOUNT, // global users
                         (LPBYTE*) &lBuf, lPrefMaxLen, &lEntriesRead, &lTotalEntries, &lResumeHandle);

    if ((lStatus == NERR_Success) || (lStatus == ERROR_MORE_DATA))
    {
      if ((lTempBuffer = lBuf) != NULL)
      {
        for (lCounter = 0; lCounter < lEntriesRead; lCounter++)
        {
          if (lTempBuffer == NULL)
            break;

          ZeroMemory(lFullUserName, sizeof(lFullUserName));
          ZeroMemory(lUserName, sizeof(lUserName));
		  wcstombs(lFullUserName, lTempBuffer->usri10_full_name, sizeof(lFullUserName));
		  wcstombs(lUserName, lTempBuffer->usri10_name, sizeof(lUserName));

		  _snprintf(lTemp, sizeof(lTemp) - 1, "%-20s -\t%s\n", lUserName, lFullUserName);
          lRetVal = addDataToBuffer(lRetVal, pBufferSize, lTemp, strlen(lTemp));

          lTempBuffer++;
        } // for (lCounter = 0; lCo...
      } // if ((lTempBuffer = pBu...
    } // if ((lStatus == NE...

    if (lBuf != NULL)
    {
      NetApiBufferFree(lBuf);
      lBuf = NULL;
    }
  } while (lStatus == ERROR_MORE_DATA); // end do

  if (lBuf != NULL)
    NetApiBufferFree(lBuf);

  return(lRetVal);
}