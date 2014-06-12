// clicktplconsole.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
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
		wlog("CloudTerm\\cthandler.log",true,"vd command error.");
	wlog("CloudTerm\\cthandler.log",true,"cRecv:%s",cRecv);
	//wrlog("CloudTerm\\cthandler.log",cRecv,true);
	int i = 0;
	while (cRecv[nIndex] != '*')
	{
		szAppname[i++] = cRecv[nIndex++];
	}
	szAppname[i]='\0';
	//wlog("CloudTerm\\cthandler.log",szAppname,true);
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
	//wrlog("CloudTerm\\cthandler.log",sAppname.c_str(),true);
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
		//MessageBox(NULL, "����cacvd_old.exeʧ��","",MB_OK);
		PrinErr("|401|");
		char szError[256];
		sprintf(szError, "CreateProcess failed (%d).\n", GetLastError());
		wlog("CloudTerm\\cthandler.log",true,"%s\n",szError);
		//wrlog("CloudTerm\\cthandler.log",szError,true);
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	wlog("CloudTerm\\cthandler.log",true,"clicktplconsole����\n");
}
int main(int argc, char* argv[])
{
	if(argc<2)
	{
		PrinErr("|402|");
		return -1;
	}
	InitErr();
	string arg(argv[1]);
	string logmsg="���ղ�����";
	logmsg+=argv[1];
	wlog("CloudTerm\\cthandler.log",true,"%s\n",logmsg.c_str());
	string ccip;
	ccip=getenv("CCIP");
	wlog("CloudTerm\\cthandler.log",true,"%s\n",ccip.c_str());
	//������ͷ�cc
	//���������̨ʵ��
	int loc;
	if ((loc = arg.find("|tplid")) != string::npos)
	{
		string SendInfo = arg;
		SOCKET Sock = tmp_sender(ccip, "50000");
		if(Sock==-1)
		{
			//MessageBox(NULL,"tplconsole����CCʧ��\n","ERROR",0);
			PrinErr("|404|");
			return -1;
		}
		if(send(Sock,"if=clicktplconsole\n",strlen("if=clicktplconsole\n"),0)==SOCKET_ERROR)
		{ 
			//MessageBox(NULL,"tplconsole������Ϣʧ��\n","ERROR",0);
			PrinErr("|404|");
			wlog("CloudTerm\\cthandler.log",true,"tplconsole������Ϣʧ�ܣ�"
				"if=clicktplconsole WGLE=%d\n",WSAGetLastError()); 
			return -1; 
		} 
		int bytes; 
		char cRecv[1024];
		memset(cRecv,0,1024);
		if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
		{ 
			//MessageBox(NULL, "tplconsole������Ϣʧ��","",MB_OK);
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
				//MessageBox(NULL,"tplconsole������Ϣʧ��\n","ERROR",0);
				PrinErr("|404|");
				wlog("CloudTerm\\cthandler.log",true,"tplconsole������Ϣʧ�ܣ�"
					"tplid=userid= WGLE=%d\n",WSAGetLastError()); 
				return -1; 
			} 
			if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
			{ 
				//MessageBox(NULL, "tplconsole������Ϣʧ��","",MB_OK);
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


