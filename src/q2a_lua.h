#ifndef Q2A_LUA_H

#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* include the plugin manager code */
#include "q2a_lua_plugman.h"

extern lua_State *lua_L;

/* functions exported to Lua */
int q2a_lua_gi_dprintf(lua_State *lua_L);
int q2a_lua_gi_cprintf(lua_State *lua_L);
int q2a_lua_gi_centerprintf(lua_State *lua_L);
int q2a_lua_gi_argc(lua_State *lua_L);
int q2a_lua_gi_argv(lua_State *lua_L);
int q2a_lua_gi_AddCommandString(lua_State *lua_L);

#define Q2A_LUA_H
#endif
