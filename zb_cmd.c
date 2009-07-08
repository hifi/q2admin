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
//#include "md4.c"

int    lastClientCmd = -1;

#define ZBOTCOMMANDSSIZE (sizeof(zbotCommands) / sizeof(zbotCommands[0]))

zbotcmd_t zbotCommands[] =
	{
		{
			"checkclientipaddress",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&checkClientIpAddress
		},
		{
			"customclientcmd",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			customClientCmd
		},
		{
			"customclientcmdconnect",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			customClientCmdConnect
		},
		{
			"customservercmd",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			customServerCmd
		},
		{
			"customservercmdconnect",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			customServerCmdConnect
		},
		{
			"defaultreconnectmessage",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			defaultreconnectmessage
		},
		{
			"disconnectuser",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&disconnectuser
		},
		{
			"displaynamechange",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&displaynamechange
		},
		{
			"dopversion",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&dopversion
		},
		{
			"entity_classname_offset",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&entity_classname_offset,
		},
		{
			"filternonprintabletext",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&filternonprintabletext
		},
		{
			"framesperprocess",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&framesperprocess
		},
		{
			"gamemaptomap",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&gamemaptomap
		},
		{
			"hackuserdisplay",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			hackuserdisplay
		},		
		{
			"mapcfgexec",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&mapcfgexec
		},
		{
			"maxclientsperframe",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&maxclientsperframe
		},
		{
			"numofdisplays",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&numofdisplays
		},
		{
			"printmessageonplaycmds",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&printmessageonplaycmds
		},
		{
			"quake2dirsupport",
			CMDWHERE_CFGFILE,
			CMDTYPE_LOGICAL,
			&quake2dirsupport
		},
		{
			"q2adminrunmode",
			CMDWHERE_CFGFILE,
			CMDTYPE_NUMBER,
			&q2adminrunmode
		},
		{
			"randomwaitreporttime",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&randomwaitreporttime
		},
		{
			"reconnect_address",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			reconnect_address
		},
		{
			"reconnect_checklevel",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&reconnect_checklevel
		},
		{
			"reconnect_time",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_NUMBER,
			&reconnect_time
		},
		{
			"serverinfoenable",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&serverinfoenable
		},
		{
			"skincrashmsg",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_STRING,
			skincrashmsg
		},
		{
			"soloadlazy",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE,
			CMDTYPE_LOGICAL,
			&soloadlazy
		},
		{
			"client_map_cfg", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&client_map_cfg
		},
		{ 
			"client_msg", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&client_msg
		},
		{ 
			"do_franck_check", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_LOGICAL,
			&do_franck_check
		},
		{ 
			"do_vid_restart", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_LOGICAL,
			&do_vid_restart
		},
		{ 
			"gl_driver_check", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&gl_driver_check
		},
		{ 
			"gl_driver_max_changes", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&gl_driver_max_changes
		},
		{ 
			"inverted_command1", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[4].command
		},
		{ 
			"inverted_command2", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[5].command
		},
		{ 
			"inverted_command3", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[6].command
		},
		{ 
			"inverted_command4", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[7].command
		},
		{
			"lanip", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			lanip
		},
		{ 
			"max_pmod_noreply", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&max_pmod_noreply
		},
		{ 
			"msec_int", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&msec_int
		},
		{ 
			"msec_kick_on_bad", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&msec_kick_on_bad
		},
		{ 
			"msec_max", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&msec_max
		},
		{ 
			"private_command1", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[0].command
		},
		{ 
			"private_command2", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[1].command
		},
		{ 
			"private_command3", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[2].command
		},
		{ 
			"private_command4", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&private_commands[3].command
		},
		{
			"private_command_kick",
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_LOGICAL,
			&private_command_kick
		},
		{ 
			"q2a_command_check", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_LOGICAL,
			&q2a_command_check
		},
		{
			"serverip", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			serverip
		},
		{ 
			"speedbot_check_type", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&speedbot_check_type
		},
		{ 
			"userinfochange_count", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&USERINFOCHANGE_COUNT
		},
		{ 
			"userinfochange_time", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_NUMBER,
			&USERINFOCHANGE_TIME
		},
/*		{ 
			"version_check", 
			CMDWHERE_CFGFILE | CMDWHERE_CLIENTCONSOLE | CMDWHERE_SERVERCONSOLE, 
			CMDTYPE_STRING,
			&version_check
		},*/
//*** UPDATE END ***
	};
    
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

        if (q2adminrunmode == 0 || !proxyinfo) {
                gi.dprintf("%s", cbuffer);
                return;
        }

        if (clienti == -1) {
                unsigned int i;

                if (maxclients) {
                        for (i = 0; i < maxclients->value; i++) {
                                if (proxyinfo[i].inuse
                                    && startContains(cbuffer,
                                                     proxyinfo[i].name)) {
                                        if (q2a_strstr
                                            (cbuffer,
                                             proxyinfo[i].lastcmd)) {

                                                clienti = i;
                                                break;
                                        }
                                }
                        }
                }
        } else if (((proxyinfo[clienti].inuse
                   && !q2a_strstr(cbuffer, proxyinfo[clienti].name))
                   || !q2a_strstr(cbuffer, proxyinfo[clienti].lastcmd))) {
                clienti = -1;
        }

	if(filternonprintabletext)
		{
			char *cp = cbuffer;
			
			while(*cp)
				{
					if(!isprint(*cp) && *(cp + 1) != 0)
						{
							*cp = ' ';
						}
						
					cp++;
				}
		}
		
	gi.dprintf("%s", cbuffer);
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
	
	if(q2adminrunmode == 0)
		{
			gi.cprintf(ent, printlevel, "%s", cbuffer);
			return;
		}
		
	cp = q2a_strstr(cbuffer, "swpplay ");
	if(cp)
		{
			// found a play_team command, play sound at client console instead.
			//r1ch 2005-01-26 fix fucking huge security hole BEGIN
			//stuffcmd(ent, cp + 3);
			//r1ch 2005-01-26 fix fucking huge security hole END
			return;
		}
		
	
	if(q2a_strcmp(mutedText, cbuffer)== 0)
		{
			return;
		}
		
	if(printlevel== PRINT_CHAT && clienti==-1)
		{
			unsigned int i;
			
			for(i= 0; i< maxclients->value; i++)
				{
					if(proxyinfo[i].inuse && startContains(cbuffer, proxyinfo[i].name))
						{
							if(q2a_strstr(cbuffer, proxyinfo[i].lastcmd))
								{
									clienti= i;
									break;
								}
						}
				}
		}
		
	if(printlevel== PRINT_CHAT && filternonprintabletext)
		{
			cp= cbuffer;
			
			while( *cp)
				{
					if(!isprint( *cp) && *(cp+ 1) != 0)
						{
							*cp=' ';
						}
						
					*cp++;
				}
		}
		
	gi.cprintf(ent, printlevel,"%s", cbuffer);
	
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
	
	if(q2adminrunmode == 0)
		{
			gi.bprintf(printlevel, "%s", cbuffer);
			return;
		}
		
	if(q2a_strcmp(mutedText, cbuffer) == 0)
		{
			return;
		}
		
	if(printlevel == PRINT_CHAT && clienti == -1)
		{
			unsigned int i;
			
			for(i = 0; i < maxclients->value; i++)
				{
					if(proxyinfo[i].inuse && startContains (cbuffer, proxyinfo[i].name))
						{
							if(q2a_strstr(cbuffer, proxyinfo[i].lastcmd))
								{
									clienti = i;
									break;
								}
						}
				}
		}
		
	if(printlevel == PRINT_CHAT)
		{
			if(filternonprintabletext)
				{
					char *cp = cbuffer;
					
					while(*cp)
						{
							if(!isprint(*cp) && *(cp + 1) != 0)
								{
									*cp = ' ';
								}
								
							*cp++;
						}
				}
		}
		
	gi.bprintf(printlevel, "%s", cbuffer);
	
}


