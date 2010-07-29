//
// q2admin
//
// q2a_lua.h
//
// copyright 2009 Toni Spets
//

#ifndef Q2A_LUA_H

#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* include the plugin manager code */
#include "q2a_lua_plugman.h"

extern lua_State *L;

/* functions exported to Lua */
int q2a_lua_gi_dprintf(lua_State *L);
int q2a_lua_gi_bprintf(lua_State *L);
int q2a_lua_gi_cprintf(lua_State *L);
int q2a_lua_gi_centerprintf(lua_State *L);
int q2a_lua_gi_argc(lua_State *L);
int q2a_lua_gi_argv(lua_State *L);
int q2a_lua_gi_AddCommandString(lua_State *L);
int q2a_lua_stuffcmd(lua_State *L);
int q2a_lua_cvar(lua_State *L);
int q2a_lua_cvar_set(lua_State *L);

#define Q2A_LUA_H
#endif
