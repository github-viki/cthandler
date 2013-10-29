// clickvmconsole.cpp : �������̨Ӧ�ó������ڵ㡣
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
#include "simpleDll.h"
using namespace std;

#include <stdlib.h>
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"simpleDll.lib")
extern void wrlog(char *filename,const char *p,bool writetime);
int tmp_sender(string ip,string port)
{
	WSADATA wsa; 
	//��ʼ���׽���DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","socket initialize failed.\n",true); 
		return -1; 
	} 
	//�����׽��� 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","create socket failed.\n",true); 
		return -1; 
	} 
	struct sockaddr_in serverAddress; 
	memset(&serverAddress,0,sizeof(sockaddr_in)); 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = inet_addr(ip.c_str()); 
	serverAddress.sin_port = htons(atoi(port.c_str())); 
	//�����ͷ����������� 
	if(connect(sock,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
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
void VdProcess(char *cRecv)
{
	string vnc_path;
	vnc_path = GetModuleDir() + "\\if\\cacvd_old.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	string sRecv(cRecv);
	char szAppname[64], szIp[64], szPort[16], szPassword[256];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szPassword, sizeof(szPassword));
	int nIndex(-1);
	(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
		wrlog("CloudTerm\\cthandler.log","vd command error.",true);
	wrlog("CloudTerm\\cthandler.log","cRecv:",true);
	wrlog("CloudTerm\\cthandler.log",cRecv,true);
	int i = 0;
	while (cRecv[nIndex] != '*')
	{
		szAppname[i++] = cRecv[nIndex++];
	}
	szAppname[i]='\0';
	wrlog("CloudTerm\\cthandler.log",szAppname,true);
	string sAppname(szAppname);
	if (szAppname[0] == '\0')
	{
		//memcpy(szAppname, "cacrd", strlen("cacrd"));
		sAppname = "cacvd";
	}
	else
	{

		ConvertUtf8ToGBK(sAppname);
	}
	//wrlog("CloudTerm\\cthandler.log",szAppname,true);
	wrlog("CloudTerm\\cthandler.log",sAppname.c_str(),true);
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
	//if string is empty
	if (szPort[0] == '\0')
	{
		memcpy(szPort, "5901", strlen("5901"));
	}

	string scmd;
	scmd = vnc_path + " -title " + "\"" + sAppname + "\" " + szIp + ":" + szPort;

	string logmsg("�������򼰴��������");
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
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
		char szError[256];
		sprintf(szError, "CreateProcess failed (%d).", GetLastError());
		wrlog("CloudTerm\\cthandler.log",szError,true);
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	wrlog("CloudTerm\\cthandler.log","vdcomplete",true);
}
int _tmain(int argc, _TCHAR* argv[])
{
	if(argc<2)
	{
		MessageBox(NULL,"param error","error",0);
		return -1;
	}
	string arg(argv[1]);
	string logmsg="���ղ�����";
	logmsg+=argv[1];
	string ccip(argv[2]);
	wrlog("CloudTerm\\cthandler.log",logmsg.c_str(),true);
	wrlog("CloudTerm\\cthandler.log",ccip.c_str(),true);
	//������ͷ�cc
	//���������̨ʵ��
	int loc;

	if ((loc = arg.find("|vmid")) != string::npos)
	{
		string SendInfo = arg;
		SOCKET Sock = tmp_sender(ccip, "50000");
		if(Sock==-1)
		{return -1;}
		if(send(Sock,"if=clickvmconsole\n",strlen("if=clickvmconsole\n"),0)==SOCKET_ERROR)
		{ 
			MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
			wrlog("CloudTerm\\cthandler.log","send data failed.",true); 
			return -1; 
		} 
		int bytes; 
		char cRecv[1024];
		memset(cRecv,0,1024);
		if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
		{ 
			MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
			wrlog("CloudTerm\\cthandler.log","recive data failed.",true); 
			return -1; 
		}
		if (!strncmp(cRecv, "ok", 2))
		{
			//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 
			if(send(Sock,SendInfo.c_str(),strlen(SendInfo.c_str()),0)==SOCKET_ERROR)
			{ 
				MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
				wrlog("CloudTerm\\cthandler.log","send data failed.",true); 
				GetLastError();
				return -1; 
			} 
			if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
			{ 
				MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
				wrlog("CloudTerm\\cthandler.log","recive data failed.",true); 
				return -1; 
			}
		}
		closesocket(Sock);
			if (strstr(cRecv, "|infotype=vd|") != NULL)
			{
				VdProcess(cRecv);
			}
		}

	
	return 0;
}
