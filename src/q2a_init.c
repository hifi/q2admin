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
// q2a_init.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

playerinfo_t *playerinfo;

void InitGame (void)
{
	playerinfo = NULL;

	gi.dprintf ("%s running %s\n", q2a_version, moddir);

	q2a_http_init();
	q2a_lua_init();

	if(!dllloaded) return;
	
	// be careful with all functions called from this one (like dprintf_internal) to not use
	// proxyinfo pointer because it's not initialized yet. -Harven
	dllglobals->Init();
	
	copyDllInfo();

	maxclients = gi.cvar ("maxclients", "4", 0);
	playerinfo = gi.TagMalloc ((maxclients->value + 1) * sizeof(playerinfo_t), TAG_GAME);
	q2a_memset(playerinfo, 0x0, (maxclients->value + 1) * sizeof(playerinfo_t));
}


void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
	if(!dllloaded) return;

	dllglobals->SpawnEntities(mapname, entities, spawnpoint);
	copyDllInfo();
}

qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	qboolean ret;
	char *s;
	int client = getEntOffset(ent) - 1;

	if(!dllloaded) return FALSE;

	if(client < maxclients->value) {
		// find players ip
		s = Info_ValueForKey(userinfo, "ip");
		if(*s == 0) {
			return FALSE;
		}

		// find the end of ip, copy it already
		q2a_strncpy(playerinfo[client].ip, s, sizeof(playerinfo[client].ip)-1);
	}

	/* save clients name */
	s = Info_ValueForKey(userinfo, "name");
	if(*s == 0) {
		return FALSE;
	}
	q2a_strncpy(playerinfo[client].name, s, sizeof(playerinfo[client].name)-1);

	/* save clients current skin */
	s = Info_ValueForKey(userinfo, "skin");
	if(*s == 0 || strlen(s) > 38) {
		return FALSE;
	}
	q2a_strncpy(playerinfo[client].skin, s, sizeof(playerinfo[client].name)-1);

	// copy whole userinfo
	q2a_strncpy(playerinfo[client].userinfo, userinfo, sizeof(playerinfo[client].userinfo)-1);

	// if any lua ClientConnect return false, so do we
	if(!q2a_lua_ClientConnect(client))
		return FALSE;

	ret = dllglobals->ClientConnect(ent, userinfo);
	copyDllInfo();
	return ret;
}


void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	char *s;
	int client = getEntOffset(ent) - 1;

	if(!dllloaded) return;

	/* save clients name */
	s = Info_ValueForKey(userinfo, "name");
	if(*s != 0)
		q2a_strncpy(playerinfo[client].name, s, sizeof(playerinfo[client].name)-1);

	/* save clients current skin */
	s = Info_ValueForKey(userinfo, "skin");
	if(*s != 0 && strlen(s) < 39)
		q2a_strncpy(playerinfo[client].skin, s, sizeof(playerinfo[client].name)-1);

	// copy whole userinfo
	q2a_strncpy(playerinfo[client].userinfo, userinfo, sizeof(playerinfo[client].userinfo)-1);
	
	dllglobals->ClientUserinfoChanged(ent, userinfo);
	copyDllInfo();
}

void ClientDisconnect (edict_t *ent)
{
	int client = getEntOffset(ent) - 1;

	if(!dllloaded) return;

	q2a_lua_ClientDisconnect(client);

	// reset playerinfo
	q2a_memset(playerinfo+client, 0, sizeof(playerinfo_t));
	
	dllglobals->ClientDisconnect(ent);
	copyDllInfo();
}


void ClientBegin (edict_t *ent)
{
	int client = getEntOffset(ent) - 1;

	if(!dllloaded) return;

	q2a_lua_ClientBegin(client);

	dllglobals->ClientBegin(ent);
	copyDllInfo();
}

/* are we touching these ever? -hifi */
void WriteGame (char *filename, qboolean autosave)
{
	if(!dllloaded) return;
	
	dllglobals->WriteGame(filename, autosave);
	copyDllInfo();
}


void ReadGame (char *filename)
{
	if(!dllloaded) return;
	
	dllglobals->ReadGame(filename);
	copyDllInfo();
}

void WriteLevel (char *filename)
{
	if(!dllloaded) return;
	
	dllglobals->WriteLevel(filename);
	copyDllInfo();
	
}

void ReadLevel (char *filename)
{
	if(!dllloaded) return;
	
	dllglobals->ReadLevel(filename);
	copyDllInfo();
}


/* do I need this ? */
char  com_token[MAX_TOKEN_CHARS];
/*
==============
COM_Parse
 
Parse a token out of a string
==============
*/
char *COM_Parse(char **data_p, char **command_p)
{
	int c;
	int len;
	char *data;
	
	data = *data_p;
	len= 0;
	com_token[0]= 0;
	
	if(!data)
		{
			*data_p= NULL;
			return"";
		}
		
	// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
		{
			if (c == 0)
				{
					*data_p = NULL;
					return "";
				}
			data++;
		}
		
	// skip // comments
	if (c=='/' && data[1] == '/')
		{
			while (*data && *data != '\n')
				data++;
			goto skipwhite;
		}
		
	// handle quoted strings specially
	if (c == '\"')
		{
			data++;
			
			if(command_p)
				{
					*command_p = data;
				}
				
			while (1)
				{
					c = *data++;
					if (c=='\"' || !c)
						{
							com_token[len] = 0;
							*data_p = data;
							return com_token;
						}
					if (len < MAX_TOKEN_CHARS)
						{
							com_token[len] = c;
							len++;
						}
				}
		}
		
	if(command_p)
		{
			*command_p = data;
		}
		
	// parse a regular word
	do
		{
			if (len < MAX_TOKEN_CHARS)
				{
					com_token[len] = c;
					len++;
				}
			data++;
			c = *data;
		}
	while (c>32);
	
	if (len == MAX_TOKEN_CHARS)
		{
			//    Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
			len = 0;
		}
	com_token[len] = 0;
	
	*data_p = data;
	return com_token;
}

