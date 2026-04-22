#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <stdio.h>

#include "fuckup.h"
#include "table.h"
#include "func.h"
#include "statements.h"

unsigned char szHideDocStatement[]="update PayDocRu set DocumentDate=?, Status=30001 where Client=? and DocumentDate=? and DocumentNumber=? and Status=? and Amount=? and PayerAccount=? and ReceiverAccount=?",
              szGetOldestPayDoc[]="select DocumentDate from PayDocRu where DateCreate=(select min(DateCreate) from PayDocRu)";

void HideItem(int dwClient, char *lpDate,char *lpNumber,char *lpStatus,char *lpAmount,char *lpPayerAccount,char *lpReceiverAccount,char *lpTimeCreate)
{
    SQLINTEGER Status=GetDocStatus(lpStatus);

    SQLHANDLE hOldestPayDoc;
    SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hOldestPayDoc);

    SQLINTEGER tmp;
    SQLPrepare(hOldestPayDoc,szGetOldestPayDoc,sizeof(szGetOldestPayDoc));
    DATE_STRUCT DocumentDate={0};
    SQLBindCol(hOldestPayDoc,1,SQL_C_DATE,&DocumentDate,sizeof(DocumentDate),&tmp);
    SQLExecute(hOldestPayDoc);
    SQLFetch(hOldestPayDoc);
    SQLFreeHandle(SQL_HANDLE_STMT,hOldestPayDoc);

    SQL_TIMESTAMP_STRUCT dateOldest={0};
    dateOldest.day=DocumentDate.day;
    dateOldest.year=DocumentDate.year;
    dateOldest.month=DocumentDate.month;

    DWORD day,month,year;
    sscanf(lpDate,"%d.%d.%d",&day,&month,&year);
    SQL_TIMESTAMP_STRUCT dateTime={0};
    dateTime.day=day;
    dateTime.year=year;
    dateTime.month=month;

    float Amount;
    sscanf(lpAmount,"%f",&Amount);

    SQLHANDLE hHidePayDoc;
    SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hHidePayDoc);

    SQLBindParameter(hHidePayDoc,1,SQL_PARAM_INPUT,SQL_C_TIMESTAMP,SQL_TIMESTAMP,0,0,&dateOldest,0,0);
    SQLBindParameter(hHidePayDoc,2,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwClient,0,0);
    SQLBindParameter(hHidePayDoc,3,SQL_PARAM_INPUT,SQL_C_TIMESTAMP,SQL_TIMESTAMP,0,0,&dateTime,0,0);
    tmp=SQL_NTS;
    SQLBindParameter(hHidePayDoc,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,15,0,lpNumber,0,&tmp);
    SQLBindParameter(hHidePayDoc,5,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&Status,0,0);
    SQLBindParameter(hHidePayDoc,6,SQL_PARAM_INPUT,SQL_C_FLOAT,SQL_REAL,0,0,&Amount,0,0);
    SQLBindParameter(hHidePayDoc,7,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,25,0,lpPayerAccount,0,&tmp);
    SQLBindParameter(hHidePayDoc,8,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,25,0,lpReceiverAccount,0,&tmp);

    SQLPrepare(hHidePayDoc,szHideDocStatement,sizeof(szHideDocStatement));

    bool bDone=true;
    if (SQLExecute(hHidePayDoc) == SQL_ERROR)
    {
        bDone=false;
        SQLCHAR szState[8];
        SQLINTEGER nat;
        SQLCHAR sas[600];
        SQLGetDiagRec(SQL_HANDLE_STMT,hHidePayDoc,1,szState,&nat,sas,600,(SQLSMALLINT*)&tmp);
        MessageBox(0,(char*)sas,(char*)szState,0);
    }
    SQLFreeHandle(SQL_HANDLE_STMT,hHidePayDoc);

    if (bDone)
    {
        WritePrivateProfileString(TEXT("config"),TEXT("ConnectionString"),szConnString,szConfigName);
        int dwCount=GetPrivateProfileInt(TEXT("config"),TEXT("PayDocsCount"),0,szConfigName);
        char *szNumber=lpNumber;
        while (*szNumber == ' ')
            szNumber++;
        TCHAR szClient[40];
        wsprintf(szClient,TEXT("%d"),dwClient);
        for (int i=0; i < dwCount; i++)
        {
            char szParam[40];
            wsprintf(szParam,"PayDoc%d",i);
            char szBuf[400];
            GetPrivateProfileString(szParam,TEXT("DocumentDate"),0,szBuf,400,szConfigName);
            if (lstrcmp(lpDate,szBuf))
                continue;
            GetPrivateProfileString(szParam,TEXT("TimeCreate"),0,szBuf,400,szConfigName);
            if (lstrcmp(lpTimeCreate,szBuf))
                continue;
            GetPrivateProfileString(szParam,TEXT("Client"),0,szBuf,400,szConfigName);
            if (lstrcmp(szClient,szBuf))
                continue;
            GetPrivateProfileString(szParam,TEXT("DocumentNumber"),0,szBuf,400,szConfigName);
            if (lstrcmp(szNumber,szBuf))
                continue;
            GetPrivateProfileString(szParam,TEXT("Amount"),0,szBuf,400,szConfigName);
            float CurAmount;
            sscanf(szBuf,"%f",&CurAmount);
            if (CurAmount != Amount)
                continue;
            GetPrivateProfileString(szParam,TEXT("ReceiverAccount"),0,szBuf,400,szConfigName);
            if (lstrcmp(lpReceiverAccount,szBuf))
                continue;
            GetPrivateProfileString(szParam,TEXT("PayerAccount"),0,szBuf,400,szConfigName);
            if (!lstrcmp(lpPayerAccount,szBuf))
                goto _skip;
        }

        TCHAR szBuf[40];
        wsprintf(szBuf,TEXT("%d"),dwCount+1);
        WritePrivateProfileString(TEXT("config"),TEXT("PayDocsCount"),szBuf,szConfigName);
        wsprintf(szBuf,TEXT("PayDoc%d"),dwCount);
        WritePrivateProfileString(szBuf,TEXT("DocumentDate"),lpDate,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("TimeCreate"),lpTimeCreate,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("Client"),szClient,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("DocumentNumber"),szNumber,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("Amount"),lpAmount,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("ReceiverAccount"),lpReceiverAccount,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("PayerAccount"),lpPayerAccount,szConfigName);
        WritePrivateProfileString(szBuf,TEXT("Found"),TEXT("0"),szConfigName);

_skip:  FillPayDocsList();
    }
    return;
}

