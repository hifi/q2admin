#include "g_local.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* include the plugin manager code */
#include "q2a_lua_plugman.h"

lua_State *lua_L = NULL;

static int q2a_lua_gi_dprintf(lua_State *lua_L)
{
	// FIXME: do things like real printf(fmt, ...)
	char *str;
	str = (char *)lua_tostring(lua_L, 1);
	gi.dprintf("%s", str);
	return 0;
}

void q2a_lua_init(void)
{
	if(lua_L) return;
	lua_L = lua_open();
	luaL_openlibs(lua_L);

	/* load plugin manager code */
	if(luaL_loadstring(lua_L, LUA_PLUGMAN) != 0) {
		gi.dprintf("q2a_lua_init: Plugin manager code load failed, disabling Lua support");
		q2a_lua_shutdown();
		return;
	}
	lua_call(lua_L, 0, 0);

	/* insert output functions to global namespace */

	/* gi functions */
	lua_newtable(lua_L);
	lua_setglobal(lua_L, "gi");

	lua_getglobal(lua_L, "gi");
	lua_pushcfunction(lua_L, q2a_lua_gi_dprintf);
	lua_setfield(lua_L, 1, "dprintf");

	/* run the initialization Lua routine */
	lua_getglobal(lua_L, "q2a_init");
	lua_call(lua_L, 0, 0);
}

void q2a_lua_shutdown(void)
{
	if(!lua_L) return;

	/* run the shutdown Lua routine */
	lua_getglobal(lua_L, "q2a_shutdown");
	lua_call(lua_L, 0, 0);

	lua_close(lua_L);
	lua_L = NULL;
}

void q2a_lua_load(const char *file)
{
	if(!lua_L) return;
}
