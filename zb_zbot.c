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

void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	//int client;
	
	if(!dllloaded) return;
		
	//client = getEntOffset(ent);
	//client -= 1;

	// do client think stuff here
	
	dllglobals->ClientThink(ent, ucmd);
	
	copyDllInfo();
}

void G_RunFrame(void)
{
	if(!dllloaded) return;
	
	dllglobals->RunFrame();
	copyDllInfo();
	return;
}
