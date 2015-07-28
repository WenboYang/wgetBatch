#include <iostream>
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

int main( int argc, char* argv[] )
{
#if 0
   std::stringstream stream;

   char cCurrentPath[FILENAME_MAX];

   if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
   {
      return -1;
   }
   cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
   printf ("The current working directory is %s", cCurrentPath);

   stream << cCurrentPath << "\"" << "gttf.bat";
#endif
   for( int i = 0; i < 1000; i++ )
   {
      system( "cmd /C ""gettf"" ");
      sleep(60);
   }
   return 0;
}

