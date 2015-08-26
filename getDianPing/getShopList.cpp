/***************************************************************************//**
* @file 
*     This file implements a command line version dianping shop list extraction
*     routine.
*
* Input 
*     A dianping search result URL for certain areas, such as:
*        
*        http://www.dianping.com/search/category/1/10/r801
*
*     A task name, such as Lujiazui.
*  
* Output:
*     Extract shop id, name, gps info from first 5 pages, and it will also call
*     the getComments routine to extract comment and pictures for each shop.
*
* @note
*     Only works for restaurants, not for hotels.
*
* @warning 
*     None.
*******************************************************************************
*/

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <cstdio>
#include "rapidjson/reader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace rapidjson;
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

string cookieStr = " -H \"Host: www.dianping.com\" -H \"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; rv:40.0) Gecko/20100101 Firefox/40.0\" -H \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" -H \"Accept-Language: en-US,en;q=0.5\" --compressed -H \"Referer: http://www.dianping.com/search/category/1/10/r808p2\" -H \"Cookie: showNav=#nav-tab|0|0; navCtgScroll=112; _hc.v=\"\"\\\"\"1d9f9bd4-09b0-4298-b8f3-2f1747eefebe.1439726970\\\"\"\"\"; cy=1; cye=shanghai; PHOENIX_ID=0a03034f-14f6a161742-1ce44b; JSESSIONID=DC1F3EE945D7875FA0FD113A494D0D30; aburl=1; s_ViewType=10\" -H \"Connection: keep-alive\" -H \"Cache-Control: max-age=0\"";

int main( int argc, char* argv[] ) {

    // Prepare reader and input stream.
    Reader reader;
    string firstPageLinkStr;
    string nameStr;


    if ( argc < 3 )
    {
       std::cout << "usage: \"link of first page\" \"area name\"";
       return 0;
    }

    firstPageLinkStr = argv[1];
    nameStr = argv[2];

    stringstream mkdirSs;
    mkdirSs << "mkdir -p " << nameStr << "/sample";
    exec( mkdirSs.str().c_str() );

    ofstream lutFs;
    stringstream ss;
    ss << nameStr << "/" << nameStr << "Index.txt";
    lutFs.open( ss.str().c_str() );
    lutFs << "[\n";

    ofstream outFs;
    ss.str("");
    ss << nameStr << "/" << nameStr << ".txt";
    outFs.open( ss.str().c_str(), std::ios_base::app );
    outFs << "[\n";
    cout << ss.str().c_str();
    outFs.close();

    for ( int j = 1; j < 6; j++ )
    {
       stringstream ss;
       ss << "curl -s \"" << firstPageLinkStr << "p" << j << "\"" << cookieStr.c_str() << " | pup a.\"o-map\" json{}";
       Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
       string strBuf;
       strBuf = exec( ss.str().c_str() );
       /*
       if ( strBuf.size() > 2 )
       {
          strBuf = strBuf.substr( 1, strBuf.size() - 2);
       }
       */

       cout << strBuf;

#if 1
       // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
       if (document.Parse(strBuf.c_str()).HasParseError())
          return 1;

#else
       // In-situ parsing, decode strings directly in the source string. Source must be string.
       char buffer[sizeof(json)];
       memcpy(buffer, json, sizeof(json));
       if (document.ParseInsitu(buffer).HasParseError())
          return 1;
#endif
       ////////////////////////////////////////////////////////////////////////////
       // 2. Access values in document. 
       assert(document.IsArray());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.
       const Value& a = document;
       assert(a.IsArray());
       cout << "parsed" << a.Size() << "shops";
       for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
       {
          stringstream idSs;
          stringstream nameSs;
          stringstream addrSs; 
          stringstream cmdSs;

          idSs   << a[i]["data-shopid"].GetString();
          nameSs << a[i]["data-sname"].GetString();
          addrSs << a[i]["data-address"].GetString();

          cmdSs << "python decode.py " << a[i]["data-poi"].GetString();
          string gpsStr = exec( cmdSs.str().c_str() );

          lutFs << "\n{\n" 
                << "   \"id\"   : " << "\"" << idSs.str()   << "\"" << ",\n"
                << "   \"name\" : " << "\"" << nameSs.str() << "\"" << ",\n"
                << "   \"addr\" : " << "\"" << addrSs.str() << "\"" << ",\n"
                << "   \"gps\"  : " << gpsStr
                << "}";
          exec( cmdSs.str().c_str() );

          cout << "Getting " << idSs.str() << " " << nameSs.str() << "...\n";
          cmdSs.str("");
          if ( j == 5 && (i+1) == a.Size() )
          {
             cmdSs << "./getJapanComments " << idSs.str() << " " << nameStr << " " << " tail ";
          }
          else
          {
             cmdSs << "./getJapanComments " << idSs.str() << " " << nameStr << " " << "content";
             lutFs << ",";
             lutFs.flush();
          }
          cout << cmdSs.str() << "\n";
          sleep(3);
          exec( cmdSs.str().c_str() );
       }
    }
    lutFs << "\n]";
    lutFs.close();

    ss.str("");
    ss << nameStr << "/" << nameStr << ".txt";
    outFs.open( ss.str().c_str(), std::ios_base::app );
    outFs << "\n]";
    outFs.close();

    stringstream cmdSs;
    cmdSs << "mogrify -format jpg -gravity Center -crop 75x75%+0+0 ";
    cmdSs << nameStr << "/";
    cmdSs << "sample/*.jpg";
    exec( cmdSs.str().c_str() );
    return 0;
}
