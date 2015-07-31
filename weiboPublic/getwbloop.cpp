#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <cstring>
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
using namespace std;

std::string exec( const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[1000];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 1000, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

int main( int argc, char* argv[] )
{
   std::ifstream infile;
   infile.open("addlist.txt");
   string weiboDotCnHostStr = " -H \"Host: weibo.cn\"";
   string sinaImgDotCnHostStr = " -H \"Host: ww2.sinaimg.cn\"";
   string pupSelectorForSinaImagDotCnHost = "| pup \"div.nm [href^=\"http\"] attr{href}\"";
   string headerStr = " -H \"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:39.0) Gecko/20100101 Firefox/39.0\" -H \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" -H \"Accept-Language: en-US,en;q=0.5\" --compressed -H \"Referer: http://weibo.cn/1651330523\" -H \"Cookie: _T_WM=1723bd66136652106e71cf96a2dba994; SUB=_2A254vmX3DeTxGedH7VEQ8SjNyT6IHXVYQQu_rDV6PUJbrdANLWjekW2FQ28-A6K-fXFK9Dv0IYSq2Ivpcg..; gsid_CTandWM=4ujs2a881NcvDGPTG25wb8eHa1Y\" -H \"Connection: keep-alive\"";
   string getRspStr = " -o /dev/null -w \"%{http_code}\"";



   std::string line;
   while ( std::getline( infile,line ) )
   {
      std::string cmdline = "curl ";
      cmdline += "\"" + line + "\"" + headerStr + getRspStr;
      cout << cmdline << "\n\n\n";
      cout.flush();
      string result = exec( cmdline.c_str() );
      if ( !strcmp( result.c_str(), "302" ) )
      {
         cout << "\n>>dowloadable\n";
         cmdline.clear();
         cmdline += "curl \"" + line + "\"" +  weiboDotCnHostStr + headerStr + " -L -O";
         cout << '\n' << cmdline;
         exec( cmdline.c_str() );

      }
      else if ( !strcmp( result.c_str(), "200" ) )
      {
         cout << "\n>>need confirm!";
         cmdline.clear();
         cmdline += "curl \"" + line + "\"" +  weiboDotCnHostStr + headerStr + pupSelectorForSinaImagDotCnHost;
         string addrStr = exec( cmdline.c_str() );
         cout << "\n>>real addr!";
         cout << addrStr;
         //somehow there is a trailing \n
         addrStr.erase( addrStr.find_last_not_of(" \n\r\t")+1);
         //now download the big pictures...
         cmdline.clear();
         cmdline += "curl \"" + addrStr + "\"" +  sinaImgDotCnHostStr + headerStr + " -O";
         cout << cmdline;
         cout.flush();
         exec( cmdline.c_str() );
      }
   }
   infile.close();
   return 0;
}

