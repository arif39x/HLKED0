#include <string>
#include <sstream>
#include <windows.h>
#include <winsock.h>
#pragma comment(lib,"Urlmon.lib")
#pragma comment(lib,"Ws2_32.lib")
using namespace std;
#if defined DEBUG
  #include <iostream>
#else
  #pragma comment(linker, "/subsystem:windows ")
  #pragma comment(linker, "/entry:main")
#endif
