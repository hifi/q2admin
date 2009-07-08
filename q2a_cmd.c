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


void ClientCommand (edict_t *ent)
{
	//int client = getEntOffset(ent) - 1;
	
	if(!dllloaded) return;

	dllglobals->ClientCommand(ent);
	copyDllInfo();
}


void ServerCommand (void)
{
	char *cmd;

	if(!dllloaded) return;

	cmd = gi.argv(1);

	if(q2a_strlen(cmd) > 0) {
		if(cmd[0] == '!') {
			gi.dprintf("Q2A: Unknown command: %s\n", cmd);
			return;
		}
	}
		
	dllglobals->ServerCommand();
	copyDllInfo();
}
