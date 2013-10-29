#include "split.h"
#include <tchar.h> 
#include <shlobj.h>
#include <Shlwapi.h>
#include <comdef.h>
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib") 
string logfile = "log.txt";

BOOL  CreateDirectoryNested(LPCTSTR  lpszDir)
{
	if(PathIsDirectory( lpszDir )) return TRUE;

	TCHAR   szPreDir[ MAX_PATH ];
	_tcscpy_s( szPreDir, lpszDir );
	//确保路径末尾没有反斜杠
	//ModifyPathSpec( szPreDir, FALSE );

	//获取上级目录
	BOOL  bGetPreDir  = PathRemoveFileSpec( szPreDir );
	if( !bGetPreDir ) return FALSE;

	//如果上级目录不存在,则递归创建上级目录
	if( !PathIsDirectory( szPreDir ) )
	{
		CreateDirectoryNested( szPreDir );
	}

	return CreateDirectory( lpszDir, NULL );
}

string GetSysTime()
{
	SYSTEMTIME nowTime;                            
	GetLocalTime(&nowTime);	
	char szTime[128];
	sprintf(szTime, "%d/%d/%d %d:%d:%d",nowTime.wMonth,nowTime.wDay, 
	nowTime.wYear, nowTime.wHour, nowTime.wMinute, nowTime.wSecond);
	string sTime(szTime);
	return sTime;
}

int wr_Log(string log)
{
	 char basedir[256];
// 	 DWORD dwRet=GetModuleFileName(NULL,basedir,100);
//    
// 	if(dwRet==0)
// 	{
//       //printf("GetCurrentDirectory failed (%d)\n", GetLastError());
//       return GetLastError();
//     }
	SHGetSpecialFolderPath(NULL, basedir, CSIDL_PERSONAL, FALSE);
	strcat(basedir, "\\");
	strcat(basedir, "clouddesk_log");
	CreateDirectoryNested(basedir);
    log += " -- " + GetSysTime() + "\n";
	string bsd=string(basedir,strlen(basedir));
// 	int temp=bsd.find_last_of("\\");
// 	string base_str=string(bsd,0,temp);
	
	logfile = bsd +"\\"+logfile;
	ofstream log_f;
	log_f.open(logfile.c_str(), ios::app);	
	log_f<<log.c_str()<<endl;
	log_f.close();
	log_f.clear();
	return 0;
}

stringsplitor::stringsplitor()
{
    m_bIsSplited = false;   
}

 

stringsplitor::stringsplitor(const char* strSrc)

{
    m_str = string(strSrc);
}

 

stringsplitor::~stringsplitor()
{

}

void stringsplitor::split(const char csp)
{
    // split the m_data into array, store into global vector m_v
    unsigned int    iSize = m_str.length();
    char c;
    string buf;
    for (unsigned int i=0; i<iSize; i++)
    {
        c = m_str[i];
        if (c==csp)
        {
           m_v.push_back(buf);
           buf = "";
           continue;
        }
        buf += c;
    }
    if (c!=csp)
	  m_v.push_back(buf);
    m_bIsSplited = true;
}

 
void stringsplitor::clear()
{
    m_str = "";
    m_bIsSplited = false;
    m_v.clear();
}

 

// inlet, load

void stringsplitor::load(const char* pChar)
{
    clear();
	int     nLen = ::MultiByteToWideChar (CP_UTF8, 0, pChar, -1, NULL, 0) ;
    WCHAR   * pWstr = new WCHAR[nLen+1] ;
    ZeroMemory (pWstr, sizeof(WCHAR) * (nLen+1)) ;
    ::MultiByteToWideChar (CP_UTF8, 0, pChar, -1, pWstr, nLen) ;
	std::string  strAnsi (_bstr_t((wchar_t*)pWstr)) ;
    delete[] pWstr ;
    m_str = string(strAnsi);

}

 
void stringsplitor::load(const std::string &str)
{
    clear();
    m_str = str;
}

 

void stringsplitor::operator =(const char *pChar)
{
    load(pChar);
}

 

void stringsplitor::operator <<(const char * pChar)
{

    load(pChar);

}

 

void stringsplitor::operator =(const string& str)
{

    load(str);

}

 

void stringsplitor::operator <<(const string& str)
{

    load(str);

}

 

// outlet, get certain item

string stringsplitor::operator[](unsigned int iIndex) const
{

    if (iIndex>=m_v.size() || !m_bIsSplited)
    {
        return string("");
    }
    return m_v.at(iIndex);
}

 
string stringsplitor::at(unsigned int iIndex) const
{
    return (*this)[iIndex];
}
