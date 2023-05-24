#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#pragma lua module "test"

#define lua_export(...) clang::annotate("lua export "#__VA_ARGS__)

[[lua_export(function)]]
static int add(int x, int y)
{ return x + y; }

[[lua_export(function)]]
static void say_hello(const char *to)
{ printf("Hello %s!\n", to); }
