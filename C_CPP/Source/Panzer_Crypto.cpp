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
#include "Panzer_Definitions.h"


unsigned char sBox[SBOX_SIZE + 1];




int RC4Encrypt(unsigned char *pKey, unsigned char *pPlainText, int pPlainTextLength, unsigned char *pCipherText, int pMaxCipherTextLength, int *pOutputLength)
{
  int lCounter = 0;
  int i = 0;
  int j = 0;
  int t = 0;
  char *lPlainText = NULL;
  unsigned char lKey[MAX_BUF_SIZE + 1];
  int lRetVal = 0;


  if ((lPlainText = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pPlainTextLength + 1)) == NULL)
  {
    lRetVal = 1;
	goto END;
  }


  ZeroMemory(lKey, sizeof(lKey));
  CopyMemory((char *) lKey, (char *) pKey, sizeof(lKey) - 1);
  CopyMemory((char *) lPlainText, (char *) pPlainText, pPlainTextLength);

  RC4prepareKey(lKey);
  *pOutputLength = 0;

  for (lCounter = 0; lCounter < pPlainTextLength && lCounter < pMaxCipherTextLength; lCounter++)
  {
    i = (i + 1) % 256;
    j = (j + sBox[i]) % 256;
    t = sBox[i];
    sBox[i] = sBox[j];
    sBox[j] = t;

    t = (sBox[i] + sBox[j]) % 256;
    pCipherText[lCounter] = lPlainText[lCounter] ^ sBox[t];
  } // for (lCounter = 0; lC...

  *pOutputLength = lCounter;



END:

  if (lPlainText != NULL)
    HeapFree(GetProcessHeap(), 0, lPlainText);

  return(lRetVal);
}




/*
 *
 *
 */

int RC4prepareKey(unsigned char *pKey)
{
  int mLen = 0;
  int i = 0;
  int j = 0;
  int t = 0;

  mLen = (int) strlen((const char *) pKey);

  for (i = 0; i < 256; i++)
    sBox[i] = i;

  for (i = 0; i < 256; i++)
  {
    j = (j + sBox[i] + pKey[i % mLen]) % 256;
    t = sBox[i];
    sBox[i] = sBox[j];
    sBox[j] = t;
  }
  return(0);
}
