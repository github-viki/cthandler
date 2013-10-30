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
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib,"cloudenc.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "ws2_32.lib")
char appid[128];
string rdp_path = "cacrd.exe";
string vnc_path = "cacvd.exe";
string vdp_path ="vncviewer.exe";
string lc_path="caclc.exe";
string xw_path="xwkalc.exe";
char accountid[20]={0};
char vmpassportdes[1024]={0};
char vmpassport[256]={0};
string arc_path="";
string ccip;
char userid[128];
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
int CommuToVm(string ip)
{
	SOCKET sockToVm=tmp_sender(ip,"50000");
	if(sockToVm==-1)
	{return -1;}
	char SendInfo[256]={'\0'};
	int bytes;
	//SendInfo="cdc:|"+userid+"|"+passport+"|\n";
	if(send(sockToVm,"if=vmuser\n",strlen("if=vmuser\n"),0)==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","send data failed.",true); 
		return -1; 
	} 
	char tmp[10];
	if((bytes = recv(sockToVm, tmp, 10, 0)) == SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","recive data failed.",true); 
		return -1; 
	}
	if (strncmp(tmp,"ok",2)!=0)
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","ok?",true); 
		return -1; 
	}

	sprintf(SendInfo,"cdc:|%s|%s|\n",userid,passportdes);
	if(send(sockToVm,SendInfo,strlen(SendInfo),0)==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","send data failed.",true); 
		return -1; 
	} 
	wrlog("CloudTerm\\cthandler.log",tmp,true);

	char cRecv[1024];

	if((bytes = recv(sockToVm, accountid, 1024, 0)) == SOCKET_ERROR)
	{ 
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","recive data failed.",true); 
		return -1; 
	}
	accountid[bytes]='\0';
	if (strncmp(accountid,"update",6)==0)
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","accounitd error",true);
		wrlog("CloudTerm\\cthandler.log",accountid,true); 
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
	wrlog("CloudTerm\\cthandler.log",vmpassport,true);
	//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 

	wrlog("CloudTerm\\cthandler.log",accountid,true); 
	return 0;
}
void RcProcess(char *cRecv)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	string sRecv(cRecv);

	char szAppname[64], szIp[64], szPort[16], szPassword[256],szapp[256];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szPassword, sizeof(szPassword));
	int nIndex(-1);
	(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
		wrlog("CloudTerm\\cthandler.log","rc command error.",true);
	wrlog("CloudTerm\\cthandler.log","crecv:",true);
	wrlog("CloudTerm\\cthandler.log",cRecv,true);
	int i = 0;
	while (cRecv[nIndex] != '*')
	{
		szAppname[i++] = cRecv[nIndex++];
	}
	szAppname[i]='\0';
	string sAppname(szAppname);
	if (szAppname[0] == '\0')
	{
		//memcpy(szAppname, "cacrd", strlen("cacrd"));
		sAppname = "cacrc";
	}
	else
	{

		ConvertUtf8ToGBK(sAppname);
	}
	nIndex++;
	i = 0;
	wrlog("CloudTerm\\cthandler.log",sAppname.c_str(),true);
	while (cRecv[nIndex] != '*')
	{
		szIp[i++] = cRecv[nIndex++];
	}
	nIndex++;
	i = 0;

	while (cRecv[nIndex] != '*')
	{
		szPort[i++] = cRecv[nIndex++];
	}
	szPort[i]='\0';
	nIndex++;
	i=0;
	//if string is empty
	if (szPort[0] == '\0')
	{
		memcpy(szPort, "3389", strlen("3389"));
	}
	while (cRecv[nIndex]!='|')
	{
		szapp[i++]=cRecv[nIndex++];
	}
	szapp[i]='\0';
	if(CommuToVm(szIp)!=0)
	{
		MessageBox(NULL,"hzw","eee",0);
		exit(-1);
	}
	string scmd;
	scmd = rdp_path + " -t " + "\"" + sAppname + "\" " + " -s "+szIp + " -d " + szPort+" -u "+vmuserid+" -p "+vmpassport+" -e "+"\""+szapp+"\"";
	string logmsg("启动程序及传入参数：");
	logmsg+=scmd;
	//wrlog("CloudTerm\\cthandler.log",logmsg.c_str(),true);
	if( !CreateProcess(NULL,   // No module name (use command line)
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
	}
	else{
		wrlog("CloudTerm\\cthandler.log","rc complete",true);
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
}
void LcProcess(char *cRecv)
{
	string sRecv(cRecv);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	char szAppname[64], szIp[64], szPort[16], szPassword[256];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szPassword, sizeof(szPassword));
	string ::size_type tmp=sRecv.find('*');

	string cmd;
	string scmd;


	string ::size_type loc=sRecv.find("|setting");
	if(loc==string::npos)
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		return ;
	}

	cmd.assign(sRecv.begin()+loc,sRecv.end());

	scmd="\""+lc_path+"\""+" "+"\""+cmd+"\"";


	wrlog("CloudTerm\\cthandler.log",scmd.c_str(),true);
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
		wrlog("CloudTerm\\cthandler.log","LC complete",true);
	}
}
void XwProcess(char *cRecv)
{
	string sRecv(cRecv);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	string scmd;
	char szName[64],szPassword[256],szapp[1024],szvms[2048];
		ZeroMemory(szName, sizeof(szName));
		ZeroMemory(szPassword, sizeof(szPassword));
		ZeroMemory(szapp,sizeof(szapp));
		int nIndex(-1);
		(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
			wrlog("CloudTerm\\cthandler.log","vd command error.",true);
		wrlog("CloudTerm\\cthandler.log","cRecv:",true);
		wrlog("CloudTerm\\cthandler.log",cRecv,true);
		int i = 0;
		while (cRecv[nIndex] != '*')
		{
			szName[i++] = cRecv[nIndex++];
		}
		szName[i]='\0';


		string sAppname(szName);
		if (szName[0] == '\0')
		{
			//memcpy(szAppname, "cacrd", strlen("cacrd"));
			sAppname = "xiaowei";
		}
		else
		{

			ConvertUtf8ToGBK(sAppname);
		}
		wrlog("CloudTerm\\CloudTerm.log",sAppname.c_str(),true);
		nIndex++;
		i=0;
		while (cRecv[nIndex] != '*')
		{
			szvms[i++] = cRecv[nIndex++];
		}
		szvms[i]='\0';
		nIndex++;
		i = 0;

		while (cRecv[nIndex] != '|')
		{
			szapp[i++] = cRecv[nIndex++];
		}
		szapp[i]='\0';
		nIndex++;
		i=0;
		//if string is emp
		string tmpip;
		tmpip=ccip;
		int tmp;
		if((tmp=tmpip.find("\n"))!=string::npos)
			tmpip.erase(tmp,1);
		scmd="\""+xw_path+"\" "+tmpip+" "+userid+" "+sAppname+" "+passport+" "+szapp+" "+szvms;
		wlog("CloudTerm\\cthandler.log",true,"xwkalcscmd:%s\n",scmd.c_str());
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
			cout<<"eee"<<endl;
			wrlog("CloudTerm\\cthandler.log","LC complete",true);
		}
}
void VdpProcess(char *cRecv)
{
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
	wrlog("CloudTerm\\CloudTerm.log",szAppname,true);
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

	while (cRecv[nIndex] != '|'&&cRecv[nIndex]!='*')
	{
		szIp[i++] = cRecv[nIndex++];
	}
	nIndex++;
	i = 0;


	//if string is empty

	memcpy(szPort, "7000", strlen("7000"));


	string scmd;
	scmd = "\""+vdp_path+"\"" + " "+ szIp + "::" + szPort+" -user "+userid+" "+"-password "+passport;

	string logmsg("启动程序及传入参数：");
	logmsg+=scmd;
	wlog("CloudTerm\\cthandler.log",true,"vdp:%s\n",logmsg.c_str());
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
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	wrlog("CloudTerm\\cthandler.log","vdcomplete",true);
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

	while (cRecv[nIndex] != '|'&&cRecv[nIndex]!='*')
	{
		szPort[i++] = cRecv[nIndex++];
	}
	if (szPort[0] == '\0')
	{
		memcpy(szPort, "3389", strlen("3389"));
	}
	if(CommuToVm(szIp)!=0)
	{
		MessageBox(NULL,"hzw","eee",0);
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
	wlog("CloudTerm\\cthandler.log",true,"%s\n",scmd.c_str());
	//调用apprd.exe apprc.exe arg也要用引号，因为arg中有可能有空格，而出现错误。	
	CallApp(scmd);
	return 0;
}
void configuration()
{
	string config_path = GetModuleDir();

	int find = config_path.find_last_of("\\");
	string path = config_path.assign(config_path.begin(),config_path.begin() + find);
	arc_path= path+"\\xw\\ARCClient.exe";
	xw_path=path+"\\ct\\xw\\xwkalc.exe";
	rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
	vnc_path = GetModuleDir() + "\\if\\cacvd_old.exe";
	lc_path=GetModuleDir()+"\\if\\caclc.exe";
	vdp_path=GetModuleDir()+"\\if\\vncviewer.exe";
	//char a_ip[64] = {0};
	//char b_ip[64]={0};

	//FILE *fp = NULL;

	//while ((fp = fopen(path.c_str(),"r")) == NULL)
	//{
	//	cout<<"file open error"<<endl;
	//}
	//fgets(a_ip,64,fp);
	//if (!judgeIpConnect(a_ip,50000))
	//{
	//	fgets(b_ip,64,fp);
	//	strcpy(a_ip,b_ip);
	//HKEY Hkey;
	//memset(passport,0,128);
	//memset(passportdes,0,128);
	//DWORD length;
	//length=128;
	//RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\CloudTerm\\Settings",0,KEY_ALL_ACCESS,&Hkey);
	////RegSetValue(Hkey,"passport",REG_SZ,"d/NKBOa2RoQ=",5);
	//RegQueryValueEx(Hkey,"passport",NULL,NULL,(BYTE*)passportdes,&length);
	////wrlog("\\CloudTerm\\cthandler.log",passportdes,true);
	//RegCloseKey(Hkey);
	//desdec(passportdes,passport,"caclab");

}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=3)
	{
		MessageBox(NULL,"param error","error",0);
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
	configuration();
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



