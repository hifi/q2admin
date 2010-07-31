//
// q2admin
//
// q2a_lua.c
//
// copyright 2009 Toni Spets
//

#include "g_local.h"
#include "q2a_lua.h"

/* include the compiled plugin manager code */
#include "q2a_lua_plugman.h"

#define MAX_PLAYERS maxclients->value

lua_State *L = NULL;

/* this isn't working.. why? */
static void *q2a_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	void *ret = NULL;
	(void)ud; (void)osize;
	q2a_fpu_q2();
	if(nsize == 0) {
		q2a_free(ptr);
	} else {
		ret = q2a_realloc(ptr, nsize);
	}
	q2a_fpu_lua();
	return ret;
}

static int lua_players_index(lua_State *L)
{
	lua_touserdata(L, 1);
	int index = luaL_checkint(L, 2);

	luaL_argcheck(L, 1 <= index && index <= MAX_PLAYERS, 2, "index out of range");

	uint16_t *tmp = (uint16_t *)lua_newuserdata(L, sizeof(uint16_t));
	luaL_getmetatable(L, "Player");
	lua_setmetatable(L, -2);

	*tmp = (uint16_t)index;

	return 1;
}

static int lua_player_index(lua_State *L)
{
	uint16_t *index = (uint16_t *)lua_touserdata(L, 1);
	char *key = (char *)lua_tostring(L, 2);

	luaL_argcheck(L, 1 <= *index && *index <= MAX_PLAYERS, 1, "client index expected");

	if(!strncmp(key, "name", 4)) {
		lua_pushstring(L, playerinfo[*index - 1].name);
		return 1;
	}

	if(!strncmp(key, "skin", 4)) {
		lua_pushstring(L, playerinfo[*index - 1].skin);
		return 1;
	}

	if(!strncmp(key, "ip", 2)) {
		lua_pushstring(L, playerinfo[*index - 1].ip_str);
		return 1;
	}

	if(!strncmp(key, "ip_bin", 6)) {
		lua_pushnumber(L, playerinfo[*index - 1].ip);
		return 1;
	}

	if(!strncmp(key, "inuse", 5)) {
		lua_pushboolean(L, playerinfo[*index - 1].inuse);
		return 1;
	}

	if(!strncmp(key, "userinfo", 2)) {
		lua_pushstring(L, playerinfo[*index - 1].userinfo);
		return 1;
	}

	return 0;
}

void q2a_lua_init(void)
{
	if(L) return;

	q2a_fpu_lua();

	//L = lua_newstate(q2a_lua_alloc, NULL);
	L = lua_open();
	luaL_openlibs(L);

	gi.dprintf("q2a_lua_init: loading stored Lua code, %d bytes\n", sizeof(q2a_lua_plugman));

	/* load plugin manager code */
	if(luaL_loadbuffer(L, (const char *)q2a_lua_plugman, sizeof(q2a_lua_plugman), "q2a_lua_plugman") != 0) {
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

	q2a_lua_cvar_register(L);

	lua_newuserdata(L, 0);
	luaL_newmetatable(L, "Players");
	lua_pushcfunction(L, lua_players_index);
	lua_setfield(L, -2, "__index");	
	lua_setmetatable(L, -2);
	lua_setglobal(L, "players");

	luaL_newmetatable(L, "Player");
	lua_pushcfunction(L, lua_player_index);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	/* register "constants" */
	lua_pushnumber(L, 0);
	lua_setglobal(L, "PRINT_LOW");
	lua_pushnumber(L, 1);
	lua_setglobal(L, "PRINT_MEDIUM");
	lua_pushnumber(L, 2);
	lua_setglobal(L, "PRINT_HIGH");
	lua_pushnumber(L, 3);
	lua_setglobal(L, "PRINT_CHAT");

	lua_pushnumber(L, CVAR_ARCHIVE);
	lua_setglobal(L, "CVAR_ARCHIVE");
	lua_pushnumber(L, CVAR_USERINFO);
	lua_setglobal(L, "CVAR_USERINFO");
	lua_pushnumber(L, CVAR_SERVERINFO);
	lua_setglobal(L, "CVAR_SERVERINFO");
	lua_pushnumber(L, CVAR_NOSET);
	lua_setglobal(L, "CVAR_NOSET");
	lua_pushnumber(L, CVAR_LATCH);
	lua_setglobal(L, "CVAR_LATCH");

	/* register gi functions */
	lua_newtable(L); // gi table

	lua_pushcfunction(L, q2a_lua_gi_dprintf);
	lua_setfield(L, 1, "dprintf");

	lua_pushcfunction(L, q2a_lua_gi_bprintf);
	lua_setfield(L, 1, "bprintf");

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

	lua_pushcfunction(L, q2a_lua_gi_cvar);
	lua_setfield(L, 1, "cvar");

	lua_pushcfunction(L, q2a_lua_gi_cvar_set);
	lua_setfield(L, 1, "cvar_set");

	lua_pushcfunction(L, q2a_lua_gi_cvar_forceset);
	lua_setfield(L, 1, "cvar_forceset");

	lua_setglobal(L, "gi");

	lua_pushcfunction(L, q2a_lua_stuffcmd);
	lua_setglobal(L, "stuffcmd");

	lua_pushcfunction(L, q2a_lua_Info_ValueForKey);
	lua_setglobal(L, "Info_ValueForKey");

	lua_pushcfunction(L, q2a_lua_Info_SetValueForKey);
	lua_setglobal(L, "Info_SetValueForKey");

	/* run the initialization Lua routine */
	lua_getglobal(L, "q2a_init");
	lua_pushstring(L, q2a_config->string);
	if(lua_pcall(L, 1, 0, 0) != 0) {
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
