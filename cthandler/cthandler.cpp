// cthandler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock.h>
#include "simpleDll.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // ����������ѡ��
//ȥ������̨
int InitSocket(string ip,string port)
{
	WSADATA wsa; 
	//��ʼ���׽���DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"socket initialize failed.\n"); 
		return -1; 
	} 
	//�����׽��� 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"create socket failed.\n"); 
		return -1; 
	} 
	struct sockaddr_in serverAddress; 
	memset(&serverAddress,0,sizeof(sockaddr_in)); 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = inet_addr(ip.c_str()); 
	serverAddress.sin_port = htons(atoi(port.c_str())); 
	//�����ͷ����������� 
	if(connect(sock,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR)
	{ 
		MessageBox(NULL, "��������Ӧ�����������Ƿ�ͨ��","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"connect failed.\n"); 
		return -1; 
	} 
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf); 
	return sock;
}
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc<2)
	{
		wlog("CloudTerm\\cthandler.log",true,"%s:start arvc wrong\n",argv[0]);
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
	TCHAR lpchbuf[64]={'\0'};
	DWORD dSize=64;
	if(!GetUserName(lpchbuf,&dSize))
	{
		wlog("CloudTerm\\cthandler.log",true,"GetUserNameError %d\n",GetLastError());
		return -1;
	}
	//wlog("CloudTerm\\cthandler.log",true,"username %s\n",lpchbuf);
	TCHAR pChPipeName[128]={'\0'};
	sprintf(pChPipeName,"\\\\.\\pipe\\CTLIS%s",lpchbuf);
	wlog("CloudTerm\\cthandler.log",true,"pchpipename %s\n",pChPipeName);

label: if (WaitNamedPipe(pChPipeName, NMPWAIT_WAIT_FOREVER) == FALSE)
	   {
		   wlog("CloudTerm\\cthandler.log",true,"ERROR:waitnamedpipe %d\n",GetLastError());
		   
		   if(GetLastError()==ERROR_PIPE_BUSY)
		   {
			   goto label;
		   }
		   else
		   
		   {
			   if(GetLastError()==ERROR_FILE_NOT_FOUND)MessageBox(NULL,"�����ȵ�¼ct",argv[0],NULL);
			   return -1;
		   }
	   }
	   
	   HANDLE hPipe = CreateFile(pChPipeName, GENERIC_READ | GENERIC_WRITE,0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	   if (hPipe == INVALID_HANDLE_VALUE)
	   {

		   wlog("CloudTerm\\cthandler.log",true,"ERROR��createfile %d\n",GetLastError());
		   if(GetLastError()==ERROR_FILE_NOT_FOUND)
		   MessageBox(NULL,"�����ȵ�¼ct",argv[0],NULL);
		   return -1;
	   }
		
	// ��ܵ�д������
	DWORD WriteNum=0;
	memset(lpchbuf,0,64);
	sprintf(lpchbuf,"%s",argv[1]);
	if (WriteFile(hPipe, lpchbuf, strlen(lpchbuf), 
		&WriteNum, NULL) == FALSE)
	{
		wlog("CloudTerm\\cthandler.log",true,"ERROR write file %d\n",GetLastError());
		return -1;
	} else {
		//wlog("CloudTerm\\cthandler.log",true,"write %s\n",lpchbuf);

	}
	CloseHandle(hPipe); // �رչܵ����
	wlog("CloudTerm\\cthandler.log",true,"%s:complete\n",argv[0]);
	return 0;
	
}

