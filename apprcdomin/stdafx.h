// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <Wtsapi32.h>
#include <algorithm>
#include <direct.h>
#include <iostream>
#include <string>
#include <io.h>
#include <windows.h>
#include <ctime>
#include <WinSock.h>
#include "cloudenc.h"
#include "simpleDll.h"
#define ERRDLL
#include "errmsg.h"

using namespace std;

#include <stdlib.h>
//���󵯳���Ϣ
#pragma comment(lib,"errmsg.lib")
//wr_log()
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "WtsApi32.lib")

#pragma comment(lib, "ws2_32.lib")
//�ӽ���
#pragma comment(lib,"cloudenc.lib")
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