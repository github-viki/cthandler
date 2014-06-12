// testDLL.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "errmsg.h"
#include <windows.h>
#include "simpleDll.h"
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib,"errmsg.lib")
/*读取re_start 配置文件 ,返回重新打开的次数*/
int read_restart()
{
	int re_count=1;
	FILE *fp;
	fp=fopen("restart.ini","r");
	if (fp == NULL)
	{
		wlog("CloudTerm\\cthandler.log",true,"读取重新打开配置文件失败\n");
		return -1;
	}
	char line[128]={0};

	while (fgets(line,128,fp))
	{
		if (line[0] == '#')
		{
			continue;
		}
		if(strstr(line,"re_count="))
		{
			char *i=strchr(line,'=');
			re_count=atoi(i+1);
		}
	}
	fclose(fp);
	return re_count;
}
/*弹出等待提示*/
int message_limit()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if( !CreateProcess(NULL,   // No module name (use command line)
		const_cast <char *>("re_start.exe"),        // Command line
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
		//PrinErr("|401|");
		char szError[256];
		sprintf(szError, "ERROR:CreateProcess failed (%d).\n", GetLastError());
		//wlog("CloudTerm\\cthandler.log",true,szError);
		return -1;
	}
	else{
		//wlog("CloudTerm\\cthandler.log",true,"clickapp结束\n");
		WaitForSingleObject(pi.hProcess,INFINITE);
		DWORD res=0;
		GetExitCodeProcess(pi.hProcess,&res);
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return res;
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	InitErr();
	PrinErr("|401|");
	int re_count=read_restart();
	while (re_count > 0)
	{
		int i=message_limit();
		wlog("CloudTerm\\cthandler.log",true,"%d\n",i);
		re_count--;
	}
	return 0;
}

