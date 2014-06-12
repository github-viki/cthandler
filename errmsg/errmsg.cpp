// errmsg.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "errmsg.h"
errmsg errMsg[ERRNUM]={0};
/*同虚拟机交互需要用到的信息*/

 ERRDLL char accountid[20];
 ERRDLL char vmuserid[20];
 ERRDLL char vmpassport[128];
 ERRDLL char vmpassportdes[256];
 ERRDLL char *userid;
 ERRDLL char *password;
 ERRDLL char passwordes[1024];
 //
 char accountid[20];
 char vmuserid[20];
 char vmpassport[128];
 char vmpassportdes[256];
 char *userid;
 char *password;
 char passwordes[1024];

void InitErr()
{
	FILE *fp;
	fp=fopen(PATH,"r");
	if(fp==NULL)
	{
		MessageBox(NULL,"载入配置文件出错","error",0);
		exit(-1);
	}
	char tmp[128]={0};
	int i=0;
	while(fgets(tmp,128,fp)&&i<ERRNUM)
	{
		if (tmp[0]=='#')
		{
			continue;
		}
		sscanf_s(tmp,"%d %s\n",&(errMsg[i].errNum),errMsg[i].errInfo);
		i++;
		memset(tmp,0,128);
	}
	fclose(fp);
}
void PrinErr(string arg)
{
	char tmp[10]={'\0'};
	if (arg[0]!='|')
	{
		MessageBox(NULL,"404:网络连接不畅，请稍后再试","ERROR",0);
		wlog("CloudTerm\\cthandler.log",true,"其他错误:%s\n",arg.c_str());
		return ;
	}
	int i=1;
	int  k=0;
	while (arg[i]!='|')
	{
		tmp[k++]=arg[i++];
	}
	int num=atoi(tmp);
	//wlog("CloudTerm\\cthandler.log",true,"%s:%d\n",tmp,num);
	i=0;
	while (errMsg[i].errNum!=0)
	{
		if (errMsg[i].errNum==num)
		{
			char detailInfo[200]={'\0'};
			sprintf(detailInfo,"%s:%s",tmp,errMsg[i].errInfo);
			MessageBox(NULL,detailInfo,"ERROR",0);
			wlog("CloudTerm\\cthandler.log",true,"%s\n",arg.c_str());
			return ;
		}
		i++;
	}
	MessageBox(NULL,"404:网络连接不通，请稍后再试","ERROR",0);
	wlog("CloudTerm\\cthandler.log",true,"未知类型错误%s\n",arg.c_str());
	return ;

}

