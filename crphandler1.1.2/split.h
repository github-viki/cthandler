#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

 //char c_port[100],c_ip[100],c_termid[10];

 int wr_Log(string log);


 class stringsplitor

{

private:

    string              m_str;

    bool                m_bIsSplited;

    std::vector<string> m_v;    

 

public:

    // constructors

    stringsplitor();    

    stringsplitor(const char* strSrc);

    

    ~stringsplitor();

 

    // operator, action

    void    split(const char csp='|');  // splitor

    void    clear();                    // clear all data

 

    // inlet

    // load

    void            load(const char*);

    void            load(const string &);

    void            operator=(const char*);

    void            operator=(const string&);

    void            operator<<(const char*);

    void            operator<<(const string&);

 
    // outlet

    // [] overload

    string  operator[](unsigned int) const;

    string  at(unsigned int )const;

 
};
