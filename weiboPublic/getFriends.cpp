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

int main( int argc, char* argv[] ) {

    // Prepare reader and input stream.
    Reader reader;


    if ( argc < 2 )
    {
       cout << "usage: getFriends tokenstring";
       return 0;
       
    }


    string frindsApiStr = " -s \"https://api.weibo.com/2/friendships/friends.json?source=2813060966&access_token=";
    string uidStr = "&uid=1963116122";
    
    std::string headStr = "curl " ;
    headStr +=frindsApiStr;
    headStr += argv[1];
    headStr += uidStr;
    std::ofstream resultFile;
    resultFile.open("friendsList.txt");

    uint64_t nextCursor = 0;
    uint64_t totalNumber = 0;
    do 
    {
       char readBuffer[65536];
       stringstream ss;
       ss << headStr << "&cursor=" << nextCursor <<'\"';
       string result = exec( ss.str().c_str() );
       Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

#if 1
       // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
       if (document.Parse(result.c_str()).HasParseError())
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

       //   printf("\nAccess values in document:\n");
       assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.
       const Value& a = document["users"]; // Using a reference for consecutive access is handy and faster.
       assert(a.IsArray());
       for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
       {
          resultFile <<  a[i]["idstr"].GetString()  << "," << a[i]["name"].GetString() << '\n';
       }
       nextCursor += a.Size();
       totalNumber = document["total_number"].GetUint();

       if ( !(a.Size()) )
       {
          break;
       }
       //cout << nextCursor << '\n';
       //cout << totalNumber << '\n';
       //cout.flush();
    } while (  nextCursor <= totalNumber );

    resultFile.close();
    return 0;
}
