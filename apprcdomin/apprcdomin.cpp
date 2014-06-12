// apprc.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
ERRDLL char accountid[];
ERRDLL char vmuserid[];
ERRDLL char vmpassport[];
ERRDLL char vmpassportdes[];
ERRDLL char *userid;
ERRDLL char *password;
ERRDLL char passwordes[1024];
/*
apprcdomin 类型是由服务器返回虚拟机用户和密码的类型，
而没有了CommuTovm();
不需要和虚拟机进行通信。
*/
//给虚拟机汇报颗粒信息
int UploadGranuleInfo(char *appInfo,char *szIp)
{
	SOCKET sock=tmp_sender(szIp,"50000");
	if (sock==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"同虚拟机联系时，创建套接字失败\n");
		PrinErr("|404|");
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
		//MessageBox(NULL,"网络无响应","apprcdomin",0);
		PrinErr("|404|");
		return -1;
	}
	nsize=send(sock,appInfo,strlen(appInfo),0);
	wlog("CloudTerm\\cthandler.log",true,"提交给虚拟机的应用信息如下\n%s\n",appInfo);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"给虚拟机回报应用信息失败\n");
		//MessageBox(NULL,"网络无响应","apprcdomin",0);
		PrinErr("|404|");
		return -1;
	}
	closesocket(sock);
	return 0;

}
int main(int argc,char* argv[])
{
	wlog("CloudTerm\\cthandler.log",true,"%s:start\n",argv[0]);
	userid=getenv("USERID");
	password=getenv("PASSWORD");
	char *appid;
	appid=getenv("APPID");
	desenc(password,passwordes,"cac");
	//wlog("CloudTerm\\cthandler.log",true,"userid ,%s,password,%s\n",userid,password);

	string sRecv(argv[1]);
	int loc1;
	((loc1=sRecv.find('\"'))!=string::npos)?sRecv.erase(loc1,1):sRecv;
	//wlog("CloudTerm\\cthandler.log",true,"apprc start\n");
	//wlog("CloudTerm\\cthandler.log",true,"%s\n",argv[1]);


	char szAppname[64], szIp[64], szPort[16], szUsername[64], szPassword[256]
	,szapp[1024],szDomainName[128];//域名
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
		//MessageBox(NULL, "计算资源不足，请联系管理员",argv[0],MB_OK);
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
	nIndex++;
	i = 0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		vmuserid[i++]=sRecv[nIndex++];
	}
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		vmpassportdes[i++]=sRecv[nIndex++];
	}
	desdec(vmpassportdes,vmpassport,"test");
	nIndex++;
	i=0;
	while (sRecv[nIndex]!='|'&&sRecv[nIndex]!='*'&&sRecv[nIndex]!='\0')
	{
		szDomainName[i++]=sRecv[nIndex++];
	}
	szDomainName[i]='\0';
	nIndex++;
	i=0;
	//给虚拟机汇报颗粒信息
	char appInfo[1024]={'\0'};
	sprintf(appInfo,"|%s|%s|%s|%s|%s|%s|\n",vmuserid,userid,passwordes
		,appid,szGranule,szapp);
	UploadGranuleInfo(appInfo,szIp);
	//先汇报信息，就可以开机挂网盘了。
	string rdp_path;
	rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
	string scmd;
	scmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
		+ szPort+" -u "+vmuserid+"@"+szDomainName+" -p "+vmpassport
		+" -e "+"\""+szapp+"\"";
	string logmsg("启动程序及传入参数：");
	logmsg+=scmd;
	wlog("CloudTerm\\cthandler.log",true,"%s\n",logmsg.c_str());
	int retValue=CallApp(scmd);
	if (retValue==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"apprcdomin,打开cacrd_old.exe失败\n");
		return -1;
	}

	return 0;
}
