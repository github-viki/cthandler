// apprd.cpp : 定义控制台应用程序的入口点。
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
#include "cloudenc.h"
#include "simpleDll.h"
using namespace std;

#include <stdlib.h>
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib,"cloudenc.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "ws2_32.lib")
char accountid[20]="";
char vmuserid[20]="";
char vmpassport[128]="";
char vmpassportdes[256]="";
char *userid;
char *password;
char passworddes[1024]={'0'};
int tmp_sender(string ip,string port)
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
int CommuToVm(string ip,string szGranule,string szCmd)
{
	//char *appid="23";
	char *appid;
	appid=getenv("APPID");
	SOCKET sockToVm=tmp_sender(ip,"50000");
	if(sockToVm==-1)
	{return -1;}
	char SendInfo[256]={'\0'};
	int bytes;
	//SendInfo="cdc:|"+userid+"|"+passport+"|\n";
	if(send(sockToVm,"if=vmuser\n",strlen("if=vmuser\n"),0)==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:send data failed.\n"); 
		return -1; 
	} 
	char tmp[10];
	if((bytes = recv(sockToVm, tmp, 10, 0)) == SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:recive data failed.\n"); 
		return -1; 
	}
	if (strncmp(tmp,"ok",2)!=0)
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:ok?\n"); 
		return -1; 
	}

	sprintf(SendInfo,"cdc:|%s|%s|%s|%s|%s|\n",userid,passworddes,appid,szGranule.c_str(),szCmd.c_str());
	//wlog("CloudTerm\\cthandler.log",true,"send data failed.");
	if(send(sockToVm,SendInfo,strlen(SendInfo),0)==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:send data failed.\n"); 
		return -1; 
	} 
//	wlog("CloudTerm\\cthandler.log",true,"%s\n",tmp);

	char cRecv[1024];

	if((bytes = recv(sockToVm, accountid, 1024, 0)) == SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:recive data failed.\n"); 
		return -1; 
	}
	accountid[bytes]='\0';
	closesocket(sockToVm);
	if (strncmp(accountid,"fail",4)==0)
	{
		MessageBox(NULL, "请联系管理员，计算资源不足！","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:accounitd error");
		wlog("CloudTerm\\cthandler.log",true,"ERROR:%s\n",accountid); 
		return -1;
	}
	int i,j;
	i=0;
	j=0;
	while(accountid[i]!='|')
		i++;
	i++;
	while(accountid[i]!='|')
	{
		vmuserid[j++]=accountid[i++];
	}
	vmuserid[j]='\0';
	i++;
	j=0;
	while(accountid[i]!='|'&&accountid[i]!='\0')
	{
		vmpassportdes[j++]=accountid[i++];
	}
	vmpassportdes[j]='\0';
	desdec(vmpassportdes,vmpassport,"cac");
//	wlog("CloudTerm\\cthandler.log",true,"%s",vmpassport);
	//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 

	//wlog("CloudTerm\\cthandler.log",true,"%s",accountid); 
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
	userid=getenv("USERID");
	if(userid==NULL)
	{
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s\n",userid);
	password=getenv("PASSWORD");
	if(password==NULL)
		return -1;
	//userid="1";
	//password="111111";
	wlog("CloudTerm\\cthandler.log",true,"%s\n",password);
	desenc(password,passworddes,"cac");
	wlog("CloudTerm\\cthandler.log",true,"%s\n",passworddes);
	string sRecv(argv[1]);
	wlog("CloudTerm\\cthandler.log",true,"%s\n",sRecv.c_str());
	int loc1;
	((loc1=sRecv.find('\"'))!=string::npos)?sRecv.erase(loc1,1):sRecv;
	wlog("CloudTerm\\cthandler.log",true,"%s\n",sRecv.c_str());  
	//wlog("CloudTerm\\cthandler.log",true,"apprd start\n");
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",argv[1]);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char szAppname[64], szIp[64], szPort[16], szUsername[64], szPassword[256];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szUsername, sizeof(szUsername));
	ZeroMemory(szPassword, sizeof(szPassword));
	int nIndex(-1);
	(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
		wlog("CloudTerm\\cthandler.log",true,"rd command error");
	
	if(nIndex==-1) {MessageBox(NULL,"计算资源不足，请联系管理员",argv[0],0);return -1;}
	int i = 0;
	while (sRecv[nIndex] != '*')
	{
		szAppname[i++] = sRecv[nIndex++];
	}
	szAppname[i]='\0';
	string sAppname(szAppname);
	//wrlog("CloudTerm\\CloudTerm.log",szAppname,true);
	if (szAppname[0] == '\0')
	{
		sAppname = "cacrd";
	}
	
	//wrlog("CloudTerm\\CloudTerm.log",sAppname.c_str(),true);
	nIndex++;
	i = 0;

	while (sRecv[nIndex] != '*')
	{
		szIp[i++] = sRecv[nIndex++];
	}
	nIndex++;
	i = 0;

	while (sRecv[nIndex] != '|'&&sRecv[nIndex]!='*')
	{
		szPort[i++] = sRecv[nIndex++];
	}
	if (szPort[0] == '\0')
	{
		memcpy(szPort, "3389", strlen("3389"));
	}
	nIndex++;
	i=0;
	char szapp[256]={'0'};
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*')
	{
		szapp[i++]=sRecv[nIndex++];
	}
	szapp[i]='\0';
	char szGranule[8]={'0'};
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		szGranule[i++]=sRecv[nIndex++];
	}
	if(CommuToVm(szIp,szGranule,szapp)!=0)
	{
	//	MessageBox(NULL,"hzw","eee",0);
		exit(-1);
	}
	nIndex++;
	i = 0;
	string rdp_path;
	rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
	string scmd;
	scmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "+ szPort+" -u "+vmuserid+" -p "+vmpassport;
	string logmsg("启动程序及传入参数：");
	logmsg+=scmd;
	wlog("CloudTerm\\cthandler.log",true,"%s\n",logmsg.c_str());

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
		sprintf(szError, "ERROR:CreateProcess failed (%d).", GetLastError());
		wlog("CloudTerm\\cthandler.log",true,"%s",szError);
		return -1;
	}
	else
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		wlog("CloudTerm\\cthandler.log",true,"apprd complete\n");
	}
	return 0;
}