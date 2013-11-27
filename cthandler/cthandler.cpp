// cthandler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock.h>
#include "simpleDll.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "ws2_32.lib")
int InitSocket(string ip,string port)
{
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"socket initialize failed.\n"); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"create socket failed.\n"); 
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
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"connect failed.\n"); 
		return -1; 
	} 
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
	return sock;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//if(argc<2)
	//{
	//	MessageBox(NULL,"argc","error",0);
	//	return -1;
	//}
	//SOCKET sockfd=InitSocket("127.0.0.1","50000");
	//if(sockfd==-1)
	//{return -1;}
	//char SendInfo[256]={'\0'};
	//int bytes;
	////SendInfo="cdc:|"+userid+"|"+passport+"|\n";
	//if(send(sockfd,"if=cthandler\n",strlen("if=cthandler\n"),0)==SOCKET_ERROR)
	//{ 
	//	MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	//	wlog("CloudTerm\\cthandler.log",true,"send data failed."); 
	//	return -1; 
	//} 
	//char tmp[10];
	//if((bytes = recv(sockfd, tmp, 10, 0)) == SOCKET_ERROR)
	//{ 
	//	MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	//	wlog("CloudTerm\\cthandler.log",true,"recive data failed."); 
	//	return -1; 
	//}
	//if (strncmp(tmp,"ok",2)!=0)
	//{
	//	MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	//	wlog("CloudTerm\\cthandler.log",true,"ok?"); 
	//	return -1; 
	//}
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",argv[1]);
	//char sendBuf[1024]={'0'};
	//sprintf(sendBuf,"%s\n",argv[1]);
	//if(send(sockfd,sendBuf,strlen(sendBuf),0)==SOCKET_ERROR)
	//{ 
	//	MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	//	wlog("CloudTerm\\cthandler.log",true,"send data failed."); 
	//	return -1; 
	//}
	//closesocket(sockfd);
	TCHAR lpchbuf[64]={'0'};
	DWORD dSize=64;
	if(!GetUserName(lpchbuf,&dSize))
	{
		wlog("CloudTerm\\cthandler.log",true,"GetUserNameError %d\n",GetLastError());
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"username %s\n",lpchbuf);
	TCHAR pChPipeName[128]={'0'};
	sprintf(pChPipeName,"\\\\.\\pipe\\CTLIS%s",lpchbuf);
	wlog("CloudTerm\\cthandler.log",true,"pchpipename %s\n",pChPipeName);
	if (WaitNamedPipe(pChPipeName, NMPWAIT_WAIT_FOREVER) == FALSE)
	{
		wlog("CloudTerm\\cthandler.log",true,"waitnamedpipe %d\n",GetLastError());
		return -1;
	}
	HANDLE hPipe = CreateFile(pChPipeName, GENERIC_READ | GENERIC_WRITE,0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		wlog("CloudTerm\\cthandler.log",true,"createfile %d\n",GetLastError());
		return -1;
	}

	else
	{
		wlog("CloudTerm\\cthandler.log",true,"write suc\n");
	}
	// 向管道写入数据
	DWORD WriteNum=0;
	memset(lpchbuf,0,64);
	sprintf(lpchbuf,"%s",argv[1]);
	if (WriteFile(hPipe, lpchbuf, strlen(lpchbuf), 
		&WriteNum, NULL) == FALSE)
	{
		wlog("CloudTerm\\cthandler.log",true,"write file %d\n",GetLastError());
		return -1;
	} else {
		wlog("CloudTerm\\cthandler.log",true,"write %s\n",lpchbuf);

	}
	CloseHandle(hPipe); // 关闭管道句柄
	return 0;
	
}

