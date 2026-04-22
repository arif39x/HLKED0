/*
Copyright (C) 2000-2001 Marius David.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// arena_registry.cpp

#include "stdafx.h"
//---------------------------------------------------------------------------
#include "arena_registry.h"
//---------------------------------------------------------------------------
HKEY AppRegKey;

bool SetRegValueInt(char* RegKey, DWORD Value, bool Flush)
{
  if (RegSetValueEx(AppRegKey, RegKey, 0, REG_DWORD, (CONST BYTE*)&Value,
      sizeof(Value))!=ERROR_SUCCESS)
    return false;
  if (Flush)
    RegFlushKey(AppRegKey);
  return true;
}
//---------------------------------------------------------------------------
bool SetRegValueString(char* RegKey, char* Value, bool Flush)
{
  if (RegSetValueEx(AppRegKey, RegKey, 0, REG_SZ, (CONST BYTE*)Value,
    strlen(Value) + 1)!=ERROR_SUCCESS)
    return false;
  if (Flush)
    RegFlushKey(AppRegKey);
  return true;
}
//---------------------------------------------------------------------------
bool GetRegValue(char* RegKey, LPBYTE Buffer)
{
  DWORD BufferSize, ValueType;
  RegQueryValueEx(AppRegKey, RegKey, 0, &ValueType, NULL, &BufferSize);
  if (RegQueryValueEx(AppRegKey, RegKey, 0, &ValueType,
      Buffer, &BufferSize)==ERROR_SUCCESS)
    return true;
  return false;
}
//---------------------------------------------------------------------------
bool ExistsRegKey(char* RegKey)
{
  if (RegQueryValueEx(AppRegKey, RegKey, 0, NULL, NULL, NULL)==ERROR_SUCCESS)
    return true;
  return false;
}
//---------------------------------------------------------------------------
bool OpenRegSubKey(char * SubKey, HKEY hkey)
{
  if (RegOpenKeyEx(hkey, SubKey,
      0, KEY_WRITE | KEY_READ, &AppRegKey)==ERROR_SUCCESS)
    return true;
  return false;
}
//---------------------------------------------------------------------------
bool CloseRegSubKey(void)
{
  if (RegCloseKey(AppRegKey)==ERROR_SUCCESS)
    return true;
  return false;
}
//---------------------------------------------------------------------------
bool CreateRegSubKey(char * SubKey, HKEY hkey)
{
  SECURITY_ATTRIBUTES sa;
  DWORD dwDisposition;
  sa.nLength=sizeof sa;
  sa.lpSecurityDescriptor=NULL;
  sa.bInheritHandle=true;

  if (RegCreateKeyEx(hkey, SubKey,
      0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ,
      &sa, &AppRegKey, &dwDisposition)==ERROR_SUCCESS)
    return true;
  return false;
}
//---------------------------------------------------------------------------
bool EnumKey(int Index, char *Name)
{
  DWORD NameLen=250;
  FILETIME temp;
  if (RegEnumKeyEx(AppRegKey, Index, Name, &NameLen, NULL, NULL, NULL, &temp)==
      ERROR_SUCCESS)
    return true;
  return false;
}
//---------------------------------------------------------------------------
HKEY GetHKEY(void)
{
  return AppRegKey;
}
//---------------------------------------------------------------------------
void SetHKEY(HKEY hKey)
{
  AppRegKey=hKey;
}
//---------------------------------------------------------------------------

