// apprc.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
//#define ERRDLL_IM extern "C" __declspec(dllimport)
ERRDLL char accountid[20];
ERRDLL char vmuserid[20];
ERRDLL char vmpassport[128];
ERRDLL char vmpassportdes[256];
ERRDLL char *userid;
ERRDLL char *password;
ERRDLL char passwordes[1024];

int main(int argc, char * argv[])
{
	InitErr();
	wlog("CloudTerm\\cthandler.log",true,"%s:开始\n",argv[0]);
	userid=getenv("USERID");
	password=getenv("PASSWORD");
	desenc(password,passwordes,"cac");
	//wlog("CloudTerm\\cthandler.log",true,"userid ,%s,password,%s\n",userid,password);

	string sRecv(argv[1]);
	int loc1;
	((loc1=sRecv.find('\"'))!=string::npos)?sRecv.erase(loc1,1):sRecv;
	//wlog("CloudTerm\\cthandler.log",true,"apprc start\n");
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",argv[1]);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char szAppname[64], szIp[64], szPort[16], szUsername[64], szPassword[256],szapp[1024];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szUsername, sizeof(szUsername));
	ZeroMemory(szPassword, sizeof(szPassword));
	int nIndex(-1);
	(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
		wlog("CloudTerm\\cthandler.log",true,"ERROR:rc command error");
	if (nIndex==-1)
	{
		//MessageBox(NULL, "参数不正确",argv[0],MB_OK);
		PrinErr("|402|");
		return -1;
	}
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
		//MessageBox(NULL,"hzw","eee",0);
		//wlog("CloudTerm\\cthandler.log",true,"%s\n",logmsg.c_str());
		exit(-1);
	}
	nIndex++;
	i = 0;
	string rdp_path;
	rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
	string scmd;
	scmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
		+ szPort+" -u "+vmuserid+" -p "+vmpassport+" -e "+"\""+szapp+"\"";
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
		//MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		PrinErr("|401|");
		char szError[256];
		sprintf(szError, "ERROR:CreateProcess failed (%d).", GetLastError());
		wlog("CloudTerm\\cthandler.log",true,"%s",szError);
		return -1;
	}
	else
	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		wlog("CloudTerm\\cthandler.log",true,"%s complete\n",argv[0]);
	}
	return 0;
}
