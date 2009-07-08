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
// zb_cmd.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

int lastClientCmd = -1;

//===================================================================
char mutedText[8192] = "";

void dprintf_internal(char *fmt, ...)
{
        char cbuffer[8192];
        va_list arglist;
        int clienti = lastClientCmd;

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
	char *cp;
	int clienti = lastClientCmd;
	
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
	int clienti = lastClientCmd;
	
	// convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);
	
	gi.bprintf(printlevel, "%s", cbuffer);
	return;
}


void AddCommandString_internal(char *text)
{
	char *str;
	qboolean mapChangeFound = FALSE;
	
	gi.AddCommandString(text);
	return;
}


//===================================================================

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

int getClientsFromArg(int client, edict_t *ent, char *cp, char **text)
{
	int clienti;
	unsigned int like, maxi;
	char strbuffer[sizeof(buffer)];
	
	maxi = 0;
	
	if(startContains(cp, "LIKE"))
		{
			like = 1;
			
			cp += 5;
			SKIPBLANK(cp);
			
			if(*cp == '\"')
				{
					cp++;
					cp = processstring(strbuffer, cp, sizeof(strbuffer), '\"');
					cp++;
				}
			else
				{
					cp = processstring(strbuffer, cp, sizeof(strbuffer), ' ');
				}
			SKIPBLANK(cp);
		}
	else if(startContains(cp, "CL"))
		{
			like = 3;
			
			cp += 2;
			SKIPBLANK(cp);
			
			if(!isdigit(*cp))
				{
					return 0;
				}
				
			for(clienti = 0; clienti < maxclients->value; clienti++)
				{
					proxyinfo[clienti].clientcommand &= ~CCMD_SELECTED;
				}
				
			if(isdigit(*cp))
				{
					while(*cp)
						{
							clienti = q2a_atoi(cp);
							
							if(clienti >= 0 && clienti < maxclients->value && proxyinfo[clienti].inuse)
								{
									proxyinfo[clienti].clientcommand |= CCMD_SELECTED;
									maxi++;
								}
								
							while(isdigit(*cp))
								{
									cp++;
								}
								
							SKIPBLANK(cp);
							
							if(*cp && *cp != '+')
								{
									break;
								}
								
							if(*cp == '+')
								{
									cp++;
								}
								
							SKIPBLANK(cp);
							
							if(*cp && !isdigit(*cp))
								{
									break;
								}
						}
				}
				
			SKIPBLANK(cp);
		}
	else
		{
			like = 0;
			
			if(*cp == '\"')
				{
					cp++;
					cp = processstring(strbuffer, cp, sizeof(strbuffer), '\"');
					cp++;
				}
			else
				{
					cp = processstring(strbuffer, cp, sizeof(strbuffer), ' ');
				}
			SKIPBLANK(cp);
		}
		
		
		
		
	if(like < 3)
		{
			for(clienti = 0; clienti < maxclients->value; clienti++)
				{
					proxyinfo[clienti].clientcommand &= ~CCMD_SELECTED;
					
					//    if(clienti == client)
					//    {
					//      continue;
					//    }
					
					if(proxyinfo[clienti].inuse)
						{
							switch(like)
								{
								case 0:
									if(Q_stricmp(proxyinfo[clienti].name, strbuffer) == 0)
										{
											maxi++;
											proxyinfo[clienti].clientcommand |= CCMD_SELECTED;
										}
									break;
									
								case 1:
									if(stringContains(proxyinfo[clienti].name, strbuffer))
										{
											maxi++;
											proxyinfo[clienti].clientcommand |= CCMD_SELECTED;
										}
									break;
									
								case 2:
									// FIXME: regex removed
									break;
								}
						}
				}
		}
		
	if(maxi)
		{
			*text = cp;
			return maxi;
		}
	else
		{
			gi.cprintf(ent, PRINT_HIGH, "no player name matches found.\n");
		}
		
	return 0;
}



