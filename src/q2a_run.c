//
// q2admin
//
// q2a_run.c
//
// copyright 2000 Shane Powell
// copyright 2009 Toni Spets
//

#include "g_local.h"

void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	int client = getEntOffset(ent) -1;
	
	if(!dllloaded) return;
		
	q2a_lua_ClientThink(client);
	
	dllglobals->ClientThink(ent, ucmd);
	
	copyDllInfo();
}

void G_RunFrame(void)
{
	q2a_http_tick();
	
	if(!dllloaded) return;

	q2a_lua_RunFrame();

	dllglobals->RunFrame();
	copyDllInfo();
	return;
}