void AddCommandString_internal(char *text)
{
	char *str;
	qboolean mapChangeFound = FALSE;
	
	if(q2adminrunmode == 0)
		{
			gi.AddCommandString(text);
			return;
		}
		
	if(gamemaptomap)
		{
			// check for gamemap in string.
			q2a_strcpy(buffer, text);
			q_strupr(buffer);
			
			str = q2a_strstr(buffer, "GAMEMAP");
			
			// double check the string is correct
			if(str && (str == buffer || *(str - 1) == ' ') && *(str + 7) == ' ')
				{
					// change to a map command
					if(str != buffer)
						{
							q2a_memcpy(buffer, text, str - buffer);
						}
						
					q2a_memcpy(str, text + ((str + 4) - buffer), q2a_strlen(text) - ((str + 4) - buffer) + 1);
					
					text = buffer;
				}
		}
		
	q2a_strcpy(buffer, text);
	q_strupr(buffer);
	
	str = q2a_strstr(buffer, "GAMEMAP");
	if(str && (str == buffer || *(str - 1) == ' ') && *(str + 7) == ' ')
		{
			// gamemap found, find map name
			str += 7;
			while(*str == ' ')
				{
					str++;
				}
				
			if(*str == '\"')
				{
					str++;
					str = text + (str - buffer);
					mapChangeFound = TRUE;
				}
		}
	else
		{
			str = q2a_strstr(buffer, "MAP");
			
			if(str && (str == buffer || *(str - 1) == ' ') && *(str + 3) == ' ')
				{
					// map found, find map name
					str += 3;
					while(*str == ' ')
						{
							str++;
						}
						
					if(*str == '\"')
						{
							str++;
							str = text + (str - buffer);
							mapChangeFound = TRUE;
						}
				}
		}
		
	if(mapChangeFound)
		{
			if ( mapcfgexec )
				{
					char *nameBuffer;
					
					q2a_strcpy(buffer, "exec mapcfg/");
					q2a_strcat(buffer, gmapname);
					q2a_strcat(buffer, "-end.cfg\n");
					gi.AddCommandString(buffer);
					
					q2a_strcpy(buffer, "exec ");
					
					nameBuffer = buffer + q2a_strlen(buffer);
					while(*str && *str != '\"')
						{
							*nameBuffer++ = *str++;
						}
						
					*nameBuffer = 0;
					
					q2a_strcat(buffer, "-pre.cfg\n");
					gi.AddCommandString(buffer);
				}
		}
		
	gi.AddCommandString(text);
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




void processCommand(int cmdidx, int startarg, edict_t *ent)
{
	if (gi.argc() > startarg)
		{
			switch(zbotCommands[cmdidx].cmdtype)
				{
				case CMDTYPE_LOGICAL:
					*((qboolean *)zbotCommands[cmdidx].datapoint) = getLogicalValue(gi.argv(startarg));
					break;
					
				case CMDTYPE_NUMBER:
					*((int *)zbotCommands[cmdidx].datapoint) = q2a_atoi(gi.argv(startarg));
					break;
					
				case CMDTYPE_STRING:
					processstring(zbotCommands[cmdidx].datapoint, gi.argv(startarg), 255, 0);
					break;
				}
		}
		
	switch(zbotCommands[cmdidx].cmdtype)
		{
		case CMDTYPE_LOGICAL:
			gi.cprintf (ent, PRINT_HIGH, "%s = %s\n", zbotCommands[cmdidx].cmdname, *((qboolean *)zbotCommands[cmdidx].datapoint) ? "Yes" : "No");
			break;
			
		case CMDTYPE_NUMBER:
			gi.cprintf (ent, PRINT_HIGH, "%s = %d\n", zbotCommands[cmdidx].cmdname, *((int *)zbotCommands[cmdidx].datapoint));
			break;
			
		case CMDTYPE_STRING:
			gi.cprintf (ent, PRINT_HIGH, "%s = %s\n", zbotCommands[cmdidx].cmdname, (char *)zbotCommands[cmdidx].datapoint);
			break;
		}
}



qboolean readCfgFile(char *cfgfilename)
{
	FILE *cfgfile;
	char buff1[256];
	char buff2[256];
	
	cfgfile = fopen(cfgfilename, "rt");
	if(!cfgfile) return FALSE;
	
	while(fgets(buffer, 256, cfgfile) != NULL)
		{
			char *cp = buffer;
			
			SKIPBLANK(cp);
			
			if(!(cp[0] == ';' || cp[0] == '\n' || isBlank (cp)))
				{
					if(breakLine(cp, buff1, buff2, sizeof(buff2) - 1))
						{
							unsigned int i;
							
							for(i = 0; i < ZBOTCOMMANDSSIZE; i++)
								{
									if((zbotCommands[i].cmdwhere & CMDWHERE_CFGFILE) &&  startContains (zbotCommands[i].cmdname, buff1))
										{
											if(zbotCommands[i].initfunc)
												{
													(*zbotCommands[i].initfunc)(buff2);
												}
											else switch(zbotCommands[i].cmdtype)
													{
													case CMDTYPE_LOGICAL:
														*((qboolean *)zbotCommands[i].datapoint) = getLogicalValue(buff2);
														break;
														
													case CMDTYPE_NUMBER:
														*((int *)zbotCommands[i].datapoint) = q2a_atoi(buff2);
														break;
														
													case CMDTYPE_STRING:
														q2a_strcpy(zbotCommands[i].datapoint, buff2);
														break;
													}
													
											break;
										}
								}
						}
				}
		}
		
	fclose(cfgfile);
	
	return TRUE;
}


void readCfgFiles(void)
{
	char cfgFile[100];
	qboolean ret;
	
	if(!q2admintxt || isBlank(q2admintxt->string))
		{
			q2a_strcpy(cfgFile, CFGFILE);
		}
	else
		{
			q2a_strcpy(cfgFile, q2admintxt->string);
		}
		
	ret = readCfgFile(cfgFile);
	
	sprintf(buffer, "%s/%s", moddir, cfgFile);
	if(readCfgFile(buffer))
		{
			ret = TRUE;
		}
		
	if(!ret)
		{
			gi.dprintf ("WARNING: " CFGFILE " could not be found\n");
		}
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
