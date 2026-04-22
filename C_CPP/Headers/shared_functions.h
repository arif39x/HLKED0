void ResolveHost(char* serverip,string server){
  hostent *dns = gethostbyname((char*)server.c_str());
  sprintf(serverip, "%u.%u.%u.%u",
			(unsigned char) dns->h_addr_list[0][0],
			(unsigned char) dns->h_addr_list[0][1],
			(unsigned char) dns->h_addr_list[0][2],
			(unsigned char) dns->h_addr_list[0][3]);
}
int Str2Int(string s)
{
     return atoi(s.c_str());
}     
string IntToStr(int i)	// convert int to string
{
	stringstream s;
	s << i;
	return s.str();
}
int OSVersionCheck(void)
{ 
	DWORD dwRet = 9999;

	OSVERSIONINFO verinfo;
	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&verinfo)) {
	  if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 10) dwRet = 1;
	  else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 90) dwRet = 2;
	  else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 0) dwRet = 3;
	  else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 1) dwRet = 4;//xp
	  else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 2) dwRet = 5;
	  else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 2) dwRet = 2003;//server 03
      else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 1) dwRet=7;
      else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 0) dwRet=6;//vista 
	}             

	return (dwRet);
}

string NAT(){
	char hostname[255];
 char *IPAddress;
 PHOSTENT hostinfo;

   if( gethostname ( hostname, sizeof(hostname)) == 0)
  {
   if((hostinfo = gethostbyname(hostname)) != NULL)
   {
    IPAddress = (char*)inet_ntoa ((*(struct in_addr *)*hostinfo->h_addr_list));
   }
  }
string ip=IPAddress;
ip=ip.substr(0,7);
if (ip=="192.168")
  return "Y";
else return "N";
}

string name(){
   char szTemp[512]="";
   DWORD dwSize=512;
   GetComputerName(szTemp, &dwSize);
   string name=szTemp;
        return name;
}
string serial(){
	char szTemp[512]="";
	DWORD dwSize=512, dwType;
	HKEY hKey;

	szTemp[0]=0;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, 

&hKey)==ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, "ProductId", NULL, &dwType, (LPBYTE)szTemp, &dwSize);

		RegCloseKey(hKey);
	}
	else
	{
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\WindowsNT\\CurrentVersion", 0, 

KEY_READ, &hKey)==ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, "ProductId", NULL, &dwType, (LPBYTE)szTemp, &dwSize);

			RegCloseKey(hKey);
		}
	}
	string serial=szTemp;
	return serial;
}
