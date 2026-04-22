void DoSearch( unsigned long uStartAddr, unsigned long uEndAddr, PROCESSENTRY32 pe32);
void RemoveBot( PROCESSENTRY32 pe32);
DWORD WINAPI botkiller_main(LPVOID lParam);

typedef struct s_Search
{
   char* szBot;
   char* szString;
}s_Search;