// luatest.cpp : 定义控制台应用程序的入口点。
//
 
#include <stdlib.h>
#include "lua.hpp"


static void fatal(const char* message)
{
    fprintf(stderr,"%s: %s\n","test",message);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
      lua_State* L;
      L=luaL_newstate();
      luaL_openlibs(L);
      if (L==NULL)fatal("cannot create state: not enough memory");

      printf("argc: %d\n", argc);

      int i = 0;
      while (i++<argc)
      {
          printf("argv: %s\n", argv[i-1]);
      }

      if (luaL_loadfile(L, "main.lua") )
      {
          fatal("cannot call luaL_loadfile");
      }
      if(lua_pcall(L, 0, 0, 0))
      {
          fatal("cannot call lua_pcall");
      }

      return EXIT_SUCCESS;
}

