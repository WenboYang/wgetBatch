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


void getPic( const string& url,
            const string& shopIdString,
            const string& userIdString )
{
   string filterStr = "\" | pup div.\"frame-inner\" img attr{src}";
   std::string headStr = "curl \"www.dianping.com" ;

   stringstream ss;
   ss << headStr << url << filterStr;
   string strBuf;
   cout << ss.str();
   //open the picture page, get picture link
   strBuf = exec( ss.str().c_str() );
   strBuf.erase( strBuf.size() - 1, 1 );
   //now download the picture and rename it
   //cout << strBuf;
   ss.str("");
   ss << "curl -o " << "samples/" << shopIdString << "_" << userIdString << ".jpg" << " \""<< strBuf << "\"";
   cout << ss.str();
   exec( ss.str().c_str() );
}

int main( int argc, char* argv[] ) {

    // Prepare reader and input stream.
    Reader reader;
    string shopIdStr = argv[1];

    if ( argc < 2 )
    {
       cout << "usage: getFriends tokenstring";
       return 0;
       
    }


    string filterStr = "\" | pup ul.\"comment-list\" json{}";
    std::string headStr = "curl \"www.dianping.com/shop/" ;
    headStr += shopIdStr;
    headStr += filterStr;

    std::ofstream resultFile;
    stringstream rss;
    rss << "samples/" << shopIdStr << ".txt";
    resultFile.open( rss.str().c_str() );

    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    string strBuf;
    {
       stringstream ss;
       ss << headStr;
       std::cout << ss.str();
       //fetch web page from server
       strBuf = exec( ss.str().c_str() );
       if ( strBuf.size() > 2 )
       {
          strBuf = strBuf.substr( 1, strBuf.size() - 2);
       }

       //std::cout << strBuf;
       resultFile << strBuf;
       resultFile.close();
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
       assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.
       const Value& a = document["children"];
       assert(a.IsArray());
       for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
       {
          stringstream userIdSs;
          stringstream commentSs;
          std::cout << i << "\n";
          userIdSs <<  a[i]["children"][0]["children"][0]["alt"].GetString();
          std::cout << userIdSs.str() << "\n";

          string shortCommentOnlystr;
          shortCommentOnlystr =a[i]["children"][2]["children"][1]["tag"].GetString();

          int dtTagPos = 3;
          if ( shortCommentOnlystr != "p" )
          {
             //std::cout <<  a[i]["children"][2]["children"][1]["children"][0]["text"].GetString() << "\n";
             commentSs << a[i]["children"][2]["children"][2]["children"][0]["text"].GetString() << "\n";
          }
          else
          {
             commentSs <<  a[i]["children"][2]["children"][1]["text"].GetString() << "\n";
             dtTagPos = 2;

          }
          //the position of the first picture depends on if the customer add favorite dish or not
          string dtTagStr = a[i]["children"][2]["children"][dtTagPos]["children"][0]["tag"].GetString();
          stringstream picLinkSs;
          string imgTagStr = "";
          if ( dtTagStr == "dt" )
          {
             dtTagPos++;
          }

          if ( a[i]["children"][2]["children"].Size() >  dtTagPos )
          {
             //if we want to extract all pics, need to count the array size 
             std::cout << "<<<<<<" << a[i]["children"][2]["children"].Size();
             imgTagStr = a[i]["children"][2]["children"][dtTagPos]["children"][0]["children"][0]["children"][0]["tag"].GetString();
          }
          if ( "img" == imgTagStr )
          {
             picLinkSs <<  a[i]["children"][2]["children"][dtTagPos]["children"][0]["children"][0]["href"].GetString();
          }
          else
          {
             //no picture, we won't put it into record
             continue;
          }
          cout << picLinkSs.str();
          getPic( picLinkSs.str(), shopIdStr, userIdSs.str() );
       }

       /*
       if ( !(a.Size()) )
       {
          break;
       }
       */
       //cout << nextCursor << '\n';
       //cout << totalNumber << '\n';
       //cout.flush();
    }
    return 0;
}