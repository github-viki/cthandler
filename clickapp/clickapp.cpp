// clickapp.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <setjmp.h>

/*����ִ�еĴ���*/
//int re_count=0;
/*��ת�Ļ���*/
jmp_buf jmp_env;

string ccip;
char userid[128];
char appid[128];

//������������ص���Ϣ
int handler(char *cRecv)
{
	static int re_count=read_restart();
	wlog("CloudTerm\\cthandler.log",true,"read_restart %d\n",re_count);
	string arg=cRecv;
	ConvertUtf8ToGBK(arg);
	int locin;
	while((locin=(arg.find("\"")))!=string::npos)
	{
		arg.erase(locin,1);
	}
	if(arg.find("infotype")==string::npos)
	{
		wlog("CloudTerm\\cthandler.log",true,"���յ�����Ϣ:%s\n",arg.c_str());
		if(arg.find("001")!=string::npos)
			wlog("CloudTerm\\cthandler.log",true,"have 001\n");
		if (re_count>0)
		{
			wlog("CloudTerm\\cthandler.log",true,"have more than zero\n");
		}
		if( re_count > 0 && arg.find("001") != string::npos )
		{
			wlog("CloudTerm\\cthandler.log",true,"hehe\n");
			int res_mes=message_limit();
			if(res_mes == -1)
				exit(-1);
			re_count--;
			longjmp(jmp_env,re_count);
		}
		PrinErr(arg);
		return -1;
	}
	int loc=arg.find("=");
	arg.assign(arg.begin()+loc+1,arg.end());
	loc=arg.find("|");
	string scmd="";
	scmd.assign(arg.begin(),arg.begin()+loc);
	arg.assign(arg.begin()+loc,arg.end());
	scmd="\"app"+scmd+".exe\" "+"\""+arg+"\""+" "+userid;
	//���������� ��һ������Ϊ�յ�����Ϣ���ڶ�������Ϊuserid
	//����Ϊapprx.exe |setting=name*172.20.251.113*3389*opener C:\WINDOWS\system32\mspaint.exe*1| userid
	wlog("CloudTerm\\cthandler.log",true,"%s\n",scmd.c_str());
	//����apprd.exe apprc.exe argҲҪ�����ţ���Ϊarg���п����пո񣬶����ִ���	
	CallApp(scmd);
	return 0;
}

int main(int argc, char* argv[])
{
	InitErr();

	//int re_count=read_restart();
	if(argc!=2)
	{
		PrinErr("|401|");
		return -1;
	}
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
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
	ccip=getenv("CCIP");
	/*�Ӵ˴���ʼ����ִ��*/
	int jmpres=setjmp(jmp_env);
	if(jmpres>0)
	{
		wlog("CloudTerm\\cthandler.log",true,"���¿�ʼִ��\n");
	}
	string arg(argv[1]);
	string logmsg="���ղ�����";
	logmsg+=argv[1];
	wlog("CloudTerm\\cthandler.log",true,"%s:%s %s\n",argv[0]
	,logmsg.c_str(),ccip.c_str());
	//������ͷ�cc
	int loc;
	if ((loc = arg.find("|appid")) != string::npos)
	{
		string SendInfo = arg;
		SOCKET Sock = tmp_sender(ccip, "50000");
		if(Sock==-1)
		{
			//int re_count=read_restart();
			//re_count--;
			int re_count=read_restart();
			while (re_count>0)
			{
				closesocket(Sock);
				int res=message_limit();
				if (res == -1)
				{
					wlog("CloudTerm\\cthandler.log",true,"�û������ȡ��\n");
					exit(-1);
				}
				Sock=tmp_sender(ccip,"50000");
				if (Sock!=-1)
				{
					break;
				}
				re_count--;
			}
		}
		if (Sock == -1)
		{
			PrinErr("|404|");
			return -1;
		}
		if(send(Sock,"if=clickapp\n",strlen("if=clickapp\n"),0)==SOCKET_ERROR)
		{ 
			//MessageBox(NULL,"clickapp������Ϣʧ��\n","ERROR",0);
			PrinErr("|404|");
			wlog("CloudTerm\\cthandler.log",true,"vmconsole������Ϣʧ�ܣ�"
				"if=clickapp WGLE=%d\n",WSAGetLastError()); 
			return -1; 
		} 
		int bytes; 
		char cRecv[1024];
		memset(cRecv,0,1024);
		if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
		{ 
			//MessageBox(NULL, "clickapp������Ϣʧ��","",MB_OK);
			PrinErr("|404|");
			wlog("CloudTerm\\cthandler.log",true
				,"recive data failed.WGLE=%d\n",WSAGetLastError()); 
			return -1; 
		}
		if (!strncmp(cRecv, "ok", 2))
		{
			//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 
			if(send(Sock,SendInfo.c_str(),strlen(SendInfo.c_str()),0)==SOCKET_ERROR)
			{ 
				//MessageBox(NULL,"clickapp������Ϣʧ��\n","ERROR",0);
				PrinErr("|404|");
				wlog("CloudTerm\\cthandler.log",true,"clickapp������Ϣʧ�ܣ�"
					"appid=userid= WGLE=%d\n",WSAGetLastError());  
				//GetLastError();
				return -1; 
			} 
			if((bytes = recv(Sock, cRecv, 1024, 0)) == SOCKET_ERROR)
			{ 
				//MessageBox(NULL, "clickapp������Ϣʧ��","",MB_OK);
				PrinErr("|404|");
				wlog("CloudTerm\\cthandler.log",true
					,"recive data failed.WGLE=%d\n",WSAGetLastError()); 
				return -1; 
			}
		}
		closesocket(Sock);
		
		handler(cRecv);
	}
	wlog("CloudTerm\\cthandler.log",true,"clickapp����\n");
	return 0;
}



