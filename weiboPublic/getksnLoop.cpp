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
   for( int i = 0; i < 1000; i++ )
   {
      system( "cmd /C ""getksn.bat"" ");
      sleep(60);
   }
   return 0;
}

