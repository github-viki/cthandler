#include <string.h>
#include <cstdio>
#include <iostream>
#include <Winsock2.h>
#pragma comment(lib,"Ws2_32.lib") 
using namespace std; 
SOCKET GetParentProcessSock()
{
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int err; 
	WSAPROTOCOL_INFO lp;
	SOCKET sockSrv;
	FILE *fp=NULL;
	wVersionRequested = MAKEWORD( 1, 1 ); 
	err = WSAStartup( wVersionRequested, &wsaData ); 
	if ( err != 0 ) { 
		return -1; 
	}  
	if(LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion ) != 1 )
		{ 
//			MessageBox(NULL,"2222",NULL,0);
			WSACleanup();
//			MessageBox(NULL,"3333",NULL,0);
			return -1;
		} 
	if(!(fp=fopen("c:\\inher.pid","rb")))
	{
//		MessageBox(NULL,"1111",NULL,0);
		return -1;
		//wr_Log("Read inher.pid failed");
	}
	else
	{
		fread(&lp,sizeof(WSAPROTOCOL_INFO),1,fp);
		fclose(fp);
		remove("c:\\inher.pid");
		if(LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion ) != 1 )
		{ 
			WSACleanup();
//			MessageBox(NULL,"3333",NULL,0);
			return -1;
		} 
		sockSrv=WSASocket(AF_INET, SOCK_RAW, NULL, &lp, 0, 0);
		return sockSrv;
	}
}
int main(){
	SOCKET sock;
	sock=GetParentProcessSock();
	if(sock!=-1){
	//	你可以在这里添加代码开始使用sock传输东西 例如： send(sock,"i am example",strlen("i am rc.exe")+1,0);
    //  代码从这里开始添加：
		
	}
	else{
		printf("error!\n");
	}
	closesocket(sock);
	return 0;
}
