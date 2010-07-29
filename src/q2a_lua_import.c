//
// q2admin
//
// q2a_lua_import.c
//
// copyright 2009 Toni Spets
//

#include "g_local.h"
#include "q2a_lua.h"

qboolean q2a_lua_ClientConnect(int client, char *userinfo)
{
	char *err_msg,*rej_msg = NULL;
	qboolean ret = TRUE;

	if(!L) return ret;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call_bool");
	lua_pushstring(L, "ClientConnect");
	lua_pushboolean(L, TRUE);
	lua_pushnumber(L, client + 1);

	if(lua_pcall(L, 3, 2, 0) == 0) {
		ret = lua_toboolean(L, -2);
                rej_msg = (char *)lua_tostring(L, -1);
	} else {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientConnect returned error: %s\n", err_msg);
		ret = TRUE;
	}

	q2a_fpu_q2();

        if(!ret && rej_msg != NULL) {
            userinfo[0] = 0;
            Info_SetValueForKey(userinfo, "rejmsg", rej_msg);
        }

	return ret;
}

void q2a_lua_ClientBegin(int client)
{
	char *err_msg;
	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "ClientBegin");
	lua_pushnumber(L, client + 1);

	if(lua_pcall(L, 2, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientBegin returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_ClientDisconnect(int client)
{
	char *err_msg;
	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "ClientDisconnect");
	lua_pushnumber(L, client + 1);

	if(lua_pcall(L, 2, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientDisconnect returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_ClientThink(int client)
{
	char *err_msg;
	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "ClientThink");
	lua_pushnumber(L, client + 1);

	if(lua_pcall(L, 2, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientThink returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_RunFrame(void)
{
	char *err_msg;
	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "RunFrame");

	if(lua_pcall(L, 1, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: RunFrame returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

qboolean q2a_lua_ClientCommand(int client)
{
	char *err_msg;
	qboolean ret = FALSE;

	if(!L) return ret;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call_bool");
	lua_pushstring(L, "ClientCommand");
	lua_pushboolean(L, FALSE);
	lua_pushnumber(L, client + 1);

	if(lua_pcall(L, 3, 1, 0) == 0) {
		ret = lua_toboolean(L, -1);
	} else {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientCommand returned error: %s\n", err_msg);
		ret = FALSE;
	}

	q2a_fpu_q2();

	return ret;
}

qboolean q2a_lua_ServerCommand(const char *cmd)
{
	char *err_msg;
	qboolean ret = FALSE;

	if(!L) return ret;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call_bool");
	lua_pushstring(L, "ServerCommand");
	lua_pushboolean(L, FALSE);
	lua_pushstring(L, cmd);

	if(lua_pcall(L, 3, 1, 0) == 0) {
		ret = lua_toboolean(L, -1);
	} else {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ServerCommand returned error: %s\n", err_msg);
		ret = FALSE;
	}

	q2a_fpu_q2();

	return ret;
}

void q2a_lua_ClientNameChanged(int client, const char *new_name)
{
	char *err_msg;

	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "ClientNameChanged");
	lua_pushnumber(L, client + 1);
	lua_pushstring(L, new_name);

	if(lua_pcall(L, 3, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientNameChanged returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_ClientSkinChanged(int client, const char *new_skin)
{
	char *err_msg;

	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "ClientSkinChanged");
	lua_pushnumber(L, client + 1);
	lua_pushstring(L, new_skin);

	if(lua_pcall(L, 3, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: ClientSkinChanged returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_LevelChanged(const char *level)
{
	char *err_msg;

	if(!L) return;

	q2a_fpu_lua();

	lua_getglobal(L, "q2a_call");
	lua_pushstring(L, "LevelChanged");
	lua_pushstring(L, level);

	if(lua_pcall(L, 2, 0, 0) != 0) {
		err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("Lua: LevelChanged returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}
