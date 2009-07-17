#include "g_local.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* include the plugin manager code */
#include "q2a_lua_plugman.h"

#define LUA_CODE_BAD \
	"local foo = { \"bar\" }\n"  \
	"for k,v in pairs(foo) do\n" \
	"end\n"

#define LUA_CODE_GOOD \
	"local foo = { }\n" \
	"table.insert(foo, \"bar\")\n" \
	"for k,v in pairs(foo) do\n" \
	"end\n"

lua_State *lua_L = NULL;

static int q2a_lua_gi_dprintf(lua_State *lua_L)
{
	// FIXME: do things like real printf(fmt, ...)
	char *str;
	str = (char *)lua_tostring(lua_L, 1);
	gi.dprintf("%s", str);
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
	//lua_L = lua_open();
	lua_L = lua_newstate(q2a_lua_alloc, NULL);
	luaL_openlibs(lua_L);

#if 1
	luaL_loadstring(lua_L, LUA_CODE_BAD);
#else
	luaL_loadstring(lua_L, LUA_CODE_GOOD);
#endif
	if(lua_pcall(lua_L, 0, 0, 0) != 0) {
		char *err_msg = (char *)lua_tostring(lua_L, -1);
		gi.dprintf("q2a_lua_init: executing failed: %s\n", err_msg);
		q2a_lua_shutdown();
		return;
	}
#if 0

	gi.dprintf("q2a_lua_init: loading stored Lua code, %d bytes\n", strlen(LUA_PLUGMAN));

	/* load plugin manager code */
	if(luaL_loadstring(lua_L, LUA_PLUGMAN) != 0) {
		gi.dprintf("q2a_lua_init: Plugin manager code load failed, disabling Lua support\n");
		q2a_lua_shutdown();
		return;
	}
	luaL_loadstring(lua_L, LUA_CODE_BAD);

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
#endif
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
