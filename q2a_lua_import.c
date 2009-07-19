#include "g_local.h"
#include "q2a_lua.h"

qboolean q2a_lua_ClientConnect(int client_id)
{
	char *err_msg;
	qboolean ret = TRUE;

	if(!lua_L) return ret;

	q2a_fpu_lua();

	/* run the shutdown Lua routine */
	lua_getglobal(lua_L, "q2a_call_bool");
	lua_pushstring(lua_L, "ClientConnect");
	lua_pushboolean(lua_L, 1); // default status is 1, accept connection
	lua_pushnumber(lua_L, client_id);

	if(lua_pcall(lua_L, 3, 1, 0) == 0) {
		ret = lua_toboolean(lua_L, -1);
	} else {
		err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("Lua: ClientConnect returned error: %s\n", err_msg);
	}

	q2a_fpu_q2();

	return ret;
}
