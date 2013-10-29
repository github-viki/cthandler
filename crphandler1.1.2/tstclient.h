#include <iostream>
#include <windows.h>
#define DllExport   __declspec( dllexport )
using namespace std;
extern "C" 
{
DllExport SOCKET SendandRecv(LPSTR lpIp, unsigned short port, LPSTR lpSendmsg, LPSTR lpRecv, bool bActiveClose);
DllExport SOCKET SendandRecvOnce(LPSTR lpIp, unsigned short port, LPSTR lpSendmsg, LPSTR lpRecv, bool bActiveClose);
DllExport SOCKET ContinueShakeHands(SOCKET ConnectSocket, LPSTR lpSendmsg, LPSTR lpRecv, bool bActiveClose=false);
}