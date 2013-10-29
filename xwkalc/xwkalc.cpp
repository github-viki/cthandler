// xwkalc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "string"
#include <stdio.h>

using namespace std;
#pragma comment(lib, "ws2_32.lib")
char IP[16];
char SessionID[16];
void GetLocalhostIP();
void inet_ntop_my(u_long src, char *dst, size_t size);
int tmp_sender(string ip,string port)
{
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
//		wr_Log("socket initialize failed.\n"); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	//	wr_Log("create socket failed.\n"); 
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
//		wr_Log("connect failed.\n"); 
		return -1; 
	} 
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
	return sock;
}
int GetSessionId();
string GetModuleDir();
void GetLocalhostIP()
{
	char hostname[50];
	struct addrinfo *answer, hint;
	memset(&hint,0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	gethostname(hostname,50);
	int ret = getaddrinfo(hostname, NULL, &hint, &answer);
	inet_ntop_my(((sockaddr_in*)answer->ai_addr)->sin_addr.S_un.S_addr,IP,50);
}
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc!=6)
	{
		MessageBox(NULL,"ARGCERROR","",0);
		return -1;
	}
	GetLocalhostIP();
	GetSessionId();
	int Sock=tmp_sender("192.168.1.201","20000");
	if (Sock==-1)
	{
		MessageBox(NULL,"SOCKERROR","",0);
		return -1;
	}
	char sendInfo[128];
	sprintf(sendInfo,"appid=%s*userid=%s\n",argv[2],argv[3]);
	char cRecv[2048];
	if(send(Sock,sendInfo,strlen(sendInfo),0)==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		//wrlog("CloudTerm\\Crphandler.log","send data failed.",true); 
		return -1; 
	} 
	int bytes;
	if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		//wrlog("CloudTerm\\Crphandler.log","recive data failed.",true);  
		return -1; 
	}
	cRecv[bytes]='\0';
	//close(Sock);
	shutdown(Sock,NULL);
	int Sock1=tmp_sender("192.168.1.201","34567");
	if (Sock1==-1)
	{
		MessageBox(NULL,"SOCKERROR","",0);
		return -1;
	}
	char sendInfo1[1024];
	sprintf(sendInfo1,"%s*3*%s*%s*%s*|type=xw|xwvms=%s|\n",argv[2],argv[1],IP,SessionID,cRecv);
	char cRecv1[2048];
	if(send(Sock1,sendInfo1,strlen(sendInfo1),0)==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		//wrlog("CloudTerm\\Crphandler.log","send data failed.",true); 
		return -1; 
	} 
	if((bytes = recv(Sock1, cRecv1, 1024, 0)) == SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		//wrlog("CloudTerm\\Crphandler.log","recive data failed.",true);  
		return -1; 
	}
	cRecv1[bytes]='\0';
	cout<<cRecv1;
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&HeartBeat,(VOID*)&Sock1,0,NULL);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	string scmd;
	char openxw[1024]={"openxw"};
	scmd="\""+openxw+"\""+" "+cRecv1+" "+argv[1]+" "+argv[4]+" "+argv[5];
	MessageBox(NULL,"openxw","openxw",0);
	if( !CreateProcess( NULL,   // No module name (use command line)
		const_cast <char *>(scmd.c_str()),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		char szError[256];
		sprintf(szError, "CreateProcess failed (%d).", GetLastError());
		//wrlog("CloudTerm\\Crphandler.log",szError,true);
		return;
	}
	else
	{
		WaitForSingleObject(pi.hProcess,INFINITE);
		//wrlog("CloudTerm\\Crphandler.log","rd complete",true);
	}
	return 0;
}

void inet_ntop_my(u_long src, char *dst, size_t size) 
{ 
	const char digits[] = "0123456789"; 
	int i; 
	//struct in_addr *addr = (struct in_addr *)src; 
	//u_long a = ntohl(addr->s_addr);
	u_long a = src;
	const char *orig_dst = dst; 

	if (size < INET_ADDRSTRLEN) { 
		errno = ENOSPC; 
		return ; 
	} 
	for (i = 3; i >= 0; --i) { 
		int n = (a >> (24 - i * 8)) & 0xFF; 
		int non_zerop = 0; 

		if (non_zerop || n / 100 > 0) { 
			*dst++ = digits[n / 100]; 
			n %= 100; 
			non_zerop = 1; 
		} 
		if (non_zerop || n / 10 > 0) { 
			*dst++ = digits[n / 10]; 
			n %= 10; 
			non_zerop = 1; 
		} 
		*dst++ = digits[n]; 
		if (i != 0) 
			*dst++ = '.'; 
	} 
	*dst++ = '\0'; 
	return ; 
} 
int GetSessionId()
{
	DWORD sessionidNUM=0;
	ProcessIdToSessionId(GetCurrentProcessId(),&sessionidNUM);
	sprintf(SessionID,"%d",sessionidNUM);
	return 0;
} 
//int getCCipRandom(char ips[][24],int *ipcount)
//{
//	string strCCconfPath = GetModuleDir(); 
//	strCCconfPath = strCCconfPath.substr(0, strCCconfPath.rfind('\\'));
//	strCCconfPath = strCCconfPath + "\\conf\\ccip"; 
//
//	FILE *file = fopen(strCCconfPath.c_str(), "r");
//
//	if(file == NULL)
//	{
//		return 0;
//	}
//	int count = 0;
//	char iptemp[30][24];
//	while(fgets(iptemp[count], 24, file) != NULL)
//	{
//		int j = strlen(iptemp[count]);
//		iptemp[count][j] = '\0';
//		count++;
//	}
//	int k;
//	//////////////////////////////////////补尾法求不重复的随机数//////////////////////////
//
//	int *A=new int[count]();
//	int *B=new int[count]();
//
//	if(count>=2)
//	{
//
//		int s=2*count;
//		int lb=0;
//		srand( (unsigned)time( NULL ) );
//		for(int i=0;i<s;i++)
//		{
//			k = rand() %count+1;
//			if(A[k-1]==0)
//			{
//				A[k-1]=k;
//				B[lb]=k;
//				lb++;
//			}
//			if(lb>count)
//			{break;}
//		}
//
//		if(lb<count)
//		{
//			for(int j=0;j<count;j++)
//			{
//				if(A[j]==0)
//				{
//					A[j]=j+1;
//					B[lb]=j+1;
//					lb++;
//				}
//				if(lb>count)
//				{break;}
//			}
//		}
//
//		for(int f=0;f<count;f++)
//		{
//			sprintf(ips[f],"%s",iptemp[B[f]-1]);
//		}
//
//	}
//	else if(count==1)
//	{
//		sprintf(ips[0],"%s",iptemp[0]);
//	}
//	else
//	{
//		return 0;
//	}
//	*ipcount=count;
//
//	delete A;
//	delete B;
//	fclose(file);
//	return 1;
//
//}
static unsigned __stdcall HeartBeat(void * sock)
{
	char recvBuf[10];
	int sockfd;
	sockfd=(int)sock;
	while(1)
	{send(sockfd,"ok",2,0);
	int n=recv(sockfd,recvBuf,2,0);
	if(n<=0)
	{
		break;
	}
	Sleep(3000);
	}

}
string GetModuleDir() 
{ 
	HMODULE module = GetModuleHandle(0); 
	char pFileName[MAX_PATH]; 
	GetModuleFileName(module, pFileName, MAX_PATH); 

	string csFullPath(pFileName); 
	int nPos = csFullPath.rfind('\\'); 
	if( nPos == string::npos ) 
		return string(""); 
	else 
		return csFullPath.substr(0, nPos); 
}