edict_t *getClientFromArg(int client, edict_t *ent, int *cleintret, char *cp, char **text)
{
	int clienti, foundclienti;
	unsigned int like;
	char strbuffer[sizeof(buffer)];
	
	foundclienti = -1;
	
	if(startContains(cp, "LIKE"))
		{
			like = 1;
			
			cp += 5;
			SKIPBLANK(cp);
			
			if(*cp == '\"')
				{
					cp++;
					cp = processstring(strbuffer, cp, sizeof(strbuffer), '\"');
					cp++;
				}
			else
				{
					cp = processstring(strbuffer, cp, sizeof(strbuffer), ' ');
				}
			SKIPBLANK(cp);
		}
	else if(startContains(cp, "CL"))
		{
			like = 3;
			
			cp += 2;
			SKIPBLANK(cp);
			
			if(!isdigit(*cp))
				{
					return NULL;
				}
				
			foundclienti = q2a_atoi(cp);
			
			while(isdigit(*cp))
				{
					cp++;
				}
				
			SKIPBLANK(cp);
			
			if(foundclienti < 0 || foundclienti > maxclients->value || !proxyinfo[foundclienti].inuse)
				{
					foundclienti = -1;
				}
		}
	else
		{
			like = 0;
			
			if(*cp == '\"')
				{
					cp++;
					cp = processstring(strbuffer, cp, sizeof(strbuffer), '\"');
					cp++;
				}
			else
				{
					cp = processstring(strbuffer, cp, sizeof(strbuffer), ' ');
				}
			SKIPBLANK(cp);
		}
		
		
	if(like < 3)
		{
			for(clienti = 0; clienti < maxclients->value; clienti++)
				{
					//      if(clienti == client)
					//      {
					//        continue;
					//      }
					if(proxyinfo[clienti].inuse)
						{
							switch(like)
								{
								case 0:
									if(Q_stricmp(proxyinfo[clienti].name, strbuffer) == 0)
										{
											if(foundclienti != -1)
												{
													gi.cprintf(ent, PRINT_HIGH, "2 or more player name matches.\n");
													return NULL;
												}
												
											foundclienti = clienti;
										}
									break;
									
								case 1:
									if(stringContains(proxyinfo[clienti].name, strbuffer))
										{
											if(foundclienti != -1)
												{
													gi.cprintf(ent, PRINT_HIGH, "2 or more player name matches.\n");
													return NULL;
												}
												
											foundclienti = clienti;
										}
									break;
									
								case 2:
									q2a_strcpy(strbuffer, proxyinfo[clienti].name);
									q_strupr(strbuffer);
									// FIXME: regex removed
									break;
								}
						}
				}
		}
		
	if(foundclienti != -1)
		{
			*text = cp;
			*cleintret = foundclienti;
			return getEnt((foundclienti + 1));
		}
	else
		{
			gi.cprintf(ent, PRINT_HIGH, "no player name matches found.\n");
		}
		
	return NULL;
}



qboolean sayPersonCmd(edict_t *ent, int client, char *args)
{
	char *cp = args, *text;
	edict_t *enti;
	int clienti;
	char tmptext[2100];
	
	SKIPBLANK(cp);
	
	enti = getClientFromArg(client, ent, &clienti, cp, &text);
	
	if(enti)
		{
			// make sure the text doesn't overflow the internal buffer...
			if(q2a_strlen(text) > 2000)
				{
					text[2000] = 0;
				}
				
			sprintf(tmptext, "(%s)(private message to: %s) %s\n", proxyinfo[client].name, proxyinfo[clienti].name, text);
			cprintf_internal(NULL, PRINT_CHAT, "%s", tmptext);
			cprintf_internal(ent, PRINT_CHAT, "%s", tmptext);
			
			sprintf(tmptext, "(%s)(private message) %s\n", proxyinfo[client].name, text);
			cprintf_internal(enti, PRINT_CHAT, "%s", tmptext);
			
			return FALSE;
		}
		
	return TRUE;
}

qboolean sayGroupCmd(edict_t *ent, int client, char *args)
{
	char *cp = args, *text;
	edict_t *enti;
	int clienti;
	char  tmptext[2100];
	int max;
	
	SKIPBLANK(cp);
	
	max = getClientsFromArg(client, ent, cp, &text);
	
	if(max)
		{
			// make sure the text doesn't overflow the internal buffer...
			if(q2a_strlen(text) > 2000)
				{
					text[2000] = 0;
				}
				
				
			for(clienti = 0; clienti < maxclients->value; clienti++)
				{
					if(proxyinfo[clienti].clientcommand & CCMD_SELECTED)
						{
							enti = getEnt((clienti + 1));
							
							sprintf(tmptext, "(%s)(private message to: %s) %s\n", proxyinfo[client].name, proxyinfo[clienti].name, text);
							cprintf_internal(NULL, PRINT_CHAT, "%s", tmptext);
							cprintf_internal(ent, PRINT_CHAT, "%s", tmptext);
							
							sprintf(tmptext, "(%s)(private message) %s\n", proxyinfo[client].name, text);
							cprintf_internal(enti, PRINT_CHAT, "%s", tmptext);
						}
				}
				
			return FALSE;
		}
		
	return TRUE;
}


void ClientCommand (edict_t *ent)
{
	int client = getEntOffset(ent) - 1;
	
	if(!dllloaded) return;
	
	
	dllglobals->ClientCommand(ent);
	copyDllInfo();
}


void ServerCommand (void)
{
	if(!dllloaded) return;
		
	dllglobals->ServerCommand();
	copyDllInfo();
}
