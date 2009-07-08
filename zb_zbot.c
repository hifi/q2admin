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
	
	INITPERFORMANCE_2(1);
	INITPERFORMANCE_2(2);
	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->RunFrame();
			copyDllInfo();
			return;
		}
		
	STARTPERFORMANCE(1);
	
	lframenum++;
	ltime = lframenum * FRAMETIME;
	
	if(serverinfoenable && (lframenum > 10))
		{
			sprintf(buffer, "set Q2Admin \"" Q2ADMINVERSION "\" s\n");
			gi.AddCommandString(buffer);
			serverinfoenable = 0;
		}
		
	
	if(maxReconnectList)
		{
			unsigned int i;
			
			for(i = 0; i < maxReconnectList; i++)
				{
					if(reconnectlist[i].reconnecttimeout < ltime)
						{
							unsigned int j;
							
							// remove the retry list entry if needed...
							for(j = 0; j < maxReconnectList; j++)
								{
									if((j != i) && (reconnectlist[j].retrylistidx == reconnectlist[i].retrylistidx))
										{
											break;
										}
								}
								
							if(j >= maxReconnectList)
								{
									if((reconnectlist[i].retrylistidx + 1) < maxretryList)
										{
											q2a_memmove(&(retrylist[reconnectlist[i].retrylistidx]), &(retrylist[reconnectlist[i].retrylistidx + 1]), (maxretryList - (reconnectlist[i].retrylistidx + 1)) * sizeof(retrylist_info));
										}
									maxretryList--;
								}
								
							if((i + 1) < maxReconnectList)
								{
									q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
									i--;
								}
							maxReconnectList--;
						}
				}
		}
		
	if(framesperprocess && ((lframenum % framesperprocess) != 0))
		{
			dllglobals->RunFrame();
			copyDllInfo();
			return;
		}
		
	maxdoclients = client;
	maxdoclients += maxclientsperframe;

	if(maxdoclients > maxclients->value)
		{
			maxdoclients = maxclients->value;
		}
		
	for(; client < maxdoclients; client++)
		{
//*** UPDATE START ***
			if(client < 0)
			{
				ent = NULL;
			}
			else
			{
				ent = getEnt((client + 1));
			}


			if(getCommandFromQueue(client, &command, &data, &str))
				{		
					if(!proxyinfo[client].inuse)
					{
						if(command == QCMD_STARTUP)
						{
							addCmdQueue(client, QCMD_STARTUPTEST, 2, 0, 0);
							proxyinfo[client].clientcommand |= CCMD_STARTUPTEST;
						}
						else if((command == QCMD_DISCONNECT) || (command == QCMD_KICK))
						{
							//stuffcmd(ent, "disconnect\n");
							proxyinfo[client].clientcommand |= CCMD_KICKED;
							logEvent(LT_CLIENTKICK, client, ent, str, 0, 0.0);
							gi.cprintf (ent, PRINT_HIGH, "You have been kicked %s\n", proxyinfo[client].name);
							sprintf(buffer, "\nkick %d\n", client);
							gi.AddCommandString(buffer);
						}
					}
				else if(command == QCMD_DISPLOGFILE)
				{
					displayLogFileCont(ent, client, data);
				}
				else if(command == QCMD_DISPLOGFILELIST)
				{
					displayLogFileListCont(ent, client, data);
				}
				else if(command == QCMD_DISPLOGEVENTLIST)
				{
					displayLogEventListCont(ent, client, data, FALSE);
				}
				else if(command == QCMD_GETIPALT)
				{
					// open logfile and read IP address from log
					readIpFromLog(client, ent);
					addCmdQueue(client, QCMD_GETIPALT, 0, 0, 0);
				}
				else if(command == QCMD_LOGTOFILE1)
				{
					logEvent(LT_ZBOT, client, ent, NULL, proxyinfo[client].charindex, 0.0);
				}
				else if(command == QCMD_CONNECTCMD)
				{
					if(customClientCmdConnect[0])
						{
							sprintf(buffer, "%s\n", customClientCmdConnect);
							stuffcmd(ent, buffer);
						}

					if(customServerCmdConnect[0])
						{
							// copy string across to buffer, replacing %c with client number
							char *cp = customServerCmdConnect;
							char *dp = buffer;

							while(*cp)
								{
									if((*cp == '%') && (tolower(*(cp + 1)) == 'c'))
										{
											sprintf(dp, "%d", client);
											dp += q2a_strlen(dp);
											cp += 2;
										}
									else
										{
											*dp++ = *cp++;
										}
								}

							*dp = 0x0;

							gi.AddCommandString(buffer);
						}
				}
				else if(command == QCMD_CLEAR)
				{
					stuffcmd(ent, "clear\n");
				}
				else if(command == QCMD_CUSTOM)
				{
					if(customClientCmd[0])
						{
							sprintf(buffer, "%s\n", customClientCmd);
							stuffcmd(ent, buffer);
						}
				}
				else if((command == QCMD_DISCONNECT) || (command == QCMD_KICK))
				{
					//stuffcmd(ent, "disconnect\n");
					proxyinfo[client].clientcommand |= CCMD_KICKED;
					logEvent(LT_CLIENTKICK, client, ent, str, 0, 0.0);
					gi.cprintf (ent, PRINT_HIGH, "You have been kicked %s\n", proxyinfo[client].name);
					sprintf(buffer, "\nkick %d\n", client);
					gi.AddCommandString(buffer);
				}
				else if(command == QCMD_RECONNECT)
				{
					sprintf(buffer, "\nconnect %s\n", reconnect_address);
					stuffcmd(ent, buffer);
					//        addCmdQueue(client, QCMD_KICK, 0, 0, NULL);
				}
				else if(command == QCMD_CLIPTOMAXRATE)
				{
					sprintf(buffer, "rate %d\n", maxrateallowed);
					stuffcmd(ent, buffer);
				}
				else if(command == QCMD_CLIPTOMINRATE)
				{
					sprintf(buffer, "rate %d\n", minrateallowed);
					stuffcmd(ent, buffer);
				}
				else if(command == QCMD_SETUPMAXFPS)
				{
					stuffcmd(ent, "set cl_maxfps $cl_maxfps u\n");
					addCmdQueue(client, QCMD_FORCEUDATAUPDATE, 0, 0, 0);
				}
				else if(command == QCMD_FORCEUDATAUPDATE)
				{
					if(proxyinfo[client].rate)
						{
							sprintf(buffer, "set rate %d\nset rate %d\n", proxyinfo[client].rate + 1, proxyinfo[client].rate);
							stuffcmd(ent, buffer);
						}
				}
				else if(command == QCMD_SETMAXFPS)
				{
					if(maxfpsallowed)
						{
							sprintf(buffer, "cl_maxfps %d\n", maxfpsallowed);
							stuffcmd(ent, buffer);
						}
				}
				else if(command == QCMD_SETMINFPS)
				{
					if(minfpsallowed)
						{
							sprintf(buffer, "cl_maxfps %d\n", minfpsallowed);
							stuffcmd(ent, buffer);
						}
				}
				else if(command == QCMD_DISPBANS)
				{
					displayNextBan(ent, client, data);
				}
				else if(command == QCMD_DISPLRCONS)
				{
					displayNextLRCon(ent, client, data);
				}
				else if(command == QCMD_DISPFLOOD)
				{
					displayNextFlood(ent, client, data);
				}
				else if(command == QCMD_DISPSPAWN)
				{
					displayNextSpawn(ent, client, data);
				}
							
				else if(command == QCMD_DISPVOTE)
				{
					displayNextVote(ent, client, data);
				}
				else if(command == QCMD_DISPCHECKVAR)
				{
					displayNextCheckvar(ent, client, data);
				}
				else if(command == QCMD_CHECKVARTESTS)
				{
					checkVariableTest(ent, client, data);
				}
				else if(command == QCMD_CHANGENAME)
				{
					sprintf(buffer, "name \"%s\"\n", proxyinfo[client].name);
					stuffcmd(ent, buffer);
				}
				else if(command == QCMD_CHANGESKIN)
				{
					sprintf(buffer, "skin \"%s\"\n", proxyinfo[client].skin);
					stuffcmd(ent, buffer);
				}
				else if(command == QCMD_BAN)
				{
					gi.cprintf (NULL, PRINT_HIGH, "%s: %s\n", proxyinfo[client].name, proxyinfo[client].buffer);
					gi.cprintf (ent, PRINT_HIGH, "%s: %s\n", proxyinfo[client].name, proxyinfo[client].buffer);
					addCmdQueue(client, QCMD_DISCONNECT, 1, 0, proxyinfo[client].buffer);
				}
				else if(command == QCMD_DISPCHATBANS)
				{
					displayNextChatBan(ent, client, data);
				}
				else if(command == QCMD_STUFFCLIENT)
				{
					stuffNextLine(ent, client);
				}
				else if(command == QCMD_GETCMDQUEUE)
				{
					addCmdQueue(client, QCMD_TESTCMDQUEUE, 5, 0, 0);
					proxyinfo[client].cmdlist_timeout = ltime;
					proxyinfo[client].cmdlist_timeout += 5;
					proxyinfo[client].cmdlist = 1;
  					//1.20
					if (!proxyinfo[client].done_server_and_blocklist)
					{
						proxyinfo[client].blocklist = random()*(MAX_BLOCK_MODELS-1);
						sprintf(buffer,"p_blocklist %i\n",proxyinfo[client].blocklist);
						stuffcmd(ent,buffer); 
						generateRandomString(proxyinfo[client].serverip, 15);
						sprintf(buffer,"p_server %s\n",proxyinfo[client].serverip);
						stuffcmd(ent,buffer); 
						//q2ace responds with blahblah %i %s
					}
				}
				else if(command == QCMD_TESTCMDQUEUE)
				{
					if (proxyinfo[client].done_server_and_blocklist)
						required_cmdlist = 1;
					else
						required_cmdlist = 7;

					if (!proxyinfo[client].cmdlist)
					{
						proxyinfo[client].pcmd_noreply_count++;
						if (proxyinfo[client].pcmd_noreply_count > max_pmod_noreply)
						{
							gi.bprintf(PRINT_HIGH,MOD_KICK_MSG,proxyinfo[client].name,16);
							//sprintf(buffer,client_msg,version_check);
							//gi.cprintf(getEnt((client + 1)),PRINT_HIGH,"%s\n",buffer);
							addCmdQueue(client, QCMD_DISCONNECT, 1, 0, Q2A_MOD_KICK_MSG);
						}
					}
					else if (proxyinfo[client].cmdlist == required_cmdlist)
					{
						//all 3 checks came thru fine
						proxyinfo[client].done_server_and_blocklist = 1;
					}
					else
					{
						//just kick anyway
						//gi.bprintf(PRINT_HIGH,MOD_KICK_MSG,proxyinfo[client].name,proxyinfo[client].cmdlist);
						//sprintf(buffer,client_msg,version_check);
						//gi.cprintf(getEnt((client + 1)),PRINT_HIGH,"%s\n",buffer);
						addCmdQueue(client, QCMD_DISCONNECT, 1, 0, Q2A_MOD_KICK_MSG);
					}
				}
				else if(command == QCMD_EXECMAPCFG)
				{
					if (client_map_cfg & 1)
					{
						sprintf(buffer,"set map_name %s\n",gmapname);
						stuffcmd(ent,buffer);
					}
					else if (client_map_cfg & 2)
					{
						sprintf(buffer,"exec cfg/%s.cfg\n",gmapname);
						stuffcmd(ent,buffer);
					}
					else if (client_map_cfg & 4)
					{
						sprintf(buffer,"exec cfg/all.cfg\n");
						stuffcmd(ent,buffer);
					}
				}
			}
			else
			{
				if(maxdoclients < maxclients->value)
				{
					maxdoclients++;
				}
			}
		}
		
	//*** UPDATE START ***	
	if(client >= maxclients->value)
	{
		client = -1;
	}

	STARTPERFORMANCE(2);
	dllglobals->RunFrame();
	STOPPERFORMANCE_2(2, "mod->G_RunFrame", 0, NULL);

	copyDllInfo();

	STOPPERFORMANCE_2(1, "q2admin->G_RunFrame", 0, NULL);
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
	logEvent(LT_ENTITYCREATE, 0, NULL, *((char **)((unsigned long)ent + entity_classname_offset)), 0, 0.0);
	
	gi.linkentity(ent);
}

void unlinkentity_internal(edict_t *ent)
{
	logEvent(LT_ENTITYDELETE, 0, NULL, *((char **)((unsigned long)ent + entity_classname_offset)), 0, 0.0);
	
	gi.unlinkentity(ent);
}
