//
// q2admin
//
// q2a_lua.c
//
// copyright 2009 Toni Spets
//

#include "g_local.h"
#include "q2a_lua.h"

lua_State *L = NULL;

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
	if(L) return;

	q2a_fpu_lua();

	L = lua_newstate(q2a_lua_alloc, NULL);
	luaL_openlibs(L);

	gi.dprintf("q2a_lua_init: loading stored Lua code, %d bytes\n", strlen(LUA_PLUGMAN));

	/* load plugin manager code */
	if(luaL_loadstring(L, LUA_PLUGMAN) != 0) {
		gi.dprintf("q2a_lua_init: Plugin manager code load failed, disabling Lua support\n");
		q2a_fpu_q2();
		q2a_lua_shutdown();
		return;
	}
	if(lua_pcall(L, 0, 0, 0) != 0) {
		char *err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("q2a_lua_init: Plugin manager code execution failed, disabling Lua support: %s\n", err_msg);
		q2a_fpu_q2();
		q2a_lua_shutdown();
		return;
	}

	/* register "constants" */
	lua_pushnumber(L, 0);
	lua_setglobal(L, "PRINT_LOW");
	lua_pushnumber(L, 1);
	lua_setglobal(L, "PRINT_MEDIUM");
	lua_pushnumber(L, 2);
	lua_setglobal(L, "PRINT_HIGH");
	lua_pushnumber(L, 3);
	lua_setglobal(L, "PRINT_CHAT");

	/* register gi functions */
	lua_newtable(L); // gi table

	lua_pushcfunction(L, q2a_lua_gi_dprintf);
	lua_setfield(L, 1, "dprintf");

	lua_pushcfunction(L, q2a_lua_gi_cprintf);
	lua_setfield(L, 1, "cprintf");

	lua_pushcfunction(L, q2a_lua_gi_centerprintf);
	lua_setfield(L, 1, "centerprintf");

	lua_pushcfunction(L, q2a_lua_gi_argc);
	lua_setfield(L, 1, "argc");

	lua_pushcfunction(L, q2a_lua_gi_argv);
	lua_setfield(L, 1, "argv");

	lua_pushcfunction(L, q2a_lua_gi_AddCommandString);
	lua_setfield(L, 1, "AddCommandString");

	lua_setglobal(L, "gi");

	/* run the initialization Lua routine */
	lua_getglobal(L, "q2a_init");
	if(lua_pcall(L, 0, 0, 0) != 0) {
		char *err_msg = (char *)lua_tostring(L, -1);
		gi.dprintf("q2a_lua_init: calling q2a_init failed: %s\n", err_msg);
		q2a_fpu_q2();
		q2a_lua_shutdown();
		return;
	}

	q2a_fpu_q2();
}

void q2a_lua_shutdown(void)
{
	if(!L) return;

	q2a_fpu_lua();

	/* run the shutdown Lua routine */
	lua_getglobal(L, "q2a_shutdown");
	lua_pcall(L, 0, 0, 0);

	lua_close(L);
	L = NULL;

	q2a_fpu_q2();
}

void q2a_lua_load(const char *file)
{
	if(!L) return;
}

/* x86 workaround for Lua, fsck you Carmack! */
unsigned short q2a_fpuword = 0;
void q2a_fpu_q2 (void)
{
#ifdef __i386__
	assert(q2a_fpuword != 0);
	__asm__ __volatile__ ("fldcw %0" : : "m" (q2a_fpuword));
	q2a_fpuword = 0;
#endif
}

void q2a_fpu_lua (void)
{
#ifdef __i386__
	unsigned short tmp = 0x37F;
	assert(q2a_fpuword == 0);
	__asm__ __volatile__ ("fnstcw %0" : "=m" (q2a_fpuword));
	__asm__ __volatile__ ("fldcw %0" : : "m" (tmp));
#endif
}
