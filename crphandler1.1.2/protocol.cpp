/********************************************************************
created:	2011/12/08
created:	8:12:2011   14:46
filename: 	d:\实验室项目\crphandler_v1.0.0\protocol.cpp
file path:	d:\实验室项目\crphandler_v1.0.0
file base:	protocol
file ext:	cpp
author:		wongson

purpose:	crpprotocal implementation

Fixed:		2012/12/26	增加写日志
Fixed:      2013/07/17  cthandler完全作为一个中转程序。与服务器通信将交给二级程序。
Fixed:		2014/01/09	删除不必要的信息
*********************************************************************/
#include "split.h"
#include <stdio.h>
#include <windows.h>
#include <Wtsapi32.h>
#include <algorithm>
#include <direct.h>
#include <iostream>
#include <string>
#include <io.h>
#include "cloudenc.h"
#include <ctime>
#include <WinSock2.h>
#include "simpleDll.h"
#include <stdlib.h>
string ccip;
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib,"ws2_32.lib")
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
int tmp_sender(string ip,string port);
void wrlog(char *filename,const char *p,bool writetime);
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
int judgeIpConnect(char ip[],int port)
{//判断是否连接服务器，连接返回1，不连接返回0
	//初始化Socket库，保证ws2_32.dll已经加载
	SOCKADDR_IN clientService;//地址
	SOCKET ConnetctSocket;//Socket
	WSADATA wsaData;//库
	int iResult=WSAStartup(MAKEWORD(2,2),&wsaData);
	if (iResult!=NO_ERROR)
	{

		//printf("Error at WSAStartup()\n");

		return 0;
	}
	//创建Socket

	ConnetctSocket=socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP
		);
	unsigned long ul = 1;  
	//int nZero = 0;
	//setsockopt(ConnetctSocket, SOL_SOCKET, SO_SNDBUF, (const char *)&nZero, sizeof(nZero));
	//setsockopt(ConnetctSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&nZero, sizeof(nZero));
	//////设置发送和接收超时
	//int timeOut = 5000;
	//setsockopt(ConnetctSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&timeOut, sizeof(timeOut));
	//setsockopt(ConnetctSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&timeOut, sizeof(timeOut));

	struct linger sLinger;
	sLinger.l_onoff = 1;
	setsockopt(ConnetctSocket, SOL_SOCKET, SO_LINGER, (const char *)&sLinger, sizeof(sLinger));
	ioctlsocket(ConnetctSocket, FIONBIO, &ul);
	if (ConnetctSocket==INVALID_SOCKET)
	{   
		//char socketcreate[256];

		//sprintf(socketcreate,"Error at socket():%ld\n",WSAGetLastError());
		//printf(socketcreate);

		WSACleanup();
		return 0;
	}
	//设置服务端的通信协议，IP地址、端口
	clientService.sin_family=AF_INET;
	clientService.sin_addr.s_addr=inet_addr(ip);
	clientService.sin_port=htons(port);
	//连接到服务端
	connect(
		ConnetctSocket,
		(SOCKADDR*)&clientService,
		sizeof(clientService)
		);


		//printf("ip:%s可以连接到服务器!\n",ip);
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(ConnetctSocket,&fs);
	struct timeval timeout;
	timeout.tv_sec = 3; //连接超时3秒
	timeout.tv_usec =0;
	int ret;
	ret = select(0, 0, &fs, 0, &timeout);
	if ( ret <= 0 )
	{
		closesocket(ConnetctSocket);
		return 0;
	}else{
		closesocket(ConnetctSocket);  
		WSACleanup();
		return 1;}
}
int getCCipRandom(char ips[][24],int *ipcount)
{
	wlog("CloudTerm\\cthandler.log",true,"getcciprandom\n");
	char iptemp[30][24]={'0'};
	int count=0;
	if (getenv("CCNUM")==0)
	{
		wlog("CloudTerm\\cthandler.log",true,"getenv error\n");
		return -1;
	}
	count=atoi(getenv("CCNUM"));
	wlog("CloudTerm\\cthandler.log",true,"%d\n",count);
	char tmp[10]={'0'};
	for (int i=0;i<count;i++)
	{
		
		sprintf(tmp,"CC%d",i);
		if (getenv(tmp)==0)
		{
			wlog("CloudTerm\\cthandler.log",true,"getenv error\n");
			return -1;
		}
		strcpy(iptemp[i],getenv(tmp));
		wlog("CloudTerm\\cthandler.log",true,"ip:%s\n",iptemp[i]);
	}
	int k;
	//////////////////////////////////////补尾法求不重复的随机数//////////////////////////
	
	int *A=new int[count]();
	
	int *B=new int[count]();

	if(count>=2)
	{

		int s=2*count;
		int lb=0;
		srand( (unsigned)time( NULL ) );
		for(int i=0;i<s;i++)
		{
			k = rand() %count+1;
			if(A[k-1]==0)
			{
				A[k-1]=k;
				B[lb]=k;
				lb++;
			}
			if(lb>count)
			{break;}
		}

		if(lb<count)
		{
			for(int j=0;j<count;j++)
			{
				if(A[j]==0)
				{
					A[j]=j+1;
					B[lb]=j+1;
					lb++;
				}
				if(lb>count)
				{break;}
			}
		}

		for(int f=0;f<count;f++)
		{
			sprintf(ips[f],"%s",iptemp[B[f]-1]);
		}

	}
	else if(count==1)
	{
		sprintf(ips[0],"%s",iptemp[0]);
	}
	else
	{
		return 0;
	}
	*ipcount=count;

	delete A;
	delete B;
	//fclose(file);
	return 1;

}
int configuration()
{
	string config_path = GetModuleDir();
	int find = config_path.find_last_of("\\");
	string path = config_path.assign(config_path.begin(),config_path.begin() + find);
	char ccipArray[30][24];
	int ipcount;
	string ip;
	if(!getCCipRandom(ccipArray,&ipcount))
	{MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	wlog("CloudTerm\\cthandler.log",true,"ERROR:read ip FAILED\n");
	return -1;}
	int i;
	for (i=0;i<ipcount;i++)
	{
		if (!judgeIpConnect(ccipArray[i],50000))
		{
			continue;
		}
		else
		{
			ip=ccipArray[i];
			wlog("CloudTerm\\cthandler.log\n",true,"%d  %s",i,ccipArray[i]);
			break;
		}
	}
	if (i<ipcount)
	{
		ccip = ip;
	}
	else{
	wlog("CloudTerm\\cthandler.log",true,"ip has over\n");
	return -1;
	}
	return 0;
}
int tmp_sender(string ip,string port)
{
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wr_Log("socket initialize failed.\n"); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wr_Log("create socket failed.\n"); 
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
		wr_Log("connect failed.\n"); 
		return -1; 
	} 
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
	return sock;
}
int CallApp(string path)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	if( !CreateProcess(NULL,   // No module name (use command line)
		const_cast <char *>(path.c_str()),        // Command line
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
		sprintf(szError, "ERROR:CreateProcess failed (%d).\n", GetLastError());
		wrlog("CloudTerm\\cthandler.log",szError,true);
	return -1;
	}
	else{
		wrlog("CloudTerm\\cthandler.log"," caccthandler:complete\n",true);
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return 0;
	}
}
int main(int argc, char **argv)
{
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
	string arg(argv[1]);
	//wlog("CloudTerm\\cthandler.log",true,"recv:%s",arg.c_str());
	if(configuration())
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		return -1;
	}
	//string arg="crp:|infotype=clicktplconsole|tplid=|userid=1|";
	string logmsg="接收参数：";
	logmsg+=arg;
	wlog("CloudTerm\\cthandler.log",true,"%s:%s\n",argv[0],logmsg.c_str());
	//接命令就发cc
	//虚拟机控制台实现
	int loc;
	string scmd="";
	string cmd="";
	if ((loc = arg.find("infotype")) != string::npos)
	{
	string appname;
	loc=arg.find("=");
	arg.assign(arg.begin()+loc+1,arg.end());
	loc=arg.find("|");
	appname.assign(arg.begin(),arg.begin()+loc);
	scmd.assign(arg.begin()+loc,arg.end());
	cmd="\""+appname+".exe"+"\" "+scmd+" "+ccip;
	//得到infotype如clickapp 或者是clicktplconsole等。
	wlog("CloudTerm\\cthandler.log",true,"%s",cmd.c_str());
	CallApp(cmd);
	}
	else
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","ERROR:Ca Request Illeage!",true);
	}
}
