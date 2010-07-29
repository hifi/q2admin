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
	char *cmd;
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

	if(q2a_strlen(cmd) > 0) {
		if(cmd[0] == '!') {
			q2a_strcpy(cmdbuf, cmd);
			cmd = cmdbuf;
			cmd++;

			if(!q2a_strcmp(cmd, "lua_reload")) {
				q2a_lua_shutdown();
				q2a_lua_init();
				return;
			}

			// cmd now contains the command, argv 2+ contains the parameters
			// handle all server commands here, return when command is captured
			if(!q2a_strcmp(cmd, "curl_reload")) {
				if(q2a_http_status() != Q2A_HTTP_IDLE) {
					gi.dprintf("Q2A: libCURL is downloading, no reload possible at this time\n");
					return;
				}

				gi.dprintf("Q2A: reloading libCURL (this refreshes the DNS cache)\n");
				q2a_http_shutdown();
				q2a_http_init();
				return;
			}

			if(!q2a_strcmp(cmd, "curl_status")) {
				int curl_status = q2a_http_status();
				if(curl_status == Q2A_HTTP_NOT_READY)
					gi.dprintf("Q2A: libCURL status: not initialized\n");
				else if(curl_status == Q2A_HTTP_IDLE)
					gi.dprintf("Q2A: libCURL status: idle\n");
				else if(curl_status == Q2A_HTTP_WORKING)
					gi.dprintf("Q2A: libCURL status: downloading\n");
				return;
			}

			if(!q2a_strcmp(cmd, "curl")) {
				gi.dprintf("Q2A: trying curl...\n");
				q2a_http_get("http://hifi.iki.fi/index.html", callback, NULL, NULL);
				return;
			}

			if(!q2a_strcmp(cmd, "curl_large")) {
				gi.dprintf("Q2A: trying curl with large file...\n");
				q2a_http_get("http://hifi.iki.fi/quake2/q2-3.20-x86-full-ctf.exe", callback, NULL, NULL);
				return;
			}

			if(q2a_lua_ServerCommand(cmd))
				return;

			gi.dprintf("Q2A: Unknown server command: %s\n", cmd);
			return;
		}
	}
		
	dllglobals->ServerCommand();
	copyDllInfo();
}
