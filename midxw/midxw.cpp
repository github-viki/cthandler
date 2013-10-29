// midxw.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"
#include "iostream"
#include "string"
using namespace std;
string GetModuleDir() ;
int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=5)
		MessageBox(NULL,"ARGCERROR","",NULL);
	char *ip;
	char *name;
	char *passport;
	char *cmd;
	ip=argv[1];
	name=argv[2];
	passport=argv[3];
	cmd=argv[4];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	string config_path;
	string scmd;
	config_path=GetModuleDir();

	int find = config_path.find_last_of("\\");
	string path = config_path.assign(config_path.begin(),config_path.begin() + find);
	path= path+"\\xw\\xwClient\\openxwclient.exe";
	scmd="\""+path+"\" "+ip+" "+name+" "+passport+" "+cmd;
	MessageBox(NULL,scmd.c_str(),"",0);
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
//		wrlog("CloudTerm\\Crphandler.log",szError,true);
		return -1;
	}
	else
	{
		WaitForSingleObject(pi.hProcess,INFINITE);
		//wrlog("CloudTerm\\Crphandler.log","LC complete",true);
	}
	return 0;
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