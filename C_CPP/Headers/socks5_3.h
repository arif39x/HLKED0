SOCKET socks5_handle( SOCKET Client, char* Buffer );
#define auth_usr "usernameXdfk3" // socks5 user
#define auth_pwd "passwordX34dD" // socks5 password
#define Alloc(Typecast, Size) (Typecast)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (DWORD)Size) 
#define Free(Mem) { HeapFree(GetProcessHeap(), 0, Mem); Mem = NULL; }