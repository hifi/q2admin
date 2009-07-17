#include "g_local.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

lua_State *lua_L = NULL;

void q2a_lua_init(void)
{
	if(lua_L) return;
	lua_L = lua_open();
	luaL_openlibs(lua_L);

	// create global plugins table
	lua_newtable(lua_L);
	lua_setglobal(lua_L, "plugins");
}

void q2a_lua_shutdown(void)
{
	if(!lua_L) return;
	lua_close(lua_L);
	lua_L = NULL;
}

void q2a_lua_load(const char *file)
{
	int error;

	if(!lua_L) return;

	printf("q2a_lua_load(%s)\n", file);

	lua_newtable(lua_L);
	lua_settop(lua_L, 0);
	lua_getglobal(lua_L, "plugins");
	lua_insert(lua_L, 0);

	// create new environment for this code
	//lua_

	// and finally load the code
	luaL_dofile(lua_L, file);
}