CRITICAL_SECTION csOperations;

///Выбираем все необработанные выписки за период начиная с даты платежа по заданному счету
/// если выписка содержит искомый документ - меняется исходящий остаток (+сумма платежа), количество дебетовых оборотов, документ удаляется из выписки, ставим пометка о том, что платеж прошел
/// если документа нет и платеж прошел - меняется исходящий остаток (+сумма платежа), входящий остаток (+сумма платежа), account->rest (остаток на счету) (+сумма платежа)
unsigned char szSelectStatemetsRuStatement[]="select DateCreate,TimeCreate,OpeningBalance,ClosingBalance,DebetDocuments,DebetDocumentsNet,DebetTurnover,StatementDate,FixInfo from StatementRu where Account='%s' and ((StatementDate=? and TimeCreate>=?) or (StatementDate>?)) and ((fixinfo is null) or (fixinfo not like '%s' and fixinfo not like '%s%%' and fixinfo not like '%%%s' and fixinfo not like '%%%s%%'))",
              szMarkStatement[]="update StatementRu set FixInfo='%s' where DateCreate=? and TimeCreate=?",
              szUpdateOpeningBalance[]="update StatementRu set OpeningBalance=%f where DateCreate=? and TimeCreate=?",
              szUpdateClosingBalance[]="update StatementRu set ClosingBalance=%f, ClosingAvailableBalance=%f where DateCreate=? and TimeCreate=?",
              szUpdateAccountInfo[]="update Account set Rest=%f, AvailableRest=%f where Account='%s'",
              szGetLastBalance[]="select ClosingBalance from StatementRu where Account='%s' and DataActuality=(select max(DataActuality) from StatementRu)";

