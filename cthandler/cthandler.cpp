// cthandler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock.h>
#include "simpleDll.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "ws2_32.lib")
// 设置连接器选项
//去掉控制台
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc<2)
	{
		wlog("CloudTerm\\cthandler.log",true,"%s:cthandler参数错误\n",argv[0]);
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:开始\n",argv[0]);
	TCHAR lpchbuf[64]={'\0'};
	DWORD dSize=64;
	DWORD sessId=0;
	ProcessIdToSessionId(GetCurrentProcessId(),&sessId);
	//管道的名字变为CTLIS+USERNAME+SESSID
	if(!GetUserName(lpchbuf,&dSize))
	{
		wlog("CloudTerm\\cthandler.log",true,"GetUserNameError %d\n",GetLastError());
		return -1;
	}
	//wlog("CloudTerm\\cthandler.log",true,"username %s\n",lpchbuf);
	//利用命名管道连接ctlis
	//ctlis再调用caccthandler.exe
	TCHAR pChPipeName[128]={'\0'};
	sprintf(pChPipeName,"\\\\.\\pipe\\CTLIS%s%d",lpchbuf,sessId);
	wlog("CloudTerm\\cthandler.log",true,"pchpipename %s\n",pChPipeName);
	HANDLE hPipe;
	while (1) 
	{ 
		hPipe = CreateFile( 
			pChPipeName,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE) 
			break; 

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY) 
		{
			DWORD errCode=GetLastError();
			wlog("CloudTerm\\cthandler.log",true,
				TEXT("Could not open pipe.GLE= %d\n"),errCode); 
			if(errCode==ERROR_FILE_NOT_FOUND)
			{
				MessageBox(NULL,"请首先登录ct",argv[0],NULL);
				return -1;
			}
			/*else
			{
				MessageBox(NULL,"连接管道出错\n",argv[0],NULL);
				return -1;
			}*/
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if ( ! WaitNamedPipe(pChPipeName, 20000)) 
		{ 
			wlog("CloudTerm\\cthandler.log",true,
				"连接管道超时.\n"); 
			return -1;
		}

	} 
	   // 向管道写入数据
	   DWORD WriteNum=0;
	   memset(lpchbuf,0,64);
	   sprintf(lpchbuf,"%s",argv[1]);
	   if (WriteFile(hPipe, lpchbuf, strlen(lpchbuf), 
		   &WriteNum, NULL) == FALSE)
	   {
		   wlog("CloudTerm\\cthandler.log",true,"ERROR write file %d\n",GetLastError());
		   //MessageBox(NULL,"向管道中写数据出错\n","ERROR",0);
		   return -1;
	   } 
	   CloseHandle(hPipe); // 关闭管道句柄
	   wlog("CloudTerm\\cthandler.log",true,"%s:结束\n",argv[0]);
	   return 0;
}

