#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <string.h>
#include <vector>
#include <iterator>
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

	string headerStr = " -H 'Host: weibo.cn' -H 'User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:31.0) Gecko/20100101 Firefox/31.0' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8' -H 'Accept-Language: en-US,en;q=0.5' -H 'Accept-Encoding: gzip, deflate' -H 'Cookie: _T_WM=6f3fd2b26419adb415239be761de9abb; SUB=_2A254wEvoDeTxGedH7VEQ8SjNyT6IHXVYS1WgrDV6PUJbrdANLUnAkW2S6tPrVhCM9X1GLtU3vsZtkqtv0A..; gsid_CTandWM=4uth6b8919bdu4Mcd6e4m8eHa1Y' -H 'Connection: keep-alive' --compressed";


	string htmlheaderStr = "<!--?xml version=&#34;1.0&#34; encoding=&#34;UTF-8&#34;?--> \
<html xmlns=\"http://www.w3.org/1999/xhtml\"> \
 <head> \
  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> \
  <meta http-equiv=\"Cache-Control\" content=\"no-cache\"> \
  <meta id=\"viewport\" name=\"viewport\" content=\"width=device-width,initial-scale=1.0,minimum-scale=1.0, maximum-scale=2.0\"> \
  <link rel=\"icon\" sizes=\"any\" mask=\"\" href=\"http://h5.sinaimg.cn/upload/2015/05/15/28/WeiboLogoCh.svg\"> \
  <meta name=\"theme-color\" content=\"black\"> \
  <meta name=\"MobileOptimized\" content=\"240\"> \
  <style type=\"text/css\" id=\"internalStyle\"> \
   html,body,p,form,div,table,textarea,input,span,select{font-size:12px;word-wrap:break-word;}body{background:#F8F9F9;color:#000;padding:1px;margin:1px;}table,tr,td{border-width:0px;margin:0px;padding:0px;}form{margin:0px;padding:0px;border:0px;}textarea{border:1px solid #96c1e6}textarea{width:95%;}a,.tl{color:#2a5492;text-decoration:underline;}/*a:link {color:#023298}*/.k{color:#2a5492;text-decoration:underline;}.kt{color:#F00;}.ib{border:1px solid #C1C1C1;}.pm,.pmy{clear:both;background:#ffffff;color:#676566;border:1px solid #b1cee7;padding:3px;margin:2px 1px;overflow:hidden;}.pms{clear:both;background:#c8d9f3;color:#666666;padding:3px;margin:0 1px;overflow:hidden;}.pmst{margin-top: 5px;}.pmsl{clear:both;padding:3px;margin:0 1px;overflow:hidden;}.pmy{background:#DADADA;border:1px solid #F8F8F8;}.t{padding:0px;margin:0px;height:35px;}.b{background:#e3efff;text-align:center;color:#2a5492;clear:both;padding:4px;}.bl{color:#2a5492;}.n{clear:both;background:#436193;color:#FFF;padding:4px; margin: 1px;}.nt{color:#b9e7ff;}.nl{color:#FFF;text-decoration:none;}.nfw{clear:both;border:1px solid #BACDEB;padding:3px;margin:2px 1px;}.s{border-bottom:1px dotted #666666;margin:3px;clear:both;}.tip{clear:both; background:#c8d9f3;color:#676566;border:1px solid #BACDEB;padding:3px;margin:2px 1px;}.tip2{color:#000000;padding:2px 3px;clear:both;}.ps{clear:both;background:#FFF;color:#676566;border:1px solid #BACDEB;padding:3px;margin:2px 1px;}.tm{background:#feffe5;border:1px solid #e6de8d;padding:4px;}.tm a{color:#ba8300;}.tmn{color:#f00}.tk{color:#ffffff}.tc{color:#63676A;}.c{padding:2px 5px;}.c div a img{border:1px solid #C1C1C1;}.ct{color:#9d9d9d;font-style:italic;}.cmt{color:#9d9d9d;}.ctt{color:#000;}.cc{color:#2a5492;}.nk{color:#2a5492;}.por {border: 1px solid #CCCCCC;height:50px;width:50px;}.me{color:#000000;background:#FEDFDF;padding:2px 5px;}.pa{padding:2px 4px;}.nm{margin:10px 5px;padding:2px;}.hm{padding:5px;background:#FFF;color:#63676A;}.u{margin:2px 1px;background:#ffffff;border:1px solid #b1cee7;}.ut{padding:2px 3px;}.cd{text-align:center;}.r{color:#F00;}.g{color:#0F0;}.bn{background: transparent;border: 0 none;text-align: left;padding-left: 0;} \
  </style> \
 </head> \
<body> ";
   //string filterStr = " | pup \"div[id^=M]\"";
   string filterStr = " | pup \"div.c\"";

   std::ifstream friendsListFile;
   friendsListFile.open("friendsList.txt");

   std::string s;
   while( std::getline( friendsListFile, s ) )
   {
      std::vector<std::string> v;
      std::istringstream buf(s);
      for(std::string token; getline(buf, token, ','); )
      {
         v.push_back(token); 
      }
      string addrStr = "weibo.cn/" + v[0];
      string titleStr = v[1];
  
      std::ofstream outfile;
      outfile.open("tinyfool1000recent.html");
      for ( int i = 1; i< 11; i++ )
      {
         stringstream ss;
         ss << i;
         string str = ss.str();
         std::string cmdline = "curl ";
         cmdline += "\"";
         cmdline += addrStr;
         cmdline += "?page=" + str + "\"" + headerStr + filterStr;
         cout.flush();
         string result = exec( cmdline.c_str() );

         //cout << result;
         outfile << result;
      }
      outfile.close();
      std::ifstream infile;
      infile.open("tinyfool1000recent.html");
      std::ofstream resultFile;
      std::string title = titleStr;
      title+="Recent100.html";
      resultFile.open(title.c_str());
      std::string line;
      resultFile << htmlheaderStr;
      while ( std::getline( infile,line ) )
      {
         if ( strstr( line.c_str(), "id=\"M" ) ) 
         {
            line += "<div class=\"s\"></div>";
         }
         resultFile << line << '\n';
      }
      resultFile << "</body></html>";
      infile.close();
      resultFile.close();
   }

   return 0;
}

