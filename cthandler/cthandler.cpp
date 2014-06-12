// cthandler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock.h>
#include "simpleDll.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "ws2_32.lib")
// ����������ѡ��
//ȥ������̨
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc<2)
	{
		wlog("CloudTerm\\cthandler.log",true,"%s:cthandler��������\n",argv[0]);
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:��ʼ\n",argv[0]);
	TCHAR lpchbuf[64]={'\0'};
	DWORD dSize=64;
	DWORD sessId=0;
	ProcessIdToSessionId(GetCurrentProcessId(),&sessId);
	//�ܵ������ֱ�ΪCTLIS+USERNAME+SESSID
	if(!GetUserName(lpchbuf,&dSize))
	{
		wlog("CloudTerm\\cthandler.log",true,"GetUserNameError %d\n",GetLastError());
		return -1;
	}
	//wlog("CloudTerm\\cthandler.log",true,"username %s\n",lpchbuf);
	//���������ܵ�����ctlis
	//ctlis�ٵ���caccthandler.exe
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
				MessageBox(NULL,"�����ȵ�¼ct",argv[0],NULL);
				return -1;
			}
			/*else
			{
				MessageBox(NULL,"���ӹܵ�����\n",argv[0],NULL);
				return -1;
			}*/
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if ( ! WaitNamedPipe(pChPipeName, 20000)) 
		{ 
			wlog("CloudTerm\\cthandler.log",true,
				"���ӹܵ���ʱ.\n"); 
			return -1;
		}

	} 
	   // ��ܵ�д������
	   DWORD WriteNum=0;
	   memset(lpchbuf,0,64);
	   sprintf(lpchbuf,"%s",argv[1]);
	   if (WriteFile(hPipe, lpchbuf, strlen(lpchbuf), 
		   &WriteNum, NULL) == FALSE)
	   {
		   wlog("CloudTerm\\cthandler.log",true,"ERROR write file %d\n",GetLastError());
		   //MessageBox(NULL,"��ܵ���д���ݳ���\n","ERROR",0);
		   return -1;
	   } 
	   CloseHandle(hPipe); // �رչܵ����
	   wlog("CloudTerm\\cthandler.log",true,"%s:����\n",argv[0]);
	   return 0;
}

