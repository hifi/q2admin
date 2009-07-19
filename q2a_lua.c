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

	/* careful! always when the engine gets control back use SetFPU! */
	Sys_SetFPU();

	gi.dprintf("%s", str);

	/* now Lua has the control, reset original state */
	Sys_ResetFPU();

	return 0;
}

static void *q2a_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	//gi.dprintf("q2a_lua_alloc(ud=%p, ptr=%p, osize=%ld, nsize=%ld)\n");
	(void)ud; (void)osize;
	if(nsize == 0) {
		q2a_free(ptr);
		return NULL;
	} else {
		return q2a_realloc(ptr, nsize);
	}
}

void q2a_lua_init(void)
{
	if(lua_L) return;

	Sys_ResetFPU();

	lua_L = lua_newstate(q2a_lua_alloc, NULL);
	luaL_openlibs(lua_L);

	gi.dprintf("q2a_lua_init: loading stored Lua code, %d bytes\n", strlen(LUA_PLUGMAN));

	/* load plugin manager code */
	if(luaL_loadstring(lua_L, LUA_PLUGMAN) != 0) {
		gi.dprintf("q2a_lua_init: Plugin manager code load failed, disabling Lua support\n");
		q2a_lua_shutdown();
		return;
	}
	if(lua_pcall(lua_L, 0, 0, 0) != 0) {
		char *err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("q2a_lua_init: Plugin manager code execution failed, disabling Lua support: %s\n", err_msg);
		q2a_lua_shutdown();
		return;
	}

	/* register gi functions */
	lua_newtable(lua_L);
	lua_setglobal(lua_L, "gi");

	lua_getglobal(lua_L, "gi");
	lua_pushcfunction(lua_L, q2a_lua_gi_dprintf);
	lua_setfield(lua_L, 1, "dprintf");

	/* run the initialization Lua routine */
	lua_getglobal(lua_L, "q2a_init");
	if(lua_pcall(lua_L, 0, 0, 0) != 0) {
		char *err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("q2a_lua_init: calling q2a_init failed: %s\n", err_msg);
		q2a_lua_shutdown();
		return;
	}

	Sys_SetFPU();
}

void q2a_lua_shutdown(void)
{
	if(!lua_L) return;

	Sys_ResetFPU();

	/* run the shutdown Lua routine */
	lua_getglobal(lua_L, "q2a_shutdown");
	lua_pcall(lua_L, 0, 0, 0);

	lua_close(lua_L);
	lua_L = NULL;

	Sys_SetFPU();
}

void q2a_lua_load(const char *file)
{
	if(!lua_L) return;
}
