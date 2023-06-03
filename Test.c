#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#pragma lua module "test"

#define lua_export(...) clang::annotate("lua export "#__VA_ARGS__)

[[lua_export(function)]]
long add(long x, long y)
{ return x + y; }

[[lua_export(function)]]
extern int puts(const char *s);
