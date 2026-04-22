#ifndef STATEMENTS_H_INCLUDED
#define STATEMENTS_H_INCLUDED

bool CutStatement(PAYDOCRU *lpPayDoc,SQLINTEGER dwDateCreate,SQLINTEGER dwTimeCreate,SQLDOUBLE DebetTurnover,void *lpDebetDocuments,DWORD lpDebetDocumentsSize);

struct DEBETDOC_HDR
{
   DWORD dwSize;
   DWORD dwDummy0;
   DWORD dwPayDocsCount;
};

struct COLUMN_HDR
{
   DWORD wType;
   BYTE bDummy0[0x0C];
   DWORD dwNameSize;
   DWORD wColumnSize;
   char szColumnName[0];
};

struct VIRTUAL_TABLE
{
    DWORD AcceptDate;
    DWORD AcceptDocDate;
    DWORD AcceptNumber;
    DWORD AcceptOperType;
    DWORD AcceptPartNumber;
    DWORD AcceptRestAmount;
    DWORD Amount;
    DWORD AmountNat;
    DWORD BankOfficials;
    DWORD CBCcode;
    DWORD CashSymbol;
    DWORD CurrCode;
    DWORD DocDateParam;
    DWORD DocNumParam;
    DWORD DocRef;
    DWORD DocumentDate;
    DWORD DocumentNumber;
    DWORD Ground;
    DWORD LineNumber;
    DWORD NoteFromBank;
    DWORD OKATOcode;
    DWORD OperType;
    DWORD OrderPayer;
    DWORD OrderPayerAccount;
    DWORD OrderReceiver;
    DWORD OrderReceiverAccount;
    DWORD PRZO;
    DWORD PayGrndParam;
    DWORD PayTypeParam;
    DWORD PayUntil;
    DWORD Payer;
    DWORD PayerAccount;
    DWORD PayerBIC;
    DWORD PayerBankName;
    DWORD PayerBankType;
    DWORD PayerCorrAccount;
    DWORD PayerINN;
    DWORD PayerKPP;
    DWORD PayerPlace;
    DWORD PayerPlaceType;
    DWORD PayerPropertyType;
    DWORD PaymentUrgent;
    DWORD Receiver;
    DWORD ReceiverAccount;
    DWORD ReceiverBIC;
    DWORD ReceiverBankName;
    DWORD ReceiverBankType;
    DWORD ReceiverCorrAccount;
    DWORD ReceiverINN;
    DWORD ReceiverKPP;
    DWORD ReceiverPlace;
    DWORD ReceiverPlaceType;
    DWORD ReceiverPropertyType;
    DWORD SavedDocRef;
    DWORD SendType;
    DWORD Stat1256;
    DWORD TaxPeriodParam;
    DWORD ValueDate;
    DWORD ViewFlags;
    DWORD kbOpID;
};

#endif // STATEMENTS_H_INCLUDED
