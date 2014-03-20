// clickapp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <Wtsapi32.h>
#include <algorithm>
#include <direct.h>
#include <iostream>
#include <string>
#include <io.h>
#include <windows.h>
#include <ctime>
#include <WinSock.h>
#include "cloudenc.h"
#include "simpleDll.h"
using namespace std;

#include <stdlib.h>
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "ws2_32.lib")
string ccip;
char userid[128];
char appid[128];
char passport[128];
char passportdes[128];
char vmuserid[10];
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
int tmp_sender(string ip,string port)
{
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:socket initialize failed.\n"); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	//DWORD bufLength=1460*4;
	//setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&bufLength,sizeof(bufLength));
	//setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&bufLength,sizeof(bufLength));
	//setsockopt(sock,SOL_SOCKET,SO,&bufLength,sizeof(bufLength));
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:create socket failed.\n"); 
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
		wlog("CloudTerm\\cthandler.log",true,"ERROR:connect failed.\n"); 
		return -1; 
	} 
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
	return sock;
}
int NonBlockNetComm(string ip,string port)
{
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:socket initialize failed.\n"); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	int ret;
	unsigned long ul=1;
	ret=ioctlsocket(sock,FIONBIO,(unsigned long *)&ul);
	if(ret)
	{
		wlog("CloudTerm\\cthandler.log",true,"ioctlsocket %d\n",WSAGetLastError());
		return -1;
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
void ConvertUtf8ToGBK(string& strUtf8) 
{
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8.c_str(), -1, NULL,0);
	WCHAR* wszGBK = new WCHAR[len+1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8.c_str(), -1, wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK=new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte (CP_ACP, 0, wszGBK, -1, szGBK, len, NULL,NULL);
	strUtf8 = szGBK;
	delete[] szGBK;
	delete[] wszGBK;
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
		wlog("CloudTerm\\cthandler.log",true,szError);
		return -1;
	}
	else{
		wlog("CloudTerm\\cthandler.log",true,"clickapp complete\n");
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return 0;
	}
}
int handler(char *cRecv)
{
	string arg=cRecv;
	ConvertUtf8ToGBK(arg);
	int locin;
	while((locin=(arg.find("\"")))!=string::npos)
	{
		arg.erase(locin,1);
	}
	if(arg.find("infotype")==string::npos)
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:%s",arg.c_str());
		return -1;
	}
	int loc=arg.find("=");
	arg.assign(arg.begin()+loc+1,arg.end());
	loc=arg.find("|");
	string scmd="";
	scmd.assign(arg.begin(),arg.begin()+loc);
	arg.assign(arg.begin()+loc,arg.end());
	scmd="\"app"+scmd+".exe\" "+"\""+arg+"\""+" "+userid;
	//有两个参数 第一个参数为收到的信息，第二个参数为userid
	//命令为apprx.exe |setting=name*172.20.251.113*3389*opener C:\WINDOWS\system32\mspaint.exe*1| userid
	wlog("CloudTerm\\cthandler.log",true,"%s\n",scmd.c_str());
	//调用apprd.exe apprc.exe arg也要用引号，因为arg中有可能有空格，而出现错误。	
	CallApp(scmd);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=3)
	{
		MessageBox(NULL,"param error","error",0);
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
	char tmp[1024];
	sprintf(tmp,"%s",argv[1]);
	char *vikitmp;
	vikitmp=strstr(tmp,"|userid");
	int i=0;
	while (vikitmp[i]!='=')
	{i++;
	}
	i++;
	int k=0;
	while(vikitmp[i]!='|')
	{
		userid[k]=vikitmp[i];
		i++;
		k++;
	}
	userid[k]='\0';
	k=0;i=0;
	if((vikitmp=strstr(tmp,"|appid"))>0)
	{
		while (vikitmp[i]!='=')
		{i++;
		}
		i++;
		k=0;
		while(vikitmp[i]!='|')
		{
			appid[k]=vikitmp[i];
			i++;
			k++;
		}
	}
	i=0;k=0;
	char env[128]={'0'};
	sprintf(env,"USERID=%s",userid);
	if(_putenv(env))
	{
		wlog("CloudTerm\\cthandler.log",true,"ERROR:userid write to env\n");
	}
	memset(env,128,0);
	sprintf(env,"APPID=%s",appid);
	if(_putenv(env))
	{
		wlog("CloudTerm\\cthandler.log",true,"ERROR:appid write to env \n");
	}
	string arg(argv[1]);
	string logmsg="接收参数：";
	logmsg+=argv[1];
	ccip=argv[2];
	memset(env,128,0);
	sprintf(env,"CCIP=%s",ccip.c_str());
	if(_putenv(env))
	{
		wlog("CloudTerm\\CloudTerm.log",true,"ERROR:put env ccip \n");
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:%s %s\n",argv[0],logmsg.c_str(),ccip.c_str());
	//接命令就发cc
	//虚拟机控制台实现
	int loc;

	if ((loc = arg.find("|appid")) != string::npos)
	{
		string SendInfo = arg;
		SOCKET Sock = tmp_sender(ccip, "50000");
		if(Sock==-1)
		{return -1;}
		if(send(Sock,"if=clickapp\n",strlen("if=clickapp\n"),0)==SOCKET_ERROR)
		{ 
			MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
			wlog("CloudTerm\\cthandler.log",true,"ERROR:send data failed."); 
			return -1; 
		} 
		int bytes; 
		char cRecv[1024];
		memset(cRecv,0,1024);
		if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
		{ 
			MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
			wlog("CloudTerm\\cthandler.log",true,"ERROR:recive data failed."); 
			return -1; 
		}
		if (!strncmp(cRecv, "ok", 2))
		{
			//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 
			if(send(Sock,SendInfo.c_str(),strlen(SendInfo.c_str()),0)==SOCKET_ERROR)
			{ 
				MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
				wlog("CloudTerm\\cthandler.log",true,"ERROR:send data failed."); 
				//GetLastError();
				return -1; 
			} 
			if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
			{ 
				MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
				wlog("CloudTerm\\cthandler.log",true,"ERROR:recive data failed."); 
				return -1; 
			}
		}
		closesocket(Sock);
		handler(cRecv);
	}
	return 0;
}




