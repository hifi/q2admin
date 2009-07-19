#include "g_local.h"
#include "q2a_lua.h"

qboolean q2a_lua_ClientConnect(int client)
{
	char *err_msg;
	qboolean ret = TRUE;

	if(!lua_L) return ret;

	q2a_fpu_lua();

	lua_getglobal(lua_L, "q2a_call_bool");
	lua_pushstring(lua_L, "ClientConnect");
	lua_pushboolean(lua_L, 1); // default status is 1, accept connection
	lua_pushnumber(lua_L, client);

	if(lua_pcall(lua_L, 3, 1, 0) == 0) {
		ret = lua_toboolean(lua_L, -1);
	} else {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: ClientConnect returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();

	return ret;
}

void q2a_lua_ClientBegin(int client)
{
	char *err_msg;
	if(!lua_L) return;

	q2a_fpu_lua();

	lua_getglobal(lua_L, "q2a_call");
	lua_pushstring(lua_L, "ClientBegin");
	lua_pushnumber(lua_L, client);

	if(lua_pcall(lua_L, 2, 1, 0) != 0) {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: ClientBegin returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_ClientDisconnect(int client)
{
	char *err_msg;
	if(!lua_L) return;

	q2a_fpu_lua();

	lua_getglobal(lua_L, "q2a_call");
	lua_pushstring(lua_L, "ClientDisconnect");
	lua_pushnumber(lua_L, client);

	if(lua_pcall(lua_L, 2, 1, 0) != 0) {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: ClientDisconnect returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_ClientThink(int client)
{
	char *err_msg;
	if(!lua_L) return;

	q2a_fpu_lua();

	lua_getglobal(lua_L, "q2a_call");
	lua_pushstring(lua_L, "ClientThink");
	lua_pushnumber(lua_L, client);

	if(lua_pcall(lua_L, 2, 1, 0) != 0) {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: ClientThink returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

void q2a_lua_RunFrame(void)
{
	char *err_msg;
	if(!lua_L) return;

	q2a_fpu_lua();

	lua_getglobal(lua_L, "q2a_call");
	lua_pushstring(lua_L, "RunFrame");

	if(lua_pcall(lua_L, 1, 1, 0) != 0) {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: RunFrame returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();
}

qboolean q2a_lua_ClientCommand(int client, const char *cmd)
{
	char *err_msg;
	qboolean ret = FALSE;

	if(!lua_L) return ret;

	q2a_fpu_lua();

	lua_getglobal(lua_L, "q2a_call_bool");
	lua_pushstring(lua_L, "ClientCommand");
	lua_pushboolean(lua_L, 0); // default status is 0, don't capture
	lua_pushnumber(lua_L, client);
	lua_pushstring(lua_L, cmd);

	if(lua_pcall(lua_L, 4, 1, 0) == 0) {
		ret = lua_toboolean(lua_L, -1);
	} else {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: ClientCommand returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();

	return ret;
}
