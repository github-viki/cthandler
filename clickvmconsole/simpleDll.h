#pragma once;  

//该宏完成在dll项目内部使用__declspec(dllexport)导出  
//在dll项目外部使用时，用__declspec(dllimport)导入  
//宏DLL_IMPLEMENT在simpledll.cpp中定义  
#ifdef DLL_IMPLEMENT  
#define DLL_API __declspec(dllexport)  
#else  
#define DLL_API __declspec(dllimport)  
#endif 
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <windows.h>
#include <stdarg.h>
extern   "C"__declspec(dllexport) int wlog(const char *path,bool writetime,const char *forMat,...);