//
// q2admin
//
// q2a_lua_export.c
//
// copyright 2009 Toni Spets
//

#include "g_local.h"
#include "q2a_lua.h"

int q2a_lua_gi_dprintf(lua_State *L)
{
	// FIXME: do things like real printf(fmt, ...)
	char *str;
	str = (char *)lua_tostring(L, 1);

	q2a_fpu_q2();

	gi.dprintf("%s", str);

	q2a_fpu_lua();

	return 0;
}

int q2a_lua_gi_cprintf(lua_State *L)
{
	// FIXME: do things like real printf(fmt, ...)
	edict_t *ent;
	int client;
	int lvl;
	char *str;

	client = lua_tointeger(L, 1);
	lvl = lua_tointeger(L, 2);
	str = (char *)lua_tostring(L, 3);

	ent = getEnt((client + 1));

	q2a_fpu_q2();

	gi.cprintf(ent, lvl, str);

	q2a_fpu_lua();

	return 0;
}

int q2a_lua_gi_centerprintf(lua_State *L)
{
	// FIXME: do things like real printf(fmt, ...)
	edict_t *ent;
	int client;
	char *str;

	client = lua_tointeger(L, 1);
	str = (char *)lua_tostring(L, 2);

	ent = getEnt((client + 1));

	q2a_fpu_q2();

	gi.centerprintf(ent, str);

	q2a_fpu_lua();

	return 0;
}

int q2a_lua_gi_argc(lua_State *L)
{
	int argc;

	q2a_fpu_q2();
	argc = gi.argc();
	q2a_fpu_lua();

	lua_pushinteger(L, gi.argc());

	return 1;
}

int q2a_lua_gi_argv(lua_State *L)
{
	int num;
	char *str;

	num = lua_tointeger(L, 1);

	q2a_fpu_q2();
	str = gi.argv(num);
	q2a_fpu_lua();

	lua_pushstring(L, str);

	return 1;
}

int q2a_lua_gi_AddCommandString(lua_State *L)
{
	char *str;

	str = (char *)lua_tostring(L, 1);

	q2a_fpu_q2();
	gi.AddCommandString(str);
	q2a_fpu_lua();

	return 0;
}
