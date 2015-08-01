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
    char buffer[200000];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 200000, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

int main( int argc, char* argv[] )
{
   string headerStr = " -H \"Host: weibo.cn\" -H \"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; rv:39.0) Gecko/20100101 Firefox/39.0\" -H \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" -H \"Accept-Language: en-US,en;q=0.5\" --compressed -H \"Cookie: _T_WM=c441fc2c51af767ad88196baaf0b5b50; SUB=_2A254uFUyDeTxGedH7VEQ8SjNyT6IHXVYQ3t6rDV6PUJbrdAKLUbHkW1y0hARN9bEpS63INZuhhaBsLyzBA..; gsid_CTandWM=4uLS3cf01PT4zMVj1rfuY8eHa1Y\" -H \"Connection: keep-alive\"";
 
   //string filterStr = " | pup \"div[id^=M]\"";
   string filterStr = " | pup \"div.c\"";

   std::ofstream outfile;
   outfile.open("tinyfool1000recent.html");
   for ( int i = 1; i< 101; i++ )
   {
      stringstream ss;
      ss << i;
      string str = ss.str();
      std::string cmdline = "curl ";
      cmdline += "\"";
      cmdline += argv[1];
      cmdline += "?page=" + str + "\"" + headerStr + filterStr;
      cout.flush();
      string result = exec( cmdline.c_str() );
      //cout << result;
      outfile << result;
   }
   outfile.close();

   return 0;
}

