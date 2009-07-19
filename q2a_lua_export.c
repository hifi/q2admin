#include "g_local.h"
#include "q2a_lua.h"

int q2a_lua_gi_dprintf(lua_State *lua_L)
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

