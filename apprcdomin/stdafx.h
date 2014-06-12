// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
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
//错误弹出信息
#pragma comment(lib,"errmsg.lib")
//wr_log()
#pragma comment(lib,"simpleDll.lib")
#pragma comment(lib, "WtsApi32.lib")

#pragma comment(lib, "ws2_32.lib")
//加解密
#pragma comment(lib,"cloudenc.lib")
// TODO: 在此处引用程序需要的其他头文件