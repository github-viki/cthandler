// appxw.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <io.h>
#include <direct.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "pcre.h" //正则表达式所需头文件

using namespace std; 

#define BUFF_SIZE 1024

_TCHAR vwip[20];
_TCHAR cmd[BUFF_SIZE];
_TCHAR userid[BUFF_SIZE];
_TCHAR passport[BUFF_SIZE];

void log(_TCHAR *filename,_TCHAR *p,bool writetime){
	FILE *fp;
	_TCHAR cmd[200];
	SYSTEMTIME nowTime;                            
	GetLocalTime(&nowTime);	
	_TCHAR szTime[128];
	ZeroMemory(szTime, sizeof(szTime));
	_stprintf(szTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), nowTime.wYear, nowTime.wMonth, nowTime.wDay, 
	nowTime.wHour, nowTime.wMinute, nowTime.wSecond);

	if( (_access( "C:\\Users\\Public", 0 )) == 0 )
	{
		if( (_access( "C:\\Users\\Public\\CtAppRegister", 0 )) == -1 )
		{
			mkdir("C:\\Users\\Public\\CtAppRegister");
		}
		_stprintf(cmd,_T("C:\\Users\\Public\\%s"),filename);
	}
	else
	{
	   if( (_access( "C:\\Documents and Settings\\All Users\\CtAppRegister", 0 )) == -1 )
		{
			mkdir("C:\\Documents and Settings\\All Users\\CtAppRegister");
		}
	   _stprintf(cmd,_T("C:\\Documents and Settings\\All Users\\%s"),filename);	
	}

	fp=_tfopen(cmd,_T("a+"));
	if(fp==NULL) return ;
	if(writetime==true)
	{fprintf(fp,"%s %s\n",szTime,p);}
	else
	{fprintf(fp,"%s\n",p);}
	fclose(fp);
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

int cutmesg(_TCHAR *str)
{
	_TCHAR *start;
	_TCHAR *end;
	_TCHAR buff[BUFF_SIZE]={0};

	_tcscpy(buff,str);
	if ((start=_tcsstr(buff,"*"))==NULL || (end=_tcsstr(start+1,"*"))==NULL)
	{
		return 0;
	}else
	{
		*end=0;
	}
	_tcscpy(vwip,start+1);

	memset(buff,BUFF_SIZE,0);

	_tcscpy(buff,str);
	if ((start=_tcsstr(buff,"*"))==NULL || (start=_tcsstr(start+1,"*"))==NULL ||(end=_tcsstr(start,"|"))==NULL)
	{
		return 0;
	}else
	{
		*end=0;
	}
	_tcscpy(cmd,start+1);
	
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{

	_TCHAR Log[BUFF_SIZE]={0};
	_TCHAR temp[BUFF_SIZE]={0};

	if (argc != 4)
	{
		_stprintf(Log,_T("appxw error:参数%s错误"),argv);	
		log(_T("appxw\\appxw.log"),Log,TRUE);
		return 0; 
	}
	//运用正则表达式解析字符数串
	/*const char * pErrMsg = NULL;
	int nOffset = -1; 
	pcre * CmdPattern=NULL;
	CmdPattern=pcre_compile("|.\*.|" , 0 , &pErrMsg , &nOffset,NULL);
	int result=0;

	result=pcre_exec(CmdPattern,NULL,"fdsfa|b*a|",5,0,0,NULL,0);*/
	

	_tcscpy(temp,argv[1]);
	_tcscpy(userid,argv[2]);
	_tcscpy(passport,argv[3]);

	if (!cutmesg(temp))
	{
		log(_T("appxw\\appxw.log"),"cutmessage fialed!",TRUE);
		return 0; 
	}

	_TCHAR szCommndLine[BUFF_SIZE]={0};             //给被调用进程传参数
	string config_path = GetModuleDir();

	int find = config_path.find_last_of("\\");
	string path = config_path.assign(config_path.begin(),config_path.begin() + find);
	path= path + "\\xw\\openxwclient.exe";

	_stprintf(szCommndLine,_T("%s %s %s %s %s"),path.c_str(),vwip,userid,passport,cmd);
	_tprintf(_T("%s\n"),szCommndLine);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW; // 指定wShowWindow成员有效
	si.wShowWindow = TRUE; // 此成员设为TRUE的话则显示新建进程的主窗口
	BOOL bRet = CreateProcess (
		NULL,// 不在此指定可执行文件的文件名
		szCommndLine,//命令行参数
		NULL,// 默认进程安全性
		NULL,// 默认进程安全性
		FALSE,// 指定当前进程内句柄不可以被子进程继承
		NULL,// 为新进程创建一个新的控制台窗口
		NULL,// 使用本进程的环境变量
		NULL,// 使用本进程的驱动器和目录
		&si,
		&pi) ;

	return 1;
}

