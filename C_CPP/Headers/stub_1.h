#define xor(a, b) { char *c = (a); while(*c) *c++ ^= (b); } //macro hackro
char* GetResource(HINSTANCE ModuleHandle,int ResourceName
						,int ResourceTyp);
void LoadSettings(){
  HINSTANCE hInstance;
  char* Settings;

  hInstance=GetModuleHandle(0);
  Settings=GetResource(hInstance,666,1337);
  xor(Settings,'0');
  Settings[strlen(Settings)-1]=0;

  string Temp=Settings;
#ifdef DEBUG
  cout<<Temp<<endl;
  #endif
  server=Temp.substr(0,Temp.find("/!/"));
  Temp=Temp.substr(Temp.find("/!/")+3);

  loginlocation=Temp.substr(0,Temp.find("/!/"));
  Temp=Temp.substr(Temp.find("/!/")+3);

  cryptpassword=Temp.substr(0,Temp.find(" "));
#ifdef DEBUG
  cout<<server<<"=server"<<endl<<
     loginlocation<<"=loca"<<endl<<
	 cryptpassword<<"=pw"<<endl;
#endif
}
char* GetResource(HINSTANCE ModuleHandle,int ResourceName
						,int ResourceTyp){
	HRSRC ResourceInfo;
	ResourceInfo=FindResource(ModuleHandle,
		MAKEINTRESOURCEA(ResourceName),MAKEINTRESOURCEA(ResourceTyp));
	if (ResourceInfo==0){
        #ifdef DEBUG
		  cout<<"resinfo"<<endl;
		#endif
		return 0;
	}
    char* Lol=(char*)LoadResource(ModuleHandle,ResourceInfo);
	return Lol;
}
