// apprc.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include "stdafx.h"

#define PRI 0 //˽��ģʽ
#define LOWUSER 1//low user �û�����
#define LOWGRUN 2//low grunle ��������
#define OTHER 3 //other error
//
ERRDLL char accountid[20];
ERRDLL char vmuserid[20];
ERRDLL char vmpassport[128];
ERRDLL char vmpassportdes[256];
ERRDLL char *userid;
ERRDLL char *password;
ERRDLL char passwordes[1024];
//�㱨���������Ϣ
int UploadGranuleInfo(char *appInfo,char *szIp)
{
	SOCKET sock=tmp_sender(szIp,"50000");
	if (sock==-1)
	{
		wlog("CloudTerm\\cthandler.log",true,"ͬ�������ϵʱ�������׽���ʧ��\n");
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
		PrinErr("|404|");
		return -1;
	}
	nsize=send(sock,appInfo,strlen(appInfo),0);
	wlog("CloudTerm\\cthandler.log",true,"�ύ���������Ӧ����Ϣ����\n%s\n",appInfo);
	if(nsize<=0)
	{
		wlog("CloudTerm\\cthandler.log",true,"��������ر�Ӧ����Ϣʧ��\n");
		PrinErr("|404|");
		return -1;
	}
	closesocket(sock);
	return 0;
}
//��ʱ��
void MyOnTimer(  UINT uID,
	UINT uMsg,
	DWORD dwUser,
	DWORD dw1,
	DWORD dw2)
{
	
 HWND hwnd=FindWindowEx(NULL,NULL,NULL,"��ʾ");
 ::SendMessage(hwnd,WM_CLOSE,NULL,NULL);

}
/*�����û��İ�ģʽ�����ķ�������setting�����˸�״̬��ʶ
��ʽ����|setting=setting=PPT*172.20.250.99*3389*openerCMD *1*vmuserid*vmpwd*rc*0|
1�����Ϊ�¼��ֶΡ�
vmuserid��vmpwdΪ��ģʽ�µ��û�����������
rcΪӦ�õ����͡�
0Ϊ״̬��һ�����ĸ�״̬��
0:��ʾ��˽�з�ʽ�򿪣����ں������|vmuserid|vmpassport|
1:��ʾ˽���û����㣬��ʾ��������ԭ���ķ�ʽ�򿪣�ֻ�ᷢ�����û���һ�ε�¼��
2:��ʾ������Դ���㣬��ʾ��������ԭ���ķ�ʽ�򿪣�ֻ�ᷢ���ڵڶ��Σ�
3:��ʾ����������Ϣ���籾���û�п��õ������*/

int main(int argc, char* argv[])
{

	wlog("CloudTerm\\cthandler.log",true,"%s:��ʼ\n",argv[0]);
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
		//MessageBox(NULL, "������Դ���㣬����ϵ����Ա",argv[0],MB_OK);
		PrinErr("|402|");
		return -1;
	}

	//�����ַ���Ϊ�ɵ�setting,��oldsetting����//;
	string oldSetting=sRecv;
	//reverse_iterator rit;
	//openFlag��ʾ�򿪵ķ�ʽ��������˵��0��1��2��
	//oldAppType��ʾ���е�app���͡�

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
	//˵�����������Ϊ�¼ӵĶ�����
	//��������ַ����ض�
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
		wlog("CloudTerm\\cthandler.log",true,"��˽�з�ʽ��\n");
		//�ȸ�������㱨����
		//��vmlive�ϱ�Ӧ����Ϣ|vmuserid|userid|password|appid|granule|cmd|
		char appInfo[1024]={'\0'};
		sprintf(appInfo,"|%s|%s|%s|%s|%s|%s|\n",vmuserid,userid,password
			,appid,szGranule,szapp);
		int rerValue;
		retValue=UploadGranuleInfo(appInfo,szIp);
		if (retValue==-1)
		{
			wlog("CloudTerm\\cthandler.log",true,"��������㱨������Ϣʧ��,������Ӧ��\n");
		}
		if (strncmp(oldAppType,"rc",2)==0)
		{
			string sCmd;
			string rdp_path;
			rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
			sCmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
				+ szPort+" -u "+vmuserid+" -p "+vmpassport+" -e "+"\""+szapp+"\"";
			wlog("CloudTerm\\cthandler.log",true,"���ò���Ϊ%s\n",sCmd.c_str());
			retValue=CallApp(sCmd);
		}

		else if (strncmp(oldAppType,"rd",2)==0)
		{
			string sCmd;
			string rdp_path;
			rdp_path = GetModuleDir() + "\\if\\cacrd_old.exe";
			sCmd = rdp_path+" -t "+ "\""+ sAppname + "\"" + " -s "+ szIp + " -d "
				+ szPort+" -u "+vmuserid+" -p "+vmpassport;
			wlog("CloudTerm\\cthandler.log",true,"���ò���Ϊ%s\n",sCmd.c_str());
			retValue=CallApp(sCmd);
		}
		else
		{
			wlog("CloudTerm\\cthandler.log",true,"�ɵ�����Ϊ%s,˽��Ӧ��ֻӦ������rc"
				"������rd���͵�\n",oldAppType);
			PrinErr("|402|");
			return -1;
		}
		//��vmlive�ϱ�Ӧ����Ϣ|vmuserid|userid|password|appid|granule|cmd|
		if(retValue==-1)
		{
			wlog("CloudTerm\\cthandler.log",true,"˽�е���rc,rdʧ��\n");
			return -1;
		}
		

	}
	else if(iOpenFlag==1||iOpenFlag==2)
	{
		if(iOpenFlag==1)
		{
			wlog("CloudTerm\\cthandler.log",true
				,"��˽��Ӧ��ʱ������˽���˻����㣬���ù��з�ʽ��\n");
		}
		if (iOpenFlag==2)
		{
			wlog("CloudTerm\\cthandler.log",true
				,"��˽��Ӧ��ʱ��������ѡ������������㣬��ѡ������һ̨����������з�ʽ��\n");
		}
		SetTimer(NULL,1,1000,(TIMERPROC)MyOnTimer);//1���ֺ󣬽��Զ��ر���ʾ
		MessageBox(NULL,"��Դ���㣬�������з�ʽ��","��ʾ",0);
		

		if (strncmp(oldAppType,"rc",2)==0)
		{
			string sCmd;
			sCmd="apprc.exe \""+sRecv+"|\"";
			wlog("CloudTerm\\cthandler.log",true,"������Ϊapprc,����apprc��"
				"����Ϊ%s\n",sCmd.c_str());
			CallApp(sCmd);
		}
		else if (strncmp(oldAppType,"rd",2)==0)
		{
			string sCmd;
			sCmd="apprd.exe \""+sRecv+"|\"";
			wlog("CloudTerm\\cthandler.log",true,"������Ϊapprd,����apprd��"
				"����Ϊ%s\n",sCmd.c_str());
			CallApp(sCmd);
		}
		else
		{
			wlog("CloudTerm\\cthandler.log",true,"�ɵ�����Ϊ%s,˽��Ӧ��ֻӦ������rc"
				"������rd���͵�\n",oldAppType);
			//MessageBox(NULL,"˽��Ӧ�����Ͳ���ȷ","����",0);
			PrinErr("|402|");
		}
	}
	else
	{
		wlog("CloudTerm\\cthandler.log",true,"����״̬Ϊ%s\n"
			,openFlag);
		//MessageBox(NULL,"˽��Ӧ��������������","����",0);
		PrinErr("|402|");
		return -1;
	}
	return 0;
}
