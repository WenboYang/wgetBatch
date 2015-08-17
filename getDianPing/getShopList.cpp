// Hello World example
// This example shows basic usage of DOM-style API.

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
    string areaStr = argv[1];

    if ( argc < 2 )
    {
       cout << "usage: getFriends tokenstring";
       return 0;
       
    }

    ofstream lutFs;
    stringstream namess;
    namess << "samples/lookUpTable.txt";
    lutFs.open( namess.str().c_str() );
    for ( int i = 1; i < 6; i++ )
    {
       stringstream ss;
       ss << "curl \"" << areaStr << "p" << i << "\" | pup a.\"o-map\" json{}";
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
          stringstream idss;
          stringstream namess;
          idss <<  a[i]["data-shopid"].GetString();
          namess <<  a[i]["data-sname"].GetString();
          cout << "now getting" << idss.str() << namess.str() << "\n";
          stringstream cmdss;
          cmdss << "./getComments " << idss.str();
          cout << cmdss.str() << "\n";
          sleep(3);
          //exec( cmdss.str().c_str() );
          lutFs << namess.str() << ":" << idss.str() << "\n";
       }
    }
    lutFs.close();
    return 0;
}