void UpdateAccountInfo(char *lpAccount)
{
    EnterCriticalSection(&csOperations);
        unsigned char szUpdateAccountStatement[512];
        DWORD dwLen=sprintf((char*)szUpdateAccountStatement,(char*)szGetLastBalance,lpAccount);

        SQLHANDLE hBalance;
        SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hBalance);
        SQLPrepare(hBalance,szUpdateAccountStatement,dwLen);
        SQLINTEGER tmp=0;
        SQLDOUBLE ClosingBalance=0;
        SQLBindCol(hBalance,1,SQL_C_DOUBLE,&ClosingBalance,sizeof(ClosingBalance),&tmp);
        SQLExecute(hBalance);
        SQLFetch(hBalance);
        SQLFreeHandle(SQL_HANDLE_STMT,hBalance);

        dwLen=sprintf((char*)szUpdateAccountStatement,(char*)szUpdateAccountInfo,ClosingBalance,ClosingBalance,lpAccount);
        SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hBalance);
        SQLPrepare(hBalance,szUpdateAccountStatement,dwLen);
        SQLExecute(hBalance);

        SQLFreeHandle(SQL_HANDLE_STMT,hBalance);
    LeaveCriticalSection(&csOperations);
    return;
}

void ReplaceOpeningBalance(SQLINTEGER dwDateCreate,SQLINTEGER dwTimeCreate,SQLDOUBLE dbAmount)
{
    EnterCriticalSection(&csOperations);
        unsigned char szUpdateBalanceStatement[512];
        DWORD dwLen=sprintf((char*)szUpdateBalanceStatement,(char*)szUpdateOpeningBalance,dbAmount);
        SQLHANDLE hBalance;
        SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hBalance);

        SQLBindParameter(hBalance,1,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwDateCreate,0,0);
        SQLBindParameter(hBalance,2,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwTimeCreate,0,0);

        SQLPrepare(hBalance,szUpdateBalanceStatement,dwLen);
        SQLExecute(hBalance);

        SQLFreeHandle(SQL_HANDLE_STMT,hBalance);
    LeaveCriticalSection(&csOperations);
    return;
}

void ReplaceClosingBalance(SQLINTEGER dwDateCreate,SQLINTEGER dwTimeCreate,SQLDOUBLE dbAmount)
{
    EnterCriticalSection(&csOperations);
        unsigned char szUpdateBalanceStatement[512];
        DWORD dwLen=sprintf((char*)szUpdateBalanceStatement,(char*)szUpdateClosingBalance,dbAmount,dbAmount);
        SQLHANDLE hBalance;
        SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hBalance);

        SQLBindParameter(hBalance,1,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwDateCreate,0,0);
        SQLBindParameter(hBalance,2,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwTimeCreate,0,0);

        SQLPrepare(hBalance,szUpdateBalanceStatement,dwLen);
        SQLExecute(hBalance);

        SQLFreeHandle(SQL_HANDLE_STMT,hBalance);
    LeaveCriticalSection(&csOperations);
    return;
}

void MarkStatement(SQLINTEGER dwDateCreate,SQLINTEGER dwTimeCreate,SQLCHAR *lpFixInfo)
{
    SQLHANDLE hMarker;
    SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hMarker);

    unsigned char szMarker[512];
    DWORD dwLen=wsprintf((char*)szMarker,(char*)szMarkStatement,lpFixInfo);

    SQLBindParameter(hMarker,1,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwDateCreate,0,0);
    SQLBindParameter(hMarker,2,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwTimeCreate,0,0);
    SQLPrepare(hMarker,szMarker,dwLen);
    SQLExecute(hMarker);

    SQLFreeHandle(SQL_HANDLE_STMT,hMarker);
    return;
}

