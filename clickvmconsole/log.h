#include "stdio.h"
#include <time.h>
#include <io.h>
#include <ctime>
void wrlog(char *filename,const char *p,bool writetime){
	FILE *fp;
	char cmd[200];
	SYSTEMTIME nowTime;                            
	GetLocalTime(&nowTime);	
	char szTime[128];
	ZeroMemory(szTime, sizeof(szTime));
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.wYear, nowTime.wMonth, nowTime.wDay, 
		nowTime.wHour, nowTime.wMinute, nowTime.wSecond);


	if( (_access( "C:\\Users\\Public", 0 )) == 0 )
	{
		if( (_access( "C:\\Users\\Public\\CloudTerm", 0 )) == -1 )
		{
			mkdir("C:\\Users\\Public\\CloudTerm");
		}
		sprintf(cmd,"C:\\Users\\Public\\%s",filename);
	}
	else
	{
		if( (_access( "C:\\Documents and Settings\\All Users\\CloudTerm", 0 )) == -1 )
		{
			mkdir("C:\\Documents and Settings\\All Users\\CloudTerm");
		}
		sprintf(cmd,"C:\\Documents and Settings\\All Users\\%s",filename);	
	}



	fp=fopen(cmd,"a+");
	if(fp==NULL) return ;
	if(writetime==true)
	{fprintf(fp,"%s %s\n",szTime,p);}
	else
	{fprintf(fp,"%s\n",p);}
	fclose(fp);
}