// clickvmconsole.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
extern void wrlog(char *filename,const char *p,bool writetime);
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
		//MessageBox(NULL, "vmconsole����cacvd_oldʧ��","",MB_OK);
		PrinErr("|401|");
		char szError[256];
		sprintf(szError, "CreateProcess failed (%d).", GetLastError());
		wrlog("CloudTerm\\cthandler.log",szError,true);
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	wrlog("CloudTerm\\cthandler.log","clickvmconsole����",true);
}
int main(int argc, char* argv[])
{
	if(argc<2)
	{
		//MessageBox(NULL,"clickvmconsole��������","error",0);
		PrinErr("|402|");
		return -1;
	}
	InitErr();
	string arg(argv[1]);
	string logmsg="���ղ�����";
	logmsg+=argv[1];
	string ccip=getenv("CCIP");
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
		{PrinErr("|404|");
		return -1;}
		if(send(Sock,"if=clickvmconsole\n",strlen("if=clickvmconsole\n"),0)==SOCKET_ERROR)
		{ 
			//MessageBox(NULL,"vmconsole������Ϣʧ��\n","ERROR",0);
			PrinErr("|404|");
			wlog("CloudTerm\\cthandler.log",true,"vmconsole������Ϣʧ�ܣ�"
				"if=clickvmconsole WGLE=%d\n",WSAGetLastError()); 
			return -1; 
		} 
		int bytes; 
		char cRecv[1024];
		memset(cRecv,0,1024);
		if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
		{ 
			//MessageBox(NULL, "vmconsole������Ϣʧ��","",MB_OK);
			PrinErr("|404|");
			wlog("CloudTerm\\cthandler.log",true
				,"recive data failed.WGLE=%d\n",WSAGetLastError()); 
			return -1; 
		}
		if (!strncmp(cRecv, "ok", 2))
		{
			//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 
			if(send(Sock,SendInfo.c_str(),strlen(SendInfo.c_str()),0)==SOCKET_ERROR)
			{ 
				//MessageBox(NULL,"vmconsole������Ϣʧ��\n","ERROR",0);
				PrinErr("|404|");
				wlog("CloudTerm\\cthandler.log",true,"vmconsole������Ϣʧ�ܣ�"
					"vmid=userid= WGLE=%d\n",WSAGetLastError()); 
				return -1; 
			} 
			if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
			{ 
				//MessageBox(NULL, "vmconsole������Ϣʧ��","",MB_OK);
				PrinErr("|404|");
				wlog("CloudTerm\\cthandler.log",true
					,"recive data failed.WGLE=%d\n",WSAGetLastError()); 
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