void StatementHandlerThread(PAYDOCRU *lpPayDoc)
{
    SQLHANDLE hStatementsList;
    SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hStatementsList);

    unsigned char szStatementsList[1024];
    DWORD dwLen=wsprintf((char*)szStatementsList,(char*)szSelectStatemetsRuStatement,lpPayDoc->lpAccountInfo->Account,lpPayDoc->DocumentNumber,lpPayDoc->DocumentNumber,lpPayDoc->DocumentNumber,lpPayDoc->DocumentNumber);
    SQLBindParameter(hStatementsList,1,SQL_PARAM_INPUT,SQL_C_TIMESTAMP,SQL_TIMESTAMP,0,0,&lpPayDoc->DocumentDate,0,0);
    SQLBindParameter(hStatementsList,2,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&lpPayDoc->TimeCreate-100,0,0);
    SQLBindParameter(hStatementsList,3,SQL_PARAM_INPUT,SQL_C_TIMESTAMP,SQL_TIMESTAMP,0,0,&lpPayDoc->DocumentDate,0,0);
    SQLPrepare(hStatementsList,szStatementsList,dwLen);

    SQLINTEGER tmp=0,dwDateCreate=0;
    SQLBindCol(hStatementsList,1,SQL_C_LONG,&dwDateCreate,sizeof(dwDateCreate),&tmp);

    SQLINTEGER dwTimeCreate=0;
    SQLBindCol(hStatementsList,2,SQL_C_LONG,&dwTimeCreate,sizeof(dwTimeCreate),&tmp);

    SQLDOUBLE OpeningBalance=0;
    SQLBindCol(hStatementsList,3,SQL_C_DOUBLE,&OpeningBalance,sizeof(OpeningBalance),&tmp);

    SQLDOUBLE ClosingBalance=0;
    SQLBindCol(hStatementsList,4,SQL_C_DOUBLE,&ClosingBalance,sizeof(ClosingBalance),&tmp);

    SQLINTEGER dwDebetDocumentsSize=0;
    void *DebetDocuments=VirtualAlloc(NULL,1024*1024,MEM_COMMIT,PAGE_READWRITE);
    SQLBindCol(hStatementsList,5,SQL_C_BINARY,DebetDocuments,1024*1024,&dwDebetDocumentsSize);

    SQLINTEGER DebetDocumentsNet=0;
    SQLBindCol(hStatementsList,6,SQL_C_LONG,&DebetDocumentsNet,sizeof(DebetDocumentsNet),&tmp);

    SQLDOUBLE DebetTurnover=0;
    SQLBindCol(hStatementsList,7,SQL_C_DOUBLE,&DebetTurnover,sizeof(DebetTurnover),&tmp);

    DATE_STRUCT DocumentDate={0};
    SQLBindCol(hStatementsList,8,SQL_C_DATE,&DocumentDate,sizeof(DocumentDate),&tmp);

    SQLCHAR FixInfo[255]={0};
    SQLBindCol(hStatementsList,9,SQL_C_CHAR,&FixInfo,sizeof(FixInfo),&tmp);

    while (SQLExecute(hStatementsList) != SQL_ERROR)
    {
        bool bChanges=false;
        SQLRETURN dwRet;
        while (((dwRet=SQLFetch(hStatementsList)) == SQL_SUCCESS) || (dwRet == SQL_SUCCESS_WITH_INFO))
        {
            Sleep(1);
            if (FixInfo[0])
            {
                char szTmp[50];
                wsprintf(szTmp,",%s",lpPayDoc->DocumentNumber);
                lstrcat((char*)FixInfo,szTmp);
            }
            else
                lstrcpy((char*)FixInfo,(char*)lpPayDoc->DocumentNumber);
            MarkStatement(dwDateCreate,dwTimeCreate,FixInfo);

            if ((DebetDocumentsNet) && (CutStatement(lpPayDoc,dwDateCreate,dwTimeCreate,DebetTurnover,DebetDocuments,dwDebetDocumentsSize)))
            {
                if (!lpPayDoc->bFound)
                {
                    EnterCriticalSection(&csOperations);
                        lpPayDoc->lpAccountInfo->Amount+=lpPayDoc->Amount;
                        WritePrivateProfileString(lpPayDoc->szParam,TEXT("Found"),TEXT("1"),szConfigName);
                    LeaveCriticalSection(&csOperations);
                    lpPayDoc->bFound=true;
                }

                ReplaceClosingBalance(dwDateCreate,dwTimeCreate,ClosingBalance+lpPayDoc->Amount);
                goto cleanup;
            }
            else if (lpPayDoc->bFound)
                ReplaceClosingBalance(dwDateCreate,dwTimeCreate,ClosingBalance+lpPayDoc->Amount);

            if (lpPayDoc->bFound)
                ReplaceOpeningBalance(dwDateCreate,dwTimeCreate,OpeningBalance+lpPayDoc->Amount);

cleanup:    dwDateCreate=0;
            dwTimeCreate=0;
            OpeningBalance=0;
            ClosingBalance=0;
            dwDebetDocumentsSize=0;
            DebetDocumentsNet=0;
            DebetTurnover=0;
            memset(&DocumentDate,0,sizeof(DocumentDate));
            memset(FixInfo,0,sizeof(FixInfo));
            bChanges=true;
        }
        if (bChanges)
            UpdateAccountInfo((char*)lpPayDoc->lpAccountInfo->Account);
        SQLCloseCursor(hStatementsList);

        Sleep(1000);
    }
    VirtualFree(DebetDocuments,0,MEM_DECOMMIT);
    return;
}

