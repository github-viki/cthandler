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
//定义错误信息的路径
#define PATH "errmsg.txt"
#define RE_PATH "restart.ini"
ERRDLL void InitErr();
ERRDLL void PrinErr(string);
/*弹出错误时间限制*/
ERRDLL int message_limit();
/*读取重新执行的次数*/
ERRDLL int read_restart();
/*连接*/
ERRDLL int tmp_sender(string ip,string port);
/*获得当前路径*/
ERRDLL string GetModuleDir();
/*字符串转换*/
ERRDLL void ConvertUtf8ToGBK(string& strUtf8);
//同虚拟机进行交互
ERRDLL int CommuToVm(string ip,string szGranule,string szCmd);
//int CommuToVm(string ip,string szGranule,string szCmd);
//You can also write like this:
//extern "C" {
//_declspec(dllexport) int Add(int plus1, int plus2);
//};
//调用子程序
ERRDLL int CallApp(string);
#endif