/*读取re_start 配置文件 ,返回重新打开的次数*/
int read_restart()
{
	int re_count=0;
	FILE *fp;
	fp=fopen(RE_PATH,"r");
	if (fp == NULL)
	{
		wlog("CloudTerm\\cthandler.log",true,"读取re_start配置文件失败\n");
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
/*连接*/
int tmp_sender(string ip,string port)
{
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		//MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:socket initialize failed.\n"); 
		return -1; 
	} 
	//创建套接字 
	SOCKET sock; 
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		//MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:create socket failed.\n"); 
		return -1; 
	} 

	////使用非阻塞连接 3s即返回。
	//unsigned long ul=1;
	//ioctlsocket(sock,FIONBIO,&ul);
	struct sockaddr_in serverAddress; 
	memset(&serverAddress,0,sizeof(sockaddr_in)); 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = inet_addr(ip.c_str()); 
	serverAddress.sin_port = htons(atoi(port.c_str())); 
	//建立和服务器的连接 
	if(connect(sock,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR)
	{ 
		//MessageBox(NULL, "网络无响应，请检查网络是否畅通？","",MB_OK);
		wlog("CloudTerm\\cthandler.log",true,"ERROR:connect failed.\n"); 
		return -1; 
	}
	return sock;
	//printf("Message from %s: %s\n",inet_ntoa(serverAddress.sin_addr),buf);
	//fd_set fset;
	//FD_ZERO(&fset);
	//FD_SET(sock,&fset);
	//struct timeval timeout;
	//timeout.tv_sec=3;
	//timeout.tv_usec=0;
	//int ret=0;
	//ret = select(0,0,&fset,0,&timeout);
	//if (ret<0)
	//{
	//	closesocket(sock);
	//	return -1;
	//}
	//else
	//	closesocket(sock);
}
/*获得当前路径*/
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
/*字符串转换*/
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
//同虚拟机进行交互
jmp_buf jmpenv;
int CommuToVm(string ip,string szGranule,string szCmd)
{
	setjmp(jmpenv);
	SOCKET sockToVm=tmp_sender(ip,"50000");
	if(sockToVm==-1)
	{

		/*重新执行的次数*/
		int re_count=read_restart();
		while (re_count>0)
		{
			closesocket(sockToVm);
			int res=message_limit();
			if (res == -1)
			{
				wlog("CloudTerm\\cthandler.log",true,"用户点击了取消\n");
				exit(-1);
			}
			sockToVm=tmp_sender(ip,"50000");
			if (sockToVm!=-1)
			{
				break;
			}
			re_count--;
		}
		if(sockToVm < 0)
		{
			PrinErr("|404|");
			wlog("CloudTerm\\cthandler.log",true,"连接虚拟机失败\n");
			return -1;
		}
	}
	char SendInfo[256]={'\0'};
	int bytes;
	char *appid;
	appid=getenv("APPID");
	//SendInfo="cdc:|"+userid+"|"+passport+"|\n";
	if(send(sockToVm,"if=vmuser\n",strlen("if=vmuser\n"),0)==SOCKET_ERROR)
	{ 
		PrinErr("|404|");
		wlog("CloudTerm\\cthandler.log",true,"ERROR:给虚拟机发送信息失败\n"); 
		return -1; 
	} 
	char tmp[10];
	if((bytes = recv(sockToVm, tmp, 10, 0)) == SOCKET_ERROR)
	{ 
		PrinErr("|404|");
		wlog("CloudTerm\\cthandler.log",true,"ERROR:从虚拟机接受信息失败"); 
		return -1; 
	}
	if (strncmp(tmp,"ok",2)!=0)
	{

		wlog("CloudTerm\\cthandler.log",true,"ERROR:ok?"); 
		return -1; 
	}
	//cdc:|userid|passworddes|appid|granule|
	sprintf(SendInfo,"cdc:|%s|%s|%s|%s|%s|\n",userid,passwordes,appid,szGranule.c_str(),szCmd.c_str());
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",SendInfo);
	if(send(sockToVm,SendInfo,strlen(SendInfo),0)==SOCKET_ERROR)
	{ 
		PrinErr("|404|");
		wlog("CloudTerm\\cthandler.log",true,"ERROR:给虚拟机发送信息失败\n");  
		return -1; 
	} 
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",tmp);

	char cRecv[1024];

	if((bytes = recv(sockToVm, accountid, 1024, 0)) == SOCKET_ERROR)
	{ 
		PrinErr("|404|");
		wlog("CloudTerm\\cthandler.log",true,"ERROR:从虚拟机接受信息失败"); 
		return -1; 
	}
	accountid[bytes]='\0';
	wlog("CloudTerm\\cthandler.log",true,"%s\n",accountid);
	closesocket(sockToVm);
	string arg(accountid);
	//如果有success字符串就说明是成功的
	if(arg.find("success")==string::npos)
	{
		static int re_count=read_restart();
		if (arg.find("|202|")||arg.find("|203|")&&re_count>0)
		{
			int res_lim=message_limit();
			if (res_lim == -1)
			{
				exit(-1);
			}
			re_count--;
			longjmp(jmpenv,1);
		}
		PrinErr(arg);
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
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",vmpassport);
	//wrlog("CloudTerm\\cthandler.log",SendInfo.c_str(),true); 

	//wlog("CloudTerm\\cthandler.log",true,"%s\n",accountid); 
	return 0;
}
/*调用子程序*/
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
		PrinErr("|401|");
		char szError[256];
		sprintf(szError, "ERROR:CreateProcess failed (%d).\n", GetLastError());
		wlog("CloudTerm\\cthandler.log",true,szError);
		return -1;
	}
	else{
		
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return 0;
	}
}