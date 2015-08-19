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
    cout << ss.str().c_str();
    for ( int i = 1; i < 6; i++ )
    {
       stringstream ss;
       ss << "curl -s \"" << firstPageLinkStr << "p" << i << "\" | pup a.\"o-map\" json{}";
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
                << "},";
          lutFs.flush();
          exec( cmdSs.str().c_str() );

          cout << "Getting " << idSs.str() << " " << nameSs.str() << "...\n";
          cmdSs.str("");
          cmdSs << "./getJapanComments " << idSs.str() << " " << nameStr;
          cout << cmdSs.str() << "\n";
          sleep(3);
          exec( cmdSs.str().c_str() );
       }
    }
    lutFs.close();
    return 0;
}
