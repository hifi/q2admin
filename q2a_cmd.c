/*
Copyright (C) 2000 Shane Powell

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

//
// q2admin
//
// q2a_cmd.c
//
// copyright 2000 Shane Powell
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
	//int client = getEntOffset(ent) - 1;
	
	if(!dllloaded) return;

	cmd = gi.argv(0);

	if(q2a_strlen(cmd) > 0) {
		if(cmd[0] == '!') {
			q2a_strcpy(cmdbuf, cmd);
			cmd = cmdbuf;
			cmd++;

			// cmd now contains the command, argv 1+ contains the parameters
			// handle all client commands here, return when command is captured

			if(!q2a_strcmp(cmd, "ping")) {
				gi.cprintf(ent, PRINT_HIGH, "Q2A: Pong!\n");
				return;
			}

			if(!q2a_strcmp(cmd, "admin")) {
				if(gi.argc() < 2) {
					gi.cprintf(ent, PRINT_HIGH, "Q2A: Usage: !admin <message>\n");
					return;
				}
				// do IRC informing or sumthing
				gi.cprintf(ent, PRINT_HIGH, "Q2A: The admins have been informed about the cheater/abuser. No further action required.\n");
				return;
			}

			gi.cprintf(ent, PRINT_HIGH, "Q2A: Unknown client command: %s\n", cmd);
			return;
		}
	}

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

			// cmd now contains the command, argv 2+ contains the parameters
			// handle all server commands here, return when command is captured

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

			gi.dprintf("Q2A: Unknown server command: %s\n", cmd);
			return;
		}
	}
		
	dllglobals->ServerCommand();
	copyDllInfo();
}

/* dump everything we know about the client with dprintf */
void q2a_dump_client(edict_t *ent)
{
	int client = getEntOffset(ent);

	gi.dprintf("Dumping entity %d\n", client);
	gi.dprintf("edict_s\n");
	gi.dprintf("{\n");
	gi.dprintf("\tentity_state_t s\n");
	gi.dprintf("\t{\n");
	gi.dprintf("\t}\n");
	gi.dprintf("\tgclients_ *client = %p\n", ent->client);
	if(ent->client) {
		gi.dprintf("\t{\n");
		gi.dprintf("\t\tping = %d\n", ent->client->ping);
		gi.dprintf("\t}\n");
	}
	gi.dprintf("\tinuse = %s\n", (ent->inuse ? "yes" : "no"));
	gi.dprintf("}\n");
}