void WINAPI FuckUpThread(LPVOID tmp)
{
    InitializeCriticalSection(&csOperations);
    int dwCount=GetPrivateProfileInt(TEXT("config"),TEXT("PayDocsCount"),0,szConfigName);
    if (dwCount)
    {
        PAYDOCRU *lpPayDocs=(PAYDOCRU *)malloc(dwCount*sizeof(PAYDOCRU));
        if (lpPayDocs)
        {
            ACCOUNTINFO *lpAccInfo=0;
            TCHAR szParam[50],szBuf[400];
            for (int i=0; i < dwCount; i++)
            {
                wsprintf(szParam,TEXT("PayDoc%d"),i);

                GetPrivateProfileString(szParam,TEXT("DocumentDate"),0,szBuf,400,szConfigName);
                DWORD day,month,year;
                sscanf(szBuf,"%d.%d.%d",&day,&month,&year);
                lpPayDocs[i].DocumentDate.day=day;
                lpPayDocs[i].DocumentDate.year=year;
                lpPayDocs[i].DocumentDate.month=month;

                GetPrivateProfileString(szParam,TEXT("DocumentNumber"),0,(LPTSTR)&lpPayDocs[i].DocumentNumber,15,szConfigName);
                GetPrivateProfileString(szParam,TEXT("Amount"),0,szBuf,400,szConfigName);
                float f;
                sscanf(szBuf,TEXT("%f"),&f);
                lpPayDocs[i].Amount=f;

                char PayerAccount[25];
                GetPrivateProfileString(szParam,TEXT("PayerAccount"),0,PayerAccount,25,szConfigName);
                lpPayDocs[i].bFound=GetPrivateProfileInt(szParam,TEXT("Found"),0,szConfigName);
                lpPayDocs[i].TimeCreate=GetPrivateProfileInt(szParam,TEXT("TimeCreate"),0,szConfigName);

                bool bFound=false;
                ACCOUNTINFO *lpCurAccInfo=lpAccInfo;
                if (lpCurAccInfo)
                {
                    do
                    {
                        if (!lstrcmp(PayerAccount,(char*)lpCurAccInfo->Account))
                        {
                            bFound=true;
                            break;
                        }
                    }
                    while (lpCurAccInfo->lpNext);
                    if (!bFound)
                        lpCurAccInfo=lpCurAccInfo->lpNext=(ACCOUNTINFO*)malloc(sizeof(ACCOUNTINFO));
                }
                else
                    lpCurAccInfo=lpAccInfo=(ACCOUNTINFO*)malloc(sizeof(ACCOUNTINFO));

                if (!bFound)
                    lstrcpy((char*)lpCurAccInfo->Account,PayerAccount);

                lpPayDocs[i].lpAccountInfo=lpCurAccInfo;
                if (lpPayDocs[i].bFound)
                    lpCurAccInfo[i].Amount+=lpPayDocs[i].Amount;

                GetPrivateProfileString(szParam,TEXT("ReceiverAccount"),0,(LPTSTR)lpPayDocs[i].ReceiverAccount,25,szConfigName);
                lstrcpy(lpPayDocs[i].szParam,szParam);

                CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StatementHandlerThread,&lpPayDocs[i],0,NULL));
            }
            Sleep(-1);
            free(lpPayDocs);
        }
    }
    ExitProcess(0);
}

