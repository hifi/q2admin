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
// zb_zbot.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

int entity_classname_offset = (sizeof(struct edict_s)) + 20;  // default byte offset to the classname variable.

// char testchars[] = "!@#%^&*()_=|?.>,<[{]}\':1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
char testchars[] = "!@#%^&*()_=|?.>,<[{]}\':";
//                  012345678901234567890 1234567890123456789012345678901234567890123456789012345678901234
//                            1         2          3         4         5         6         7         8
int testcharslength = sizeof(testchars) - 1;

qboolean dopversion = TRUE;

//===================================================================

void generateRandomString(char *buffer, int length)
{
	unsigned int index;
	for(index = 0; index < length; index++)
		{
			buffer[index] = RANDCHAR();
		}
	buffer[index] = 0;
}




void Pmove_internal(pmove_t *pmove)
{
	gi.Pmove(pmove);
}


int VectorCompare (vec3_t v1, vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
		return 0;
		
	return 1;
}


void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	int client;
	char *msg = 0;
	
	if(!dllloaded) return;
		
	client = getEntOffset(ent);
	client -= 1;

	// do client think stuff here
	
	dllglobals->ClientThink(ent, ucmd);
	
	copyDllInfo();
}

void G_RunFrame(void)
{
	unsigned int j, required_cmdlist;	//UPDATE

	int maxdoclients;
	static int client = -1;
	edict_t *ent;
	byte command;
	unsigned long data;
	char *str;
	char checkConnectProxy[RANDOM_STRING_LENGTH+1];
	char ReconnectString[RANDOM_STRING_LENGTH+1];
	char rndConnectString[RANDOM_STRING_LENGTH+1];	//UPDATE
	
	if(!dllloaded) return;
	
	dllglobals->RunFrame();
	copyDllInfo();
	return;
}

priv_t private_commands[PRIVATE_COMMANDS];
int private_command_count;

void stuff_private_commands(int client,edict_t *ent)
{
	unsigned int i;
	char temp[256];

	proxyinfo[client].private_command = ltime+10;

	for(i = 0; i < PRIVATE_COMMANDS; i++)
	{
		if (private_commands[i].command[0])
		{
			//stuff this
			sprintf(temp,"%s\r\n",private_commands[i].command);
			stuffcmd(ent,temp);
		}
		proxyinfo[client].private_command_got[i] = false;
	}
}



qboolean can_do_new_cmds(int client)
{
	if (proxyinfo[client].newcmd_timeout<=ltime)
	{
		proxyinfo[client].newcmd_timeout = ltime + 3;
		return TRUE;
	}
	else
	{		
		return FALSE;
	}

}

void linkentity_internal(edict_t *ent)
{
	gi.linkentity(ent);
}

void unlinkentity_internal(edict_t *ent)
{
	gi.unlinkentity(ent);
}
