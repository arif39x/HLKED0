//---------------------------------------------------------------------------
#ifndef arena_registryH
#define arena_registryH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
bool SetRegValueInt(char* RegKey, DWORD Value, bool Flush=true);
bool SetRegValueString(char* RegKey, char* Value, bool Flush=true);
bool GetRegValue(char* RegKey, LPBYTE Buffer);
bool ExistsRegKey(char* RegKey);
bool OpenRegSubKey(char * SubKey, HKEY hkey=HKEY_LOCAL_MACHINE);
bool CloseRegSubKey(void);
bool CreateRegSubKey(char * SubKey, HKEY hkey=HKEY_LOCAL_MACHINE);
bool EnumKey(int Index, char *Name);
HKEY GetHKEY(void);
void SetHKEY(HKEY hKey);

#endif
