// apprc.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"

#define PRI 0 //私有模式
#define LOWUSER 1//low user 用户不足
#define LOWGRUN 2//low grunle 颗粒不足
#define OTHER 3 //other error
//
ERRDLL char accountid[20];
ERRDLL char vmuserid[20];
ERRDLL char vmpassport[128];
ERRDLL char vmpassportdes[256];
ERRDLL char *userid;
ERRDLL char *password;
ERRDLL char passwordes[1024];
//汇报计算颗粒信息
int UploadGranuleInfo(char *appInfo,char *szIp)
{
	SOCKET sock=tmp_sender(szIp,"50000");
	if (sock==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"同虚拟机联系时，创建套接字失败\n");
		return -1;
	}
	int nsize=send(sock,"if=privmlive\n",strlen("if=privmlive\n"),0);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"给虚拟机回报应用信息失败\n");
		//MessageBox(NULL,"网络无响应","apprcdomin",0);
		PrinErr("|404|");
		return -1;
	}
	char recvInfo[16];
	nsize=recv(sock,recvInfo,16,0);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"虚拟机返回OK失败\n");
		PrinErr("|404|");
		return -1;
	}
	nsize=send(sock,appInfo,strlen(appInfo),0);
	wlog("CloudTerm\\cthandler.log",true,"提交给虚拟机的应用信息如下\n%s\n",appInfo);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"给虚拟机回报应用信息失败\n");
		PrinErr("|404|");
		return -1;
	}
	closesocket(sock);
	return 0;
}
//定时器
void MyOnTimer(  UINT uID,
	UINT uMsg,
	DWORD dwUser,
	DWORD dw1,
	DWORD dw2)
{
	
 HWND hwnd=FindWindowEx(NULL,NULL,NULL,"提示");
 ::SendMessage(hwnd,WM_CLOSE,NULL,NULL);

}
/*增加用户的绑定模式，最后的返回是在setting最后加了个状态标识
格式如下|setting=setting=PPT*172.20.250.99*3389*openerCMD *1*vmuserid*vmpwd*rc*0|
1后面的为新加字段。
vmuserid，vmpwd为绑定模式下的用户名，和密码
rc为应用的类型。
0为状态。一个有四个状态。
0:表示按私有方式打开，并在后面加了|vmuserid|vmpassport|
1:表示私有用户不足，提示出错，并按原来的方式打开（只会发生在用户第一次登录）
2:表示计算资源不足，提示出错，并按原来的方式打开（只会发生在第二次）
3:表示其他出错信息，如本身就没有可用的虚拟机*/

