#include <Winsock2.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <Wtsapi32.h>
#include <io.h>
#include <ctime>
#include <direct.h>
using namespace std;
void wrlog(char *filename,const char *p,bool writetime){
	FILE *fp;
	char cmd[200];
	SYSTEMTIME nowTime;                            
	GetLocalTime(&nowTime);	
	char szTime[128];
	ZeroMemory(szTime, sizeof(szTime));
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.wYear, nowTime.wMonth, nowTime.wDay, 
		nowTime.wHour, nowTime.wMinute, nowTime.wSecond);


	if( (_access( "C:\\Users\\Public", 0 )) == 0 )
	{
		if( (_access( "C:\\Users\\Public\\CloudTerm", 0 )) == -1 )
		{
			mkdir("C:\\Users\\Public\\CloudTerm");
		}
		sprintf(cmd,"C:\\Users\\Public\\%s",filename);
	}
	else
	{
		if( (_access( "C:\\Documents and Settings\\All Users\\CloudTerm", 0 )) == -1 )
		{
			mkdir("C:\\Documents and Settings\\All Users\\CloudTerm");
		}
		sprintf(cmd,"C:\\Documents and Settings\\All Users\\%s",filename);	
	}



	fp=fopen(cmd,"a+");
	if(fp==NULL) return ;
	if(writetime==true)
	{fprintf(fp,"%s %s\n",szTime,p);}
	else
	{fprintf(fp,"%s\n",p);}
	fclose(fp);
}
int sender(string ip,string port,string info,char *cRecv)
{
 WSADATA wsa; 
 //初始化套接字DLL 
 if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
 { 
	wrlog("cthandler.txt","socket initialize failed.\n",true); 
    return -1; 
 } 
 //创建套接字 
 SOCKET sock; 
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
 { 
	wrlog("cthandler.txt","create socket failed.\n",true); 
	return -1; 
 } 
 struct sockaddr_in serverAddress; 
 memset(&serverAddress,0,sizeof(sockaddr_in)); 
 serverAddress.sin_family=AF_INET; 
 serverAddress.sin_addr.S_un.S_addr = inet_addr(ip.c_str()); 
 serverAddress.sin_port = htons(atoi(port.c_str())); 
 //建立和服务器的连接 
 if(connect(sock,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR)
 { 
	  wrlog("cthandler.txt","connect failed.\n",true); 
	  return -1; 
 } 
 if(send(sock,info.c_str(),strlen(info.c_str()),0)==SOCKET_ERROR)
 { 
	   wrlog("cthandler.txt","send data failed.\n",true); 
	   return -1; 
 } 
 int bytes; 
 if((bytes = recv(sock, cRecv, 1024, 0)) == SOCKET_ERROR)
 { 
	   wrlog("cthandler.txt","recive data failed.\n",true); 
	   return -1; 
 }
 if(bytes == 0)
	  wrlog("cthandler.txt","recieved 0 byte",true);
  cRecv[bytes]='\0'; 
  closesocket(sock);
  WSACleanup();

  //printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
  return bytes;
}