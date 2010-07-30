//
// q2admin
//
// q2a_cmd.c
//
// copyright 2000 Shane Powell
// copyright 2009 Toni Spets
//

#include "g_local.h"

void dprintf_internal(char *fmt, ...)
{
        char cbuffer[8192];
        va_list arglist;

        // convert to string
        va_start(arglist, fmt);
        vsprintf(cbuffer, fmt, arglist);
        va_end(arglist);

	gi.dprintf("%s", cbuffer);
	return;
}


void cprintf_internal(edict_t *ent, int printlevel, char *fmt, ...)
{
	char cbuffer[8192];
	va_list arglist;
	
	// convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);
	
	gi.cprintf(ent, printlevel, "%s", cbuffer);
	return;
}

void bprintf_internal(int printlevel, char *fmt,...)
{
	char cbuffer[8192];
	va_list arglist;
	
	// convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);
	
	gi.bprintf(printlevel, "%s", cbuffer);
	return;
}


void AddCommandString_internal(char *text)
{
	gi.AddCommandString(text);
	return;
}


char argtext[2048];

char *getArgs(void)
{
	char *p;
	
	p = gi.args();
	q2a_strcpy(argtext, p);
	p = argtext;
	
	if (*p == '"')
		{
			p++;
			p[q2a_strlen(p)-1] = 0;
		}
		
	return p;
}


char cmdbuf[1024];

void ClientCommand (edict_t *ent)
{
	int client = getEntOffset(ent) - 1;
	
	if(!dllloaded) return;

	if(q2a_lua_ClientCommand(client))
		return;

	dllglobals->ClientCommand(ent);
	copyDllInfo();
}

void callback(char *buf, int size)
{
	gi.dprintf("Download finished, got %d bytes.\n", size);
}

void ServerCommand (void)
{
	char *cmd;

	if(!dllloaded) return;

	cmd = gi.argv(1);

	if(!q2a_strcmp(cmd, "lua_reload")) {
		q2a_lua_shutdown();
		q2a_lua_init();
		return;
	}

	if(q2a_lua_ServerCommand(cmd))
		return;
		
	dllglobals->ServerCommand();
	copyDllInfo();
}
