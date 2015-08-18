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
            const string& userIdString,
            ofstream& outFs )
{
   string filterStr = "\" | pup div.\"frame-inner\" img attr{src}";
   std::string headStr = "curl -s \"www.dianping.com" ;

   stringstream ss;
   ss << headStr << url << filterStr;
   string strBuf;
   //cout << ss.str();
   //open the picture page, get picture link
   strBuf = exec( ss.str().c_str() );
   strBuf.erase( strBuf.size() - 1, 1 );
   //now download the picture and rename it
   //cout << strBuf;
   ss.str("");
   ss << "curl -s -o " << "samples/" << shopIdString << "_" << userIdString << ".jpg" << " \""<< strBuf << "\"";

   outFs << "   pic_link : ";
   outFs << "samples/" << shopIdString << "_" << userIdString << ".jpg\n";
   exec( ss.str().c_str() );
}

int main( int argc, char* argv[] ) {

    // Prepare reader and input stream.
    Reader reader;
    string shopIdStr = argv[1];

    if ( argc < 3 )
    {
       cout << "usage: shopIdString, resultfile";
       return 0;
       
    }
    ofstream outFs;
    outFs.open( argv[2], std::ios_base::app );

    string filterStr = "\" | pup ul.\"comment-list\" json{}";
    std::string headStr = "curl -s \"www.dianping.com/shop/" ;
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
       //std::cout << ss.str();
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
          //std::cout << i << "\n";
          userIdSs <<  a[i]["children"][0]["children"][0]["alt"].GetString();

          outFs << "\n{\n";
          outFs << "   shopId   : ";
          outFs << shopIdStr << ",\n";
          outFs << "   user_id  : ";
          outFs << userIdSs.str() << ",\n";

          int commentPos = 2;
          string isVipTagExistStr = a[i]["children"][1]["class"].GetString();
          if ( "user-info" != isVipTagExistStr )
          {
             commentPos++;
          }
          //std::cout << ">>>>>" << commentPos << "\n";
          string shortCommentOnlystr;
          int dtTagPos = 3;

          //move to the comment content branch of this tree
          const Value& c = a[i]["children"][commentPos];

          shortCommentOnlystr = c["children"][1]["tag"].GetString();

          if ( shortCommentOnlystr != "p" )
          {
             commentSs << c["children"][2]["children"][0]["text"].GetString() << "\n";
          }
          else
          {
             commentSs << c["children"][1]["text"].GetString() << "\n";
             dtTagPos = 2;

          }

          outFs << "   comments : ";
          outFs << commentSs.str();
          outFs << ",\n";
          //the position of the first picture depends on if the customer add favorite dish or not
          string dtTagStr = c["children"][dtTagPos]["children"][0]["tag"].GetString();
          stringstream picLinkSs;
          string imgTagStr = "";
          if ( dtTagStr == "dt" )
          {
             dtTagPos++;
          }

          //move to the comment content branch of this tree
          const Value& r = c["children"];
          if ( r.Size() >  dtTagPos )
          {
             //if we want to extract all pics, need to count the array size 
             std::cout << "\nTotally" << r.Size() << "pics\n";
             //get img tag
             imgTagStr = r[dtTagPos]["children"][0]["children"][0]["children"][0]["tag"].GetString();
          }
          if ( "img" == imgTagStr )
          {
             picLinkSs <<  r[dtTagPos]["children"][0]["children"][0]["href"].GetString();
          }
          else
          {
             //no picture, we won't put it into record
             continue;
          }
          //cout << picLinkSs.str();
          getPic( picLinkSs.str(), shopIdStr, userIdSs.str(), outFs );
          outFs << "\n},\n";
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

    outFs.close();
    return 0;
}