int main(int argc, char* argv[])
{

	wlog("CloudTerm\\cthandler.log",true,"%s:开始\n",argv[0]);
	InitErr();
	userid=getenv("USERID");
	password=getenv("PASSWORD");
	char *appid;
	appid=getenv("APPID");
	//userid="1";
	//password="111111";
	//appid="1";
	desenc(password,passwordes,"cac");
	//wlog("CloudTerm\\cthandler.log",true,"userid ,%s,password,%s\n",userid,password);

	string sRecv(argv[1]);
	int loc1;
	((loc1=sRecv.find('\"'))!=string::npos)?sRecv.erase(loc1,1):sRecv;

	
	
	//wlog("CloudTerm\\cthandler.log",true,"apprc start\n");
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",argv[1]);

	char szAppname[64], szIp[64], szPort[16], szUsername[64], szPassword[256]
	,szapp[1024];
	ZeroMemory(szAppname, sizeof(szAppname));
	ZeroMemory(szIp, sizeof(szIp));
	ZeroMemory(szPort, sizeof(szPort));
	ZeroMemory(szUsername, sizeof(szUsername));
	ZeroMemory(szPassword, sizeof(szPassword));
	int nIndex(-1);
	(nIndex = sRecv.find("|setting=")) != string::npos? nIndex += strlen("|setting="):
		wlog("CloudTerm\\cthandler.log",true,"ERROR:pri command error");
	if (nIndex==-1)
	{
		//MessageBox(NULL, "计算资源不足，请联系管理员",argv[0],MB_OK);
		PrinErr("|402|");
		return -1;
	}

	//解析字符串为旧的setting,用oldsetting保存//;
	string oldSetting=sRecv;
	//reverse_iterator rit;
	//openFlag表示打开的方式如上面所说的0，1，2；
	//oldAppType表示旧有的app类型。

	char openFlag[2]={'\0'};
	char oldAppType[8]={'\0'};
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
	szGranule[i]='\0';
	//说明后面的内容为新加的东西。
	//从这儿把字符串截断
	if (sRecv[nIndex]=='*')
	{
		sRecv[nIndex]='\0';
	}
	//
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		vmuserid[i++]=sRecv[nIndex++];
	}
	vmuserid[i]='\0';
	//
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		vmpassport[i++]=sRecv[nIndex++];
	}
	vmpassport[i]='\0';
	//
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		oldAppType[i++]=sRecv[nIndex++];
	}
	oldAppType[i]='\0';
	//
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		openFlag[i++]=sRecv[nIndex++];
	}
	openFlag[i]='\0';

	int retValue;
	int iOpenFlag=atoi(openFlag);
	if (iOpenFlag==0)
	{
		wlog("CloudTerm\\cthandler.log",true,"按私有方式打开\n");
		//先给虚拟机汇报颗粒
		//给vmlive上报应用信息|vmuserid|userid|password|appid|granule|cmd|
		char appInfo[1024]={'\0'};
		sprintf(appInfo,"|%s|%s|%s|%s|%s|%s|\n",vmuserid,userid,password
			,appid,szGranule,szapp);
		int rerValue;
		retValue=UploadGranuleInfo(appInfo,szIp);
		if (retValue==-1)
		{
			wlog("CloudTerm\\cthandler.log",true,"给虚拟机汇报颗粒信息失败,继续打开应用\n");
		}
		if (strncmp(oldAppType,"rc",2)==0)
		{
			string sCmd;
			string rdp_path;
			rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
			sCmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
				+ szPort+" -u "+vmuserid+" -p "+vmpassport+" -e "+"\""+szapp+"\"";
			wlog("CloudTerm\\cthandler.log",true,"调用参数为%s\n",sCmd.c_str());
			retValue=CallApp(sCmd);
		}

		else if (strncmp(oldAppType,"rd",2)==0)
		{
			string sCmd;
			string rdp_path;
			rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
			sCmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
				+ szPort+" -u "+vmuserid+" -p "+vmpassport;
			wlog("CloudTerm\\cthandler.log",true,"调用参数为%s\n",sCmd.c_str());
			retValue=CallApp(sCmd);
		}
		else
		{
			wlog("CloudTerm\\cthandler.log",true,"旧的类型为%s,私有应用只应该属于rc"
				"或者是rd类型的\n",oldAppType);
			PrinErr("|402|");
			return -1;
		}
		//给vmlive上报应用信息|vmuserid|userid|password|appid|granule|cmd|
		if(retValue==-1)
		{
			wlog("CloudTerm\\cthandler.log",true,"私有调用rc,rd失败\n");
			return -1;
		}
		

	}
	else if(iOpenFlag==1||iOpenFlag==2)
	{
		if(iOpenFlag==1)
		{
			wlog("CloudTerm\\cthandler.log",true
				,"打开私有应用时，由于私有账户不足，采用公有方式打开\n");
		}
		if (iOpenFlag==2)
		{
			wlog("CloudTerm\\cthandler.log",true
				,"打开私有应用时，由于所选虚拟机颗粒不足，将选择另外一台虚拟机按公有方式打开\n");
		}
		SetTimer(NULL,1,1000,(TIMERPROC)MyOnTimer);//1秒种后，将自动关闭提示
		MessageBox(NULL,"资源不足，将按公有方式打开","提示",0);
		

		if (strncmp(oldAppType,"rc",2)==0)
		{
			string sCmd;
			sCmd="apprc.exe \""+sRecv+"|\"";
			wlog("CloudTerm\\cthandler.log",true,"旧类型为apprc,调用apprc的"
				"命令为%s\n",sCmd.c_str());
			CallApp(sCmd);
		}
		else if (strncmp(oldAppType,"rd",2)==0)
		{
			string sCmd;
			sCmd="apprd.exe \""+sRecv+"|\"";
			wlog("CloudTerm\\cthandler.log",true,"旧类型为apprd,调用apprd的"
				"命令为%s\n",sCmd.c_str());
			CallApp(sCmd);
		}
		else
		{
			wlog("CloudTerm\\cthandler.log",true,"旧的类型为%s,私有应用只应该属于rc"
				"或者是rd类型的\n",oldAppType);
			//MessageBox(NULL,"私有应用类型不正确","错误",0);
			PrinErr("|402|");
		}
	}
	else
	{
		wlog("CloudTerm\\cthandler.log",true,"最后的状态为%s\n"
			,openFlag);
		//MessageBox(NULL,"私有应用类型其他错误","错误",0);
		PrinErr("|402|");
		return -1;
	}
	return 0;
}
