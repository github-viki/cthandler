// clicktplconsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
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
using namespace std;

#include <stdlib.h>
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "ws2_32.lib")
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
		wrlog("CloudTerm\\cthandler.log","socket initialize failed.\n",true); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","create socket failed.\n",true); 
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
		wrlog("CloudTerm\\cthandler.log","connect failed.\n",true); 
		return -1; 
	} 
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
	return sock;
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
void RdProcess(char *cRecv)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	string sRecv(cRecv);
	char szAppname[64], szIp[64], szPort[16], szUsername[64], szPassword[256];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szUsername, sizeof(szUsername));
	ZeroMemory(szPassword, sizeof(szPassword));
	string rdp_path;
	rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
	int nIndex(-1);
	(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
		wrlog("CloudTerm\\cthandler.log","rd command error.",true);
	wrlog("CloudTerm\\cthandler.log","crecv:",true);
	wrlog("CloudTerm\\cthandler.log",cRecv,true);
	int i = 0;
	while (cRecv[nIndex] != '*')
	{
		szAppname[i++] = cRecv[nIndex++];
	}
	szAppname[i]='\0';
	string sAppname(szAppname);
	//wrlog("CloudTerm\\CloudTerm.log",szAppname,true);
	if (szAppname[0] == '\0')
	{
		sAppname = "cacrd";
	}
	else
	{	    
		ConvertUtf8ToGBK(sAppname);
	}
	//wrlog("CloudTerm\\CloudTerm.log",sAppname.c_str(),true);
	nIndex++;
	i = 0;

	while (cRecv[nIndex] != '*')
	{
		szIp[i++] = cRecv[nIndex++];
	}
	nIndex++;
	i = 0;

	while (cRecv[nIndex] != '|')
	{
		szPort[i++] = cRecv[nIndex++];
	}
	if (szPort[0] == '\0')
	{
		memcpy(szPort, "3389", strlen("3389"));
	}
	nIndex++;
	i = 0;

	string scmd;
	scmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "+ szPort+" -u "+"Administrator"+" -p "+"654321";

	string logmsg("启动程序及传入参数：");
	logmsg+=scmd;
	//wrlog("CloudTerm\\cthandler.log",logmsg.c_str(),true);

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
		wrlog("CloudTerm\\cthandler.log",szError,true);
		return;
	}
	else
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		wrlog("CloudTerm\\cthandler.log","rd complete",true);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	if(argc<3)
	{
		MessageBox(NULL,"param error","error",0);
		return -1;
	}
	string arg(argv[1]);
	string logmsg="接收参数：";
	logmsg+=argv[1];
	string ccip(argv[2]);
	wrlog("CloudTerm\\cthandler.log",logmsg.c_str(),true);
	wrlog("CloudTerm\\cthandler.log",ccip.c_str(),true);
	//接命令就发cc
	//虚拟机控制台实现
	int loc;

	if ((loc = arg.find("|tplid")) != string::npos)
	{
		string SendInfo=arg;
		SOCKET Sock = tmp_sender(ccip, "50000");
		if(Sock==-1)
		{return -1;}
		if(send(Sock,"if=clicktplconsole\n",strlen("if=clicktplconsole\n"),0)==SOCKET_ERROR)
		{ 
			MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
			wrlog("CloudTerm\\cthandler.log","send data failed.",true); 
			return -1; 
		} 
		int bytes; 
		char cRecv[1024];
		memset(cRecv,0,1024);
		if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
		{ 
			MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
			wrlog("CloudTerm\\cthandler.log","recive data failed.",true); 
			return -1; 
		}
		if (!strncmp(cRecv, "ok", 2))
		{
			//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 
			if(send(Sock,SendInfo.c_str(),strlen(SendInfo.c_str()),0)==SOCKET_ERROR)
			{ 
				MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
				wrlog("CloudTerm\\cthandler.log","send data failed.",true); 
				GetLastError();
				return -1; 
			} 
			if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
			{ 
				MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
				wrlog("CloudTerm\\cthandler.log","recive data failed.",true); 
				return -1; 
			}
		}
		if (strstr(cRecv, "|infotype=rd|") != NULL)
		{
			RdProcess(cRecv);
		}
		else
		{
			MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
			return -1;
		}
	}
	else
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		return -1;
	}

	return 0;
}


