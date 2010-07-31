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

int q2a_lua_gi_bprintf(lua_State *L)
{
	// FIXME: do things like real printf(fmt, ...)
	int lvl;
	char *str;
	lvl = lua_tointeger(L, 1);
	str = (char *)lua_tostring(L, 2);

	q2a_fpu_q2();

	gi.bprintf(lvl, str);

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

	ent = getEnt(client);

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

	ent = getEnt(client);

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

static int q2a_lua_cvar_index(lua_State *L)
{
	cvar_t *cvar = *(cvar_t **)lua_touserdata(L, 1);
	char *key = (char *)luaL_checkstring(L, 2);

	if(!strcmp(key, "name")) {
		lua_pushstring(L, cvar->name);
		return 1;
	}

	if(!strcmp(key, "string")) {
		lua_pushstring(L, cvar->string);
		return 1;
	}

	if(!strcmp(key, "latched_string")) {
		lua_pushstring(L, cvar->latched_string);
		return 1;
	}

	if(!strcmp(key, "flags")) {
		lua_pushnumber(L, cvar->flags);
		return 1;
	}

	if(!strcmp(key, "modified")) {
		lua_pushboolean(L, cvar->modified);
		return 1;
	}

	if(!strcmp(key, "value")) {
		lua_pushnumber(L, cvar->value);
		return 1;
	}

	return 0;
}

void q2a_lua_cvar_register(lua_State *L)
{
	luaL_newmetatable(L, "Cvar");
	lua_pushcfunction(L, q2a_lua_cvar_index);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
}

int q2a_lua_gi_cvar(lua_State *L)
{
	cvar_t *cvar;

	char *key,*value;
	int flags;

	key = (char *)luaL_checkstring(L, 1);
	value = (char *)luaL_checkstring(L, 2);
	flags = (int)lua_tointeger(L, 3);

	q2a_fpu_q2();

	cvar = gi.cvar(key, value, flags);

	q2a_fpu_lua();

	*(cvar_t **)lua_newuserdata(L, sizeof(cvar_t *)) = cvar;
	luaL_getmetatable(L, "Cvar");
	lua_setmetatable(L, -2);
	return 1;
}

int q2a_lua_gi_cvar_set(lua_State *L)
{
	cvar_t *cvar;

	char *key, *value;

	key = (char *)lua_tostring(L, 1);
	value = (char *)lua_tostring(L, 2);

	q2a_fpu_q2();

	cvar = gi.cvar_set(key, value);

	q2a_fpu_lua();

	*(cvar_t **)lua_newuserdata(L, sizeof(cvar_t *)) = cvar;
	luaL_getmetatable(L, "Cvar");
	lua_setmetatable(L, -2);

	return 1;
}

int q2a_lua_gi_cvar_forceset(lua_State *L)
{
	cvar_t *cvar;

	char *key, *value;

	key = (char *)lua_tostring(L, 1);
	value = (char *)lua_tostring(L, 2);

	q2a_fpu_q2();

	cvar = gi.cvar_forceset(key, value);

	q2a_fpu_lua();

	*(cvar_t **)lua_newuserdata(L, sizeof(cvar_t *)) = cvar;
	luaL_getmetatable(L, "Cvar");
	lua_setmetatable(L, -2);

	return 1;
}

int q2a_lua_stuffcmd(lua_State *L)
{
	edict_t *ent;
	int client;
	char *str;

	client = lua_tointeger(L, 1);
	str = (char *)lua_tostring(L, 2);

	ent = getEnt(client);

	q2a_fpu_q2();

	stuffcmd(ent, str);

	q2a_fpu_lua();

	return 0;
}

int q2a_lua_Info_ValueForKey(lua_State *L)
{
	char *userinfo = lua_touserdata(L, 1);
	char *key = (char *)luaL_checkstring(L, 2);

	lua_pushstring(L, Info_ValueForKey(userinfo, key));
	return 1;
}

int q2a_lua_Info_SetValueForKey(lua_State *L)
{
	char *userinfo = lua_touserdata(L, 1);
	char *key = (char *)luaL_checkstring(L, 2);
	char *value = (char *)luaL_checkstring(L, 3);

	Info_SetValueForKey(userinfo, key, value);
	return 0;
}
