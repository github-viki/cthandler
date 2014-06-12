#include <iostream>
#include <string>
#include <windows.h>
#include "cloudenc.h"
#include <WinSock.h>
#include "simpleDll.h"
#include <setjmp.h>
using namespace std;
#pragma comment(lib,"cloudenc.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"simpleDll.lib")
using namespace std;
#ifndef errmsg_h
#define errmsg_h
#ifdef ERRDLL
#define ERRDLL extern "C" _declspec(dllimport) 
#else
#define ERRDLL extern "C" _declspec(dllexport) 
#endif
typedef struct errmsg
{
	int errNum;
	char errInfo[56];
}errmsg;
#define ERRNUM 30
//extern errmsg errMsg[ERRNUM];
//���������Ϣ��·��
#define PATH "errmsg.txt"
#define RE_PATH "restart.ini"
ERRDLL void InitErr();
ERRDLL void PrinErr(string);
/*��������ʱ������*/
ERRDLL int message_limit();
/*��ȡ����ִ�еĴ���*/
ERRDLL int read_restart();
/*����*/
ERRDLL int tmp_sender(string ip,string port);
/*��õ�ǰ·��*/
ERRDLL string GetModuleDir();
/*�ַ���ת��*/
ERRDLL void ConvertUtf8ToGBK(string& strUtf8);
//ͬ��������н���
ERRDLL int CommuToVm(string ip,string szGranule,string szCmd);
//int CommuToVm(string ip,string szGranule,string szCmd);
//You can also write like this:
//extern "C" {
//_declspec(dllexport) int Add(int plus1, int plus2);
//};
//�����ӳ���
ERRDLL int CallApp(string);
#endif