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
//#include <windows.h>
#include "cloudenc.h"
#include <ctime>
#include <WinSock2.h>
#include "tstclient.h"
#include "simpleDll.h"
#pragma comment(lib, "tstclient.lib")
#pragma comment(lib, "cloudenc.lib")

#include <stdlib.h>
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"simpleDll.lib")
string GetModuleDir();

string ccip = "", ccport = "", termid = "", mode = "";
char appid[128];
string rdp_path = "cacrd.exe";
string vnc_path = "cacvd.exe";
string vdp_path ="vncviewer.exe";
string lc_path="caclc.exe";
string xw_path="xwkalc.exe";
string vmconsole="clickvmconsole.exe";
string vmdesktop="clickvmdesktop.exe";
string tplconsole="clicktplconsole.exe";
string tpldesktop="clicktpldesktop.exe";
string clickapp="clickapp.exe";
string arc_path="";
char vmuserid[26];
char vmpassport[255];
char vmpassportdes[1024];
char accountid[16];
char userid[128];
char passport[128];
char passportdes[128];
int handler(char *cRecv);
extern int sender(string ip,string port,string info,char *cRecv);
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

char* ConstructRandomKey()
{
	const int SIZE_CHAR = 32;  //生成32 + 1位C Style字符串
	const char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
	srand((unsigned)time(NULL));
	char *cDeskeyR = new char[SIZE_CHAR + 1]; 
	memset(cDeskeyR, 0, SIZE_CHAR + 1);
	for (int i = 0; i < SIZE_CHAR; ++i)
	{
		int x = rand() % (sizeof(CCH) - 1);
		cDeskeyR[i] = CCH[x];
	}
	return cDeskeyR;
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
int getCCipRandom(char ips[][24],int *ipcount)
{
	/*string strCCconfPath = GetModuleDir(); 
	strCCconfPath = strCCconfPath.substr(0, strCCconfPath.rfind('\\'));
	strCCconfPath = strCCconfPath + "\\conf\\ccip"; 
	wlog("CloudTerm\\cthandler.log",true,"ippath:%s\n",strCCconfPath.c_str());
	FILE *file = fopen(strCCconfPath.c_str(), "r");
	if(file == NULL)
	{
		return 0;
	}
	int count = 0;
	char iptemp[30][24];
	while(fgets(iptemp[count], 24, file) != NULL)
	{
		int j = strlen(iptemp[count]);
		iptemp[count][j] = '\0';
		count++;
	}*/
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
int tmp_sender(string ip,string port);



int configuration()
{
	wlog("CloudTerm\\cthandler.log",true,"configuration\n");
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
	//}
	//string ip(a_ip);
	char ccipArray[30][24];
	int ipcount;
	string ip;
	if(!getCCipRandom(ccipArray,&ipcount))
	{MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	wlog("CloudTerm\\cthandler.log",true,"read ip FAILED\n");
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
	else{//MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
	wlog("CloudTerm\\cthandler.log",true,"ip has over\n");
	return -1;
	}
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
	return 0;
}


const char* CopyByDelimiter(char *pDes, const char *pSrc, char cDelimiter)
{
	while((*pDes++ = *pSrc++) != cDelimiter);
	*(--pDes) = '\0';
	return pSrc;
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
	wr_Log(vmuserid);
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
	scmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "+ szPort+" -u "+userid+" -p "+passport;

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

void VdProcess(char *cRecv)
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
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	wrlog("CloudTerm\\cthandler.log","vdcomplete",true);
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
	char szName[64],szPassword[
		256],szapp[1024],szvms[2048];
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

int handler(char *cRecv)
{

	//rd |infotype=rd|setting=appname*ip*port*username*password|
	if (strstr(cRecv, "|infotype=rd|") != NULL)
	{
		RdProcess(cRecv);
	}
	//vd |infotype=vd|setting=appname*ip*port*password|
	else if (strstr(cRecv, "|infotype=vd|") != NULL)
	{
		VdProcess(cRecv);
	}
	else if(strstr(cRecv,"|infotype=rc|")!=NULL)
	{
		RcProcess(cRecv);
	}
	else if(strstr(cRecv,"|infotype=lc|")!=NULL)
	{
		LcProcess(cRecv);
	}
	else if (strstr(cRecv,"|infotype=xw|")!=NULL)
	{
		XwProcess(cRecv);
	}
	else if(strstr(cRecv,"|infotype=vdp|")!=NULL)
	{
		VdpProcess(cRecv);
	}
	else
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wr_Log("received bad command from ccserver.");
	}
	return 0;
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
		sprintf(szError, "CreateProcess failed (%d).", GetLastError());
		wrlog("CloudTerm\\cthandler.log",szError,true);
	return -1;
	}
	else{
		wrlog("CloudTerm\\cthandler.log"," complete",true);
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return 0;
	}
}
SOCKET GetParentProcessSock()
{
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int err; 
	WSAPROTOCOL_INFO lp;
	SOCKET sockSrv;
	FILE *fp=NULL;
	wVersionRequested = MAKEWORD( 1, 1 ); 
	err = WSAStartup( wVersionRequested, &wsaData ); 
	if ( err != 0 ) { 
		return -1; 
	}  
	if(LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion ) != 1 )
	{ 
		//			MessageBox(NULL,"2222",NULL,0);
		WSACleanup();
		//			MessageBox(NULL,"3333",NULL,0);
		return -1;
	} 
	if(!(fp=fopen("c:\\inher.pid","rb")))
	{
		//		MessageBox(NULL,"1111",NULL,0);
		return -1;
		//wr_Log("Read inher.pid failed");
	}
	else
	{
		fread(&lp,sizeof(WSAPROTOCOL_INFO),1,fp);
		fclose(fp);
		remove("c:\\inher.pid");
		if(LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion ) != 1 )
		{ 
			WSACleanup();
			//			MessageBox(NULL,"3333",NULL,0);
			return -1;
		} 
		sockSrv=WSASocket(AF_INET, SOCK_RAW, NULL, &lp, 0, 0);
		return sockSrv;
	}
}
int main(int argc, char **argv)
{
	/*SOCKET sockfd;
	sockfd=GetParentProcessSock();
	if(sockfd==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"socket inherit error\n");
		return -1;
	}
	char recvbuf[256]={'0'};
	if(SOCKET_ERROR==recv(sockfd,recvbuf,256,0))
	{
		wlog("CloudTerm\\cthandler.log",true,"recvError%d\n",WSAGetLastError());

		return -1;
	}
	closesocket(sockfd);*/
	string arg(argv[1]);
	wlog("CloudTerm\\cthandler.log",true,"recv:%s",arg.c_str());
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
	wlog("CloudTerm\\cthandler.log",true,"%s",cmd.c_str());
	CallApp(cmd);
	
	}
	else
	{
		MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wrlog("CloudTerm\\cthandler.log","Ca Request Illeage!",true);
	}
}
