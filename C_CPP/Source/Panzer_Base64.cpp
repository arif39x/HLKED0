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



#include <string.h>
#include "Panzer_Definitions.h"


static const char gB64Basis[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";




/*
 *
 *
 */

int Base64Encode(char *encoded, const char *pUnencoded, int pUencBufferLength)
{
  int lCounter;
  char *lTempPointer = encoded;


  for (lCounter = 0; lCounter < pUencBufferLength - 2; lCounter += 3) 
  {
    *lTempPointer++ = gB64Basis[(pUnencoded[lCounter] >> 2) & 0x3F];
    *lTempPointer++ = gB64Basis[((pUnencoded[lCounter] & 0x3) << 4) | ((int) (pUnencoded[lCounter + 1] & 0xF0) >> 4)];
    *lTempPointer++ = gB64Basis[((pUnencoded[lCounter + 1] & 0xF) << 2) | ((int) (pUnencoded[lCounter + 2] & 0xC0) >> 6)];
    *lTempPointer++ = gB64Basis[pUnencoded[lCounter + 2] & 0x3F];
  } // for (lCounter = 0; lCou...


  if (lCounter < pUencBufferLength) 
  {
    *lTempPointer++ = gB64Basis[(pUnencoded[lCounter] >> 2) & 0x3F];

    if (lCounter == (pUencBufferLength - 1)) 
    {
      *lTempPointer++ = gB64Basis[((pUnencoded[lCounter] & 0x3) << 4)];
      *lTempPointer++ = '=';
    } else {
      *lTempPointer++ = gB64Basis[((pUnencoded[lCounter] & 0x3) << 4) | ((int) (pUnencoded[lCounter + 1] & 0xF0) >> 4)];
      *lTempPointer++ = gB64Basis[((pUnencoded[lCounter + 1] & 0xF) << 2)];
    } // if (lCounter ==...

    *lTempPointer++ = '=';
  } // if (lCounter < pUen...


  *lTempPointer++ = '\0';
  return((int) (lTempPointer - encoded));
}


