/***************************************************************************//**
* @file 
*     This file implements a command line version dianping shop comments 
*     extraction routine, based on curl, pup and rapidjson. 
*
* Input 
*     Shop ID and name of the task, something like Lujiazui, which will be 
*     used to locate the folder to save the extracted comments and pictures.
*  
* Output:
*     For now, only check the first page of a ceratin shop, and only grab 1 
*     picture with 1 piece of comment from each user.  
*  
*     The result will be appended into the record file, in JSON format, like
*     this:
*
*     {
*       "shopId"   : "564313",
*       "user_id"  : "Chestnut_Baby",
*       "comments" : "由于之前比较喜欢黑方，这次麻麻过生日又定了黑方...
*       "pic_link" : "samples/564313_Chestnut_Baby.jpg"
*     }
*
*     Each "pic_link"  points to a picture saved in the /sample subfoler, 
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


string pathStr;
string samplePathStr;

string cookieStr = " -H \"Host: www.dianping.com\" -H \"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; rv:40.0) Gecko/20100101 Firefox/40.0\" -H \"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\" -H \"Accept-Language: en-US,en;q=0.5\" --compressed -H \"Referer: http://www.dianping.com/search/category/1/10/r808p2\" -H \"Cookie: showNav=#nav-tab|0|0; navCtgScroll=112; _hc.v=\"\"\\\"\"1d9f9bd4-09b0-4298-b8f3-2f1747eefebe.1439726970\\\"\"\"\"; cy=1; cye=shanghai; PHOENIX_ID=0a03034f-14f6a161742-1ce44b; JSESSIONID=DC1F3EE945D7875FA0FD113A494D0D30; aburl=1; s_ViewType=10\" -H \"Connection: keep-alive\" -H \"Cache-Control: max-age=0\"";

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

   ss << "curl -s -o \"" 
      << samplePathStr
      << shopIdString 
      << "_" 
      << userIdString 
      << ".jpg\"" 
      << " \""
      << strBuf 
      << "\"";

   cout << ss.str().c_str();

   outFs << "   \"pic_link\" : ";
   outFs << "\"sample/" << shopIdString << "_" << userIdString << ".jpg\"\n";
   sleep(1);
   exec( ss.str().c_str() );
}


void ExtractCommentAndPic( const Value& a, string& shopIdStr, ofstream& outFs, bool isEndOfTable )
{
   for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
   {
      stringstream userIdSs;
      stringstream commentSs;
      std::cout << i << "\n";
      userIdSs <<  a[i]["children"][0]["children"][0]["alt"].GetString();
      cout << userIdSs.str().c_str();
      cout.flush();


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
         commentSs << c["children"][2]["children"][0]["text"].GetString();
      }
      else
      {
         commentSs << c["children"][1]["text"].GetString();
         dtTagPos = 2;
      }

      cout << commentSs.str();
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
         cout << dtTagPos;
         cout.flush();
         std::cout << "\nTotally" << r.Size() << "pics\n";
         //get img tag
         imgTagStr = r[dtTagPos]["children"][0]["children"][0]["class"].GetString();
//         imgTagStr = r[dtTagPos]["children"][0]["children"][0]["children"][0]["tag"].GetString();
         cout << imgTagStr;
      }
      cout << "check if it's img";
      if ( "item J-photo" == imgTagStr )
      {
         cout << "is img!";
         picLinkSs <<  r[dtTagPos]["children"][0]["children"][0]["href"].GetString();
         cout << picLinkSs.str();
      }
      else
      {
         //no picture, we won't put it into record
         continue;
      }

      //Only put the comment into record file if it contains picture link
      outFs << "\n{\n";
      outFs << "   \"shopId\"   : ";
      outFs << "\"" << shopIdStr << "\"" << ",\n";
      outFs << "   \"user_id\"  : ";
      outFs << "\"" << userIdSs.str() << "\"" << ",\n";

      outFs << "   \"comments\" : ";
      outFs << "\"" << commentSs.str() << "\"";
      outFs << ",\n";
      //cout << picLinkSs.str();
      getPic( picLinkSs.str(), shopIdStr, userIdSs.str(), outFs );
      outFs << "\n}";

      if( isEndOfTable && ( i + 1 ) == a.Size() )
      {
         cout << "end of comments";
         outFs <<"\n";
      }
      else
      {
         outFs << ",\n";
      }
   }
}

int main( int argc, char* argv[] ) {

    // Prepare reader and input stream.
    Reader reader;
    string shopIdStr = argv[1];
    string nameStr = argv[2];
    ofstream outFs;

    if ( argc < 3 )
    {
       cout << "usage: shopIdString, resultfile";
       return 0;
    }

    bool isEndOfTable = false;
    if ( argc > 3 )
    {
       string isTailStr = argv[3];
       if ( "tail" == isTailStr )
       {
          isEndOfTable = true;
       }
    }
    //get working path
    stringstream ss;
    ss << nameStr << "/";
    pathStr = ss.str().c_str();
    ss << "sample/";
    samplePathStr = ss.str().c_str();

    //open record file for appending
    string recordFileNameStr = pathStr + nameStr + ".txt";
    outFs.open( recordFileNameStr.c_str(), std::ios_base::app );

    //first command, download then extract comment
    string filterStr = "\" | pup ul.\"comment-list\" json{}";
    std::string headStr = " -s \"www.dianping.com/shop/" ;
    headStr += shopIdStr;
    headStr += filterStr;

    //save this into each shop's json record files
    std::ofstream resultFile;
    stringstream rss;
    rss << samplePathStr.c_str() << nameStr << shopIdStr << ".txt";
    resultFile.open( rss.str().c_str() );

    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    string strBuf;

    {
       stringstream ss;
       ss << "curl" << cookieStr << headStr;
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
       cout << "parsed";
       cout.flush();
       //const Value& a = document["children"][0]["children"][0]["children"];
       const Value& b = document["children"];
       assert(b.IsArray());

       string tagStr =  b[0]["tag"].GetString();
       if ( "div" == tagStr )
       {
          ExtractCommentAndPic( b[0]["children"][0]["children"], shopIdStr, outFs, isEndOfTable );
       }
       else
       {
          ExtractCommentAndPic( b, shopIdStr, outFs, isEndOfTable );
       }

       /*
       if ( !(a.Size()) 
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
