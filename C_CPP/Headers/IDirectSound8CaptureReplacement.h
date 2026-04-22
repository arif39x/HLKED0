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
#include <DSound.h>


 
#ifndef _IDIRECTSOUND8CAPTUREREPLACEMENT_H
#define _IDIRECTSOUND8CAPTUREREPLACEMENT_H

class IDirectSound8CaptureReplacement : public IDirectSoundCapture8
{
  private:
    IDirectSoundCapture8 *mIDirectSound8CaptureOriginal;
//    DWORD mInputDataBufferSize;
  public:
    IDirectSound8CaptureReplacement(IDirectSoundCapture8 *pIDirectSound8CaptureOriginal);
    IDirectSound8CaptureReplacement();
    ~IDirectSound8CaptureReplacement();
    ULONG __stdcall AddRef();
    ULONG __stdcall Release();
    HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    HRESULT __stdcall CreateCaptureBuffer(LPCDSCBUFFERDESC pcDSCBufferDesc, LPDIRECTSOUNDCAPTUREBUFFER *ppDSCBuffer, LPUNKNOWN pUnkOuter);
    HRESULT __stdcall GetCaps(LPDSCCAPS pDSCCaps);
    HRESULT __stdcall Initialize(LPCGUID pcGuidDevice);
};

#endif