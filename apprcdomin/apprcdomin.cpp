// apprc.cpp : �������̨Ӧ�ó������ڵ㡣
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
apprcdomin �������ɷ���������������û�����������ͣ�
��û����CommuTovm();
����Ҫ�����������ͨ�š�
*/
//��������㱨������Ϣ
int UploadGranuleInfo(char *appInfo,char *szIp)
{
	SOCKET sock=tmp_sender(szIp,"50000");
	if (sock==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"ͬ�������ϵʱ�������׽���ʧ��\n");
		PrinErr("|404|");
		return -1;
	}
	int nsize=send(sock,"if=privmlive\n",strlen("if=privmlive\n"),0);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"��������ر�Ӧ����Ϣʧ��\n");
		//MessageBox(NULL,"��������Ӧ","apprcdomin",0);
		PrinErr("|404|");
		return -1;
	}
	char recvInfo[16];
	nsize=recv(sock,recvInfo,16,0);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"���������OKʧ��\n");
		//MessageBox(NULL,"��������Ӧ","apprcdomin",0);
		PrinErr("|404|");
		return -1;
	}
	nsize=send(sock,appInfo,strlen(appInfo),0);
	wlog("CloudTerm\\cthandler.log",true,"�ύ���������Ӧ����Ϣ����\n%s\n",appInfo);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"��������ر�Ӧ����Ϣʧ��\n");
		//MessageBox(NULL,"��������Ӧ","apprcdomin",0);
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
	,szapp[1024],szDomainName[128];//����
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
		//MessageBox(NULL, "������Դ���㣬����ϵ����Ա",argv[0],MB_OK);
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
	//��������㱨������Ϣ
	char appInfo[1024]={'\0'};
	sprintf(appInfo,"|%s|%s|%s|%s|%s|%s|\n",vmuserid,userid,passwordes
		,appid,szGranule,szapp);
	UploadGranuleInfo(appInfo,szIp);
	//�Ȼ㱨��Ϣ���Ϳ��Կ����������ˡ�
	string rdp_path;
	rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
	string scmd;
	scmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
		+ szPort+" -u "+vmuserid+"@"+szDomainName+" -p "+vmpassport
		+" -e "+"\""+szapp+"\"";
	string logmsg("�������򼰴��������");
	logmsg+=scmd;
	wlog("CloudTerm\\cthandler.log",true,"%s\n",logmsg.c_str());
	int retValue=CallApp(scmd);
	if (retValue==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"apprcdomin,��cacrd_old.exeʧ��\n");
		return -1;
	}

	return 0;
}
