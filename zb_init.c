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
// zb_init.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

game_import_t  gi;
game_export_t  globals;
game_export_t  *dllglobals;

cvar_t  *rcon_password, *gamedir, *maxclients, *logfile, *rconpassword, *port, *serverbindip, *q2admintxt;	// UPDATE

qboolean quake2dirsupport = TRUE;

char dllname[256];
char gmapname[MAX_QPATH];

//*** UPDATE START ***
int USERINFOCHANGE_TIME = 60;
int USERINFOCHANGE_COUNT = 40;
int client_map_cfg = 6;
int gl_driver_max_changes = 3;
int gl_driver_check = 0;
int max_pmod_noreply = 2;
int msec_int = 5;
int msec_kick_on_bad = 2;
int msec_max = 6000;
int speedbot_check_type = 3;

char client_msg[256];
char serverip[256] = { "" };
char lanip[256] = { "" };

qboolean do_franck_check = true;
qboolean q2a_command_check = false;
qboolean do_vid_restart = false;
qboolean private_command_kick = false;
//*** UPDATE END ***

qboolean dllloaded = FALSE;

qboolean mapcfgexec = FALSE;
qboolean checkClientIpAddress = TRUE;

qboolean disconnectuser = TRUE;
qboolean displayzbotuser = TRUE;
int numofdisplays = 4;
char zbotuserdisplay[256];
char hackuserdisplay[256];
char skincrashmsg[256];
char defaultreconnectmessage[256];
qboolean displaynamechange = TRUE;

char moddir[256];

qboolean extendedsay_enable = FALSE;
qboolean spawnentities_enable = FALSE;
qboolean spawnentities_internal_enable = FALSE;

qboolean gamemaptomap = FALSE;

qboolean lockDownServer = FALSE;

qboolean printmessageonplaycmds = TRUE;

int randomwaitreporttime = 55;

int maxMsgLevel = 3;

char *zbotversion = "Q2Admin Version " Q2ADMINVERSION "\n";
qboolean serverinfoenable = TRUE;

char motd[4096];

int maxrateallowed = 0;
int minrateallowed = 0;
int maxfpsallowed = 0;
int minfpsallowed = 0;

char buffer[0x10000];
char buffer2[256];

char adminpassword[256];

char customServerCmd[256];
char customClientCmd[256];
char customClientCmdConnect[256];
char customServerCmdConnect[256];

char reconnect_address[256] = { 0 };
int reconnect_time = 60;
int reconnect_checklevel = 0;

int logfilecheckcount;

proxyinfo_t *proxyinfo;
proxyinfo_t *proxyinfoBase;
proxyreconnectinfo_t *reconnectproxyinfo;



reconnect_info* reconnectlist;
retrylist_info* retrylist;
int maxReconnectList = 0;
int maxretryList = 0;



int lframenum;
float  ltime;


int proxy_bwproxy = 1;
int proxy_nitro2 = 1;


int q2adminrunmode = 100;
int maxclientsperframe = 100;
int framesperprocess = 0;


qboolean cl_pitchspeed_display = TRUE;
qboolean cl_pitchspeed_enable = FALSE;
qboolean cl_pitchspeed_kick = FALSE;
char cl_pitchspeed_kickmsg[256];

qboolean cl_anglespeedkey_display = TRUE;
qboolean cl_anglespeedkey_enable = FALSE;
qboolean cl_anglespeedkey_kick = FALSE;
char cl_anglespeedkey_kickmsg[256];

qboolean filternonprintabletext = FALSE;


char lockoutmsg[256];





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




char * FindIpAddressInUserInfo(char * userinfo, qboolean* userInfoOverflow)
{
	char *ip = Info_ValueForKey(userinfo, "ip");
	
	if ( userInfoOverflow )
		{
			*userInfoOverflow = false;
		}
		
	if ( *ip == 0 )
		{
			char * ipuserinfo = userinfo + q2a_strlen(userinfo);
			
			// find the last '\\'
			while(ipuserinfo > userinfo && *ipuserinfo != '\\')
				{
					ipuserinfo--;
				}
				
			if ( ipuserinfo - 3 >= userinfo &&
				 *(ipuserinfo - 3) == '\\' &&
				 *(ipuserinfo - 2) == 'i' &&
				 *(ipuserinfo - 1) == 'p')
				{
				
					if ( userInfoOverflow )
						{
							*userInfoOverflow = true;
						}
						
					return ipuserinfo + 1;
				}
		}
		
	return ip;
}






void InitGame (void)
{
	int i;
	
	proxyinfo = NULL; //UPDATE - Harven fix
	gi.dprintf (zbotversion);
	
	if(!dllloaded) return;
	
	if(q2adminrunmode < 100)
		{
			gi.dprintf ("(Q2Admin runlevel %d)\n", q2adminrunmode);
		}
		
	if(q2adminrunmode == 0)
		{
			dllglobals->Init();
			copyDllInfo();
			return;
		}
		
	dllglobals->Init(); //be carefull with all functions called from this one (like dprintf_internal) to not use proxyinfo pointer because it's not initialized yet. -Harven
	
	copyDllInfo();
	
	maxclients = gi.cvar ("maxclients", "4", 0);
	logfile = gi.cvar ("logfile", "0", 0);
	rconpassword = gi.cvar("rcon_password", "", 0);
	proxyinfoBase = gi.TagMalloc ((maxclients->value + 1) * sizeof(proxyinfo_t), TAG_GAME);
	q2a_memset(proxyinfoBase, 0x0, (maxclients->value + 1) * sizeof(proxyinfo_t));
	proxyinfo = proxyinfoBase;
	proxyinfo += 1;
	proxyinfo[-1].inuse = 1;
	
	reconnectproxyinfo = gi.TagMalloc (maxclients->value  * sizeof(proxyreconnectinfo_t), TAG_GAME);
	q2a_memset(reconnectproxyinfo, 0x0, maxclients->value * sizeof(proxyreconnectinfo_t));
	
	reconnectlist = (reconnect_info *)gi.TagMalloc (maxclients->value * sizeof(reconnect_info), TAG_GAME);
	maxReconnectList = 0;
	
	retrylist = (retrylist_info *)gi.TagMalloc (maxclients->value * sizeof(retrylist_info), TAG_GAME);
	maxretryList = 0;
	
	motd[0] = 0;
	
	for(i = -1; i < maxclients->value; i++)
		{
//*** UPDATE START ***
			proxyinfo[i].speedfreeze = 0;
			proxyinfo[i].msec_bad = 0;
			proxyinfo[i].enteredgame = 0;
			proxyinfo[i].msec_start = 0;
			proxyinfo[i].msec_count = 0;
			proxyinfo[i].msec_last = 0;
			proxyinfo[i].show_fps = false;
			proxyinfo[i].frames_count = 0;
			proxyinfo[i].q2a_admin = 0;
			proxyinfo[i].q2a_bypass = 0;
			proxyinfo[i].clientcommand = 0;
			proxyinfo[i].userinfo_changed_count = 0;
			proxyinfo[i].userinfo_changed_start = ltime;
			proxyinfo[i].pmod_noreply_count = 0;
			proxyinfo[i].pcmd_noreply_count = 0;
			proxyinfo[i].private_command = 0;
			proxyinfo[i].pmod = 0;
			proxyinfo[i].pver = 0;
			proxyinfo[i].done_server_and_blocklist = 0;
			proxyinfo[i].cmdlist = 0;
			proxyinfo[i].cmdlist_timeout = 0;
			proxyinfo[i].newcmd_timeout = 0;
			proxyinfo[i].pmodver = 0;
			proxyinfo[i].gl_driver[0] = 0;
			proxyinfo[i].gl_driver_changes = 0;
			proxyinfo[i].vid_restart = false;
			proxyinfo[i].userid = -1;
//*** UPDATE END ***
			proxyinfo[i].inuse = 0;
			proxyinfo[i].admin = 0;
			proxyinfo[i].clientcommand = 0;
			proxyinfo[i].stuffFile = 0;
			proxyinfo[i].retries = 0;
			proxyinfo[i].rbotretries = 0;
			proxyinfo[i].charindex = 0;
			proxyinfo[i].teststr[0] = 0;
			proxyinfo[i].cl_pitchspeed = 0;
			proxyinfo[i].cl_anglespeedkey = 0.0;
			proxyinfo[i].checked_hacked_exe = 0;
			
			removeClientCommands(i);
		}

}


void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
	int len, currentlen;
	FILE *motdptr;
	int i;
	char *backupentities = entities;
	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->SpawnEntities(mapname, backupentities, spawnpoint);
			copyDllInfo();
			return;
		}
		
		
	//  q2a_memset(proxyinfoBase, 0x0, (maxclients->value + 1) * sizeof(proxyinfo_t));
	
	for(i = -1; i < maxclients->value; i++)
		{
		
			if(i < 0 || proxyinfo[i].inuse == 0)
				{
//*** UPDATE START ***
					proxyinfo[i].speedfreeze = 0;
					proxyinfo[i].enteredgame = 0;
					proxyinfo[i].msec_bad = 0;
					proxyinfo[i].msec_start = 0;
					proxyinfo[i].frames_count = 0;
					proxyinfo[i].show_fps = false;
					proxyinfo[i].msec_last = 0;
					proxyinfo[i].msec_count = 0;
					proxyinfo[i].q2a_admin = 0;
					proxyinfo[i].q2a_bypass = 0;
//*** UPDATE END ***
					//proxyinfo[i].inuse = 0;
					proxyinfo[i].admin = 0;
					proxyinfo[i].clientcommand = 0;
					proxyinfo[i].stuffFile = 0;
				}
			else
				{
					proxyinfo[i].clientcommand &= (LEVELCHANGE_KEEP);
				}
				
//*** UPDATE START ***
			proxyinfo[i].userinfo_changed_count = 0;
			proxyinfo[i].userinfo_changed_start = ltime;
			proxyinfo[i].pcmd_noreply_count = 0;
			proxyinfo[i].pmod_noreply_count = 0;
			proxyinfo[i].private_command = 0;
			proxyinfo[i].pmod = 0;
			proxyinfo[i].pver = 0;
			proxyinfo[i].done_server_and_blocklist = 0;
			proxyinfo[i].cmdlist = 0;
			proxyinfo[i].newcmd_timeout = 0;
			proxyinfo[i].cmdlist_timeout = 0;
			proxyinfo[i].pmodver = 0;
			proxyinfo[i].gl_driver_changes = 0;
			proxyinfo[i].gl_driver[0] = 0;
//*** UPDATE END ***
			proxyinfo[i].rbotretries = 0;
			proxyinfo[i].retries = 0;
			proxyinfo[i].charindex = 0;
			proxyinfo[i].teststr[0] = 0;
			proxyinfo[i].cl_pitchspeed = 0;
			proxyinfo[i].cl_anglespeedkey = 0.0;
			proxyinfo[i].checked_hacked_exe = 0;
			removeClientCommands(i);
		}
		
	proxyinfo[-1].inuse = 1;

	motd[0] = 0;
	
	dllglobals->SpawnEntities(mapname, backupentities, spawnpoint);
	
	copyDllInfo();
	
	/* disabled until rewritten */
	//readCheckVarLists();
	
	// exec the map cfg file...
	q2a_strcpy(gmapname, mapname);	//UPDATE
	if ( mapcfgexec )
		{
			q2a_strcpy(gmapname, mapname);
			q2a_strcpy(buffer, "exec mapcfg/");
			q2a_strcat(buffer, mapname);
			q2a_strcat(buffer, "-post.cfg\n");
			gi.AddCommandString(buffer);
		}

//*** UPDATE START ***
	if (!*serverip)
	{
			gi.dprintf("You have not set a server ip.  Please add the following to q2admin.txt\nserverip \"ip\" where ip matches the outgoing one of the server.\n");
	}
//*** UPDATE END ***

}



qboolean UpdateInternalClientInfo(int client, edict_t *ent, char *userinfo, qboolean* userInfoOverflow)
{
	char *ip = FindIpAddressInUserInfo(userinfo, userInfoOverflow);
	
	if(*ip)
		{
			unsigned int i;
			int num;
			
			q2a_strcpy(proxyinfo[client].ipaddress, ip);
			
			if ( q2a_strcmp (ip, "loopback") == 0 )
				{
					proxyinfo[client].ipaddress[0] = 127;
					proxyinfo[client].ipaddress[1] = 0;
					proxyinfo[client].ipaddress[2] = 0;
					proxyinfo[client].ipaddress[3] = 1;
					
				}
			else
				{
					for(i = 0; i < 4; i++)
						{
							num = q2a_atoi(ip);
							
							if(num > 255 || num < 0)
								{
									// not a valid ip address
									proxyinfo[client].ipaddress[0] = 0;
									num = 0;
								}
								
							proxyinfo[client].ipaddressBinary[i] = num;
							
							while(isdigit(*ip))
								{
									ip++;
								}
								
							if(*ip == '.')
								{
									ip++;
								}
							else
								{
									if ( i < 3 || (*ip != ':' && *ip != 0) )
										{
											// not a valid ip address
											proxyinfo[client].ipaddress[0] = 0;
										}
									break;
								}
						}
				}
		}
		
	ip = Info_ValueForKey(userinfo, "Nitro2");
	
	if(*ip)
	{
		if(proxy_nitro2)
		{
			proxyinfo[client].clientcommand  |= CCMD_NITRO2PROXY;
		}
		else
		{
			return TRUE;
		}
	}
		
	ip = Info_ValueForKey(userinfo, "bwproxy");
	
	if(*ip)
	{
		if(proxy_bwproxy)
		{
			proxyinfo[client].clientcommand  |= CCMD_NITRO2PROXY;
		}
		else
		{
			return TRUE;
		}
	}
		
	return FALSE;
}



qboolean checkReconnectUserInfoSame(char *userinfo1, char *userinfo2)
{
	if(reconnect_checklevel)
		{
			char *cp1 = FindIpAddressInUserInfo(userinfo1, 0);
			char *cp2 = FindIpAddressInUserInfo(userinfo2, 0);
			char *bp = cp1;
			
			if ( *cp1 == 0 || *cp2 == 0 )
				{
					return FALSE;
				}
				
			while(*bp && *bp != ':')
				{
					bp++;
				}
				
			*bp = 0;
			
			bp = cp2;
			
			while(*bp && *bp != ':')
				{
					bp++;
				}
				
			*bp = 0;
			
			if(q2a_strcmp(cp1, cp2) != 0)
				{
					return FALSE;
				}
				
			cp1 = Info_ValueForKey(userinfo1, "name");
			cp2 = Info_ValueForKey(userinfo2, "name");
			
			if(q2a_strcmp(cp1, cp2) != 0)
				{
					return FALSE;
				}
				
			cp1 = Info_ValueForKey(userinfo1, "skin");
			cp2 = Info_ValueForKey(userinfo2, "skin");
			
			return (q2a_strcmp(cp1, cp2) == 0);
		}
		
	return (q2a_strcmp(userinfo1, userinfo2) == 0);
}



qboolean checkReconnectList(char *username)
{
	unsigned int i;
	
	for(i = 0; i < maxclients->value; i++)
		{
			if(reconnectproxyinfo[i].inuse && q2a_strcmp(reconnectproxyinfo[i].name, username) == 0)
				{
					reconnectproxyinfo[i].inuse = FALSE;
					return FALSE;
				}
		}
		
	return TRUE;
}

qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	int client;
	char *s;
	char *skinname;
	
	qboolean ret;
	qboolean userInfoOverflow = FALSE;
	
	if(!dllloaded) return FALSE;
	
	if(q2adminrunmode == 0)
		{
			ret = dllglobals->ClientConnect(ent, userinfo);
			copyDllInfo();
			return ret;
		}
		
	// allways clearout just in case there isn't any clients (therefore runframe doesn't get called)
	if(maxReconnectList)
		{
			unsigned int i;
			
			for(i = 0; i < maxReconnectList; i++)
				{
					if(reconnectlist[i].reconnecttimeout < ltime)
						{
							// wipe out the retry list if it's the only one pointing to it.
							unsigned int j;
							
							for(j = 0; j < maxReconnectList; j++)
								{
									if(j != i && reconnectlist[j].retrylistidx == reconnectlist[i].retrylistidx)
										{
											break;
										}
								}
								
							if(j >= maxReconnectList)
								{
									if(reconnectlist[i].retrylistidx + 1 < maxretryList)
										{
											q2a_memmove(&(retrylist[reconnectlist[i].retrylistidx]), &(retrylist[reconnectlist[i].retrylistidx + 1]), (maxretryList - (reconnectlist[i].retrylistidx + 1)) * sizeof(retrylist_info));
										}
									maxretryList--;
								}
								
							// wipe out the reconnect list entry
							if(i + 1 < maxReconnectList)
								{
									q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
									i--;
								}
							maxReconnectList--;
						}
				}
		}
		
	client = getEntOffset(ent) - 1;
	
		
//*** UPDATE START ***
	proxyinfo[client].private_command = 0;
	proxyinfo[client].pmod = 0;
	proxyinfo[client].userinfo_changed_count = 0;
	proxyinfo[client].userinfo_changed_start = ltime;
	proxyinfo[client].pcmd_noreply_count = 0;
	proxyinfo[client].pmod_noreply_count = 0;
	proxyinfo[client].pver = 0;
	proxyinfo[client].done_server_and_blocklist = 0;
	proxyinfo[client].cmdlist = 0;
	proxyinfo[client].newcmd_timeout = 0;
	proxyinfo[client].cmdlist_timeout = 0;
	proxyinfo[client].pmodver = 0;
	proxyinfo[client].gl_driver_changes = 0;
	proxyinfo[client].gl_driver[0] = 0;
	proxyinfo[client].speedfreeze = 0;
	proxyinfo[client].enteredgame = ltime;
	proxyinfo[client].msec_bad = 0;
	proxyinfo[client].msec_start = 0;
	proxyinfo[client].show_fps = false;
	proxyinfo[client].msec_last = 0;
	proxyinfo[client].msec_count = 0;
	proxyinfo[client].frames_count = 0;
	proxyinfo[client].q2a_admin = 0;
	proxyinfo[client].q2a_bypass = 0;
	proxyinfo[client].userid = -1;
	proxyinfo[client].vid_restart = false;
//*** UPDATE END ***
	proxyinfo[client].inuse = 0;
	proxyinfo[client].admin = 0;
	proxyinfo[client].clientcommand = 0;
	proxyinfo[client].retries = 0;
	proxyinfo[client].rbotretries = 0;
	proxyinfo[client].charindex = 0;
	proxyinfo[client].ipaddress[0] = 0;
	proxyinfo[client].name[0] = 0;
	proxyinfo[client].skin[0] = 0;
	proxyinfo[client].ipaddressBinary[0] = 0;
	proxyinfo[client].ipaddressBinary[1] = 0;
	proxyinfo[client].ipaddressBinary[2] = 0;
	proxyinfo[client].ipaddressBinary[3] = 0;
	proxyinfo[client].stuffFile = 0;
	removeClientCommands(client);
	
	ret = 1;
	
	if(client < maxclients->value)
		{
			if(UpdateInternalClientInfo(client, ent, userinfo, &userInfoOverflow))
				{
					sprintf(buffer, zbotuserdisplay, proxyinfo[client].name);
					
							proxyinfo[client].clientcommand |= CCMD_BANNED;
				}
		}
		
	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo))
		{
			q2a_strcpy (userinfo, "\\name\\badinfo\\skin\\male/grunt");
		}
		
	q2a_strcpy(proxyinfo[client].userinfo, userinfo);

	// set name
	s = Info_ValueForKey (userinfo, "name");
	if ( *s == 0 )
		{
			s = NULL; //UPDATE - 1.32e - 1.32e1 change
			return FALSE;
		}

	q2a_strncpy (proxyinfo[client].name, s, sizeof(proxyinfo[client].name)-1);
	
	skinname = Info_ValueForKey (userinfo, "skin");
	if ( *skinname == 0 )
		{
			return FALSE;
		}
		
	if (strlen(skinname) > 38)
		{
			gi.cprintf (NULL, PRINT_HIGH, "%s: Skin name exceeds 38 characters (IP = %s)\n", proxyinfo[client].name, proxyinfo[client].ipaddress);
			return FALSE;
		}
		
	q2a_strncpy (proxyinfo[client].skin, skinname, sizeof(proxyinfo[client].skin)-1);
	
	//   q2a_strcpy(ent->client->pers.netname, proxyinfo[client].name);
	q2a_strncpy (proxyinfo[client].userinfo, userinfo, sizeof(proxyinfo[client].userinfo) - 1);
	
	if(lockDownServer && checkReconnectList(proxyinfo[client].name))
		{
			
			
					proxyinfo[client].clientcommand |= CCMD_BANNED;
		}
	else if(checkClientIpAddress && proxyinfo[client].ipaddress[0] == 0) // check for invlaid IP's and don't let them in :)
		{
			char *ip = FindIpAddressInUserInfo(userinfo, 0);
			gi.cprintf (NULL, PRINT_HIGH, "%s: %s (%s)\n", proxyinfo[client].name, "Client doesn't have a valid IP address", ip);
			
					proxyinfo[client].clientcommand |= CCMD_BANNED;
					q2a_strcpy(proxyinfo[client].buffer, "Client doesn't have a valid IP address");
		}
	else if(ret && !(proxyinfo[client].clientcommand & CCMD_BANNED))
		{
			qboolean doConnect = TRUE;
			
			// is reconnect_address set?
			if(!isBlank(reconnect_address))
				{
					char *ip = FindIpAddressInUserInfo(userinfo, 0);
					char *bp = ip;
					unsigned int i;
					
					if ( *ip == 0 )
						{
							// force a reconnect and exit...
							doConnect = FALSE;
							proxyinfo[client].clientcommand |= CCMD_RECONNECT;
						}
					else
						{
							while(*bp && *bp != ':')
								{
									bp++;
								}
								
							*bp = 0;
							
							
							// check to see if they are in the reconnect list?
							for(i = 0; i < maxReconnectList; i++)
								{
									if(checkReconnectUserInfoSame(userinfo, reconnectlist[i].userinfo))
										{
											// found a match...
											break;
										}
									else
										{
											// check if the same IP and delete... this stops proxies from reconnecting...
											char *reconnectip = FindIpAddressInUserInfo(reconnectlist[i].userinfo, 0);
											bp = reconnectip;
											
											if ( *reconnectip )
												{
													while(*bp && *bp != ':')
														{
															bp++;
														}
														
													*bp = 0;
													
													if(strcmp(ip, reconnectip) == 0)
														{
															// remove from list and continue with connect
															if(i + 1 < maxReconnectList)
																{
																	q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
																	i--;
																}
															maxReconnectList--;
														}
												}
										}
								}
								
							if(i < maxReconnectList)
								{
									unsigned int j;
									
									// remove the retry list entry if needed...
									for(j = 0; j < maxReconnectList; j++)
										{
											if(j != i && reconnectlist[j].retrylistidx == reconnectlist[i].retrylistidx)
												{
													break;
												}
										}
										
									if(j >= maxReconnectList)
										{
											if(reconnectlist[i].retrylistidx + 1 < maxretryList)
												{
													q2a_memmove(&(retrylist[reconnectlist[i].retrylistidx]), &(retrylist[reconnectlist[i].retrylistidx + 1]), (maxretryList - (reconnectlist[i].retrylistidx + 1)) * sizeof(retrylist_info));
												}
											maxretryList--;
										}
										
									// remove from list and continue with connect
									if(i + 1 < maxReconnectList)
										{
											q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
										}
									maxReconnectList--;
								}
							else
								{
									// force a reconnect and exit...
									
									doConnect = FALSE;
									proxyinfo[client].clientcommand |= CCMD_RECONNECT;
								}
						}
				}
				
				
			if(doConnect)
				{
					ret = dllglobals->ClientConnect(ent, userinfo);
					copyDllInfo();
				}
		}
		
	if(ret)
		{
			
			if ( userInfoOverflow )
				{
					gi.cprintf (NULL, PRINT_HIGH, "%s: %s (%s)\n", proxyinfo[client].name, "WARNING: Client's userinfo space looks to have overflowed!", proxyinfo[client].ipaddress);
					proxyinfo[client].clientcommand |= CCMD_CLIENTOVERFLOWED;
				}
		}
		
	return ret;
}


qboolean checkForNameChange(int client, edict_t *ent, char *userinfo)
{
	char *s = Info_ValueForKey (userinfo, "name");
	char oldname[sizeof(proxyinfo[client].name)];
	char newname[sizeof(proxyinfo[client].name)];
	
	q2a_strncpy (newname, s, sizeof(newname) - 1);
	newname[sizeof(newname) - 1] = 0;
	
	if(proxyinfo[client].name[0] == 0)
		{
			q2a_strcpy (proxyinfo[client].name, newname);
		}
	else if(q2a_strcmp(proxyinfo[client].name, newname) != 0)
		{
				
			q2a_strcpy(oldname, proxyinfo[client].name);
			q2a_strcpy (proxyinfo[client].name, newname);
			

					
					if(displaynamechange)
						{
							gi.bprintf (PRINT_HIGH, "%s changed name to %s\n", oldname, proxyinfo[client].name);
						}
						
		}
		
	return TRUE;
}



qboolean checkForSkinChange(int client, edict_t *ent, char *userinfo)
{
	char *s = Info_ValueForKey (userinfo, "skin");
	char oldskin[sizeof(proxyinfo[client].skin)];
	char newskin[sizeof(proxyinfo[client].skin)];
	char *skinname;
	
	q2a_strncpy (newskin, s, sizeof(newskin) - 1);
	newskin[sizeof(newskin) - 1] = 0;
	
	if(proxyinfo[client].skin[0] == 0)
		{
			q2a_strcpy (proxyinfo[client].skin, newskin);
		}
	else if(q2a_strcmp(proxyinfo[client].skin, newskin) != 0)
		{
				
			q2a_strcpy(oldskin, proxyinfo[client].skin);
			q2a_strcpy (proxyinfo[client].skin, newskin);
			
			
		}
		
	skinname = Info_ValueForKey (userinfo, "skin");
	if (strlen(skinname) > 38)
		{
			sprintf(buffer, skincrashmsg, proxyinfo[client].name);
			gi.bprintf (PRINT_HIGH, "%s\n", buffer);
			addCmdQueue(client, QCMD_DISCONNECT, 0, 0, skincrashmsg);
			return FALSE;
		}
		
	return TRUE;
}


void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	int client;
	qboolean passon;

//*** UPDATE START ***
	char *s = Info_ValueForKey (userinfo, "name");
	char tmptext[128];
	char *cl_max_temp;
	int temp;

//	cvar_t *srv_ip;
//*** UPDATE END ***
	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->ClientUserinfoChanged(ent, userinfo);
			copyDllInfo();
			return;
		}
		
	client = getEntOffset(ent) - 1;


	if (stringContains(userinfo, "\\skon\\"))	//zgh_frk check
	{
		gi.bprintf(PRINT_HIGH,"%s was caught cheating!\n",proxyinfo[client].name);
		sprintf(tmptext, "kick %d\n", client);
		gi.AddCommandString(tmptext);
	}

	proxyinfo[client].userinfo_changed_count++;
	if (proxyinfo[client].userinfo_changed_count>USERINFOCHANGE_COUNT)
	{
		temp = ltime - proxyinfo[client].userinfo_changed_start;
			//enuf time passed, reset count
			proxyinfo[client].userinfo_changed_count = 0;
			proxyinfo[client].userinfo_changed_start = ltime;
	}
	
	passon = checkForNameChange(client, ent, userinfo);
	if(!checkForSkinChange(client, ent, userinfo))
		{
			passon = FALSE;
		}
		
	if(passon && !(proxyinfo[client].clientcommand & BANCHECK))
		{
			dllglobals->ClientUserinfoChanged(ent, userinfo);
			
			copyDllInfo();
		}
		
		
	proxyinfo[client].msg = q2a_atoi(Info_ValueForKey(userinfo, "msg"));
	
	if(proxyinfo[client].msg > maxMsgLevel)
		{
			addCmdQueue(client, QCMD_MSGDISCONNECT, 2, 0, 0);
		}
		
	q2a_strcpy(proxyinfo[client].userinfo, userinfo);
	
}

void ClientDisconnect (edict_t *ent)
{
	int client;
//	int clienti;	//UPDATE
//	qboolean empty;	//UPDATE

	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->ClientDisconnect(ent);
			copyDllInfo();
			return;
		}
		
	
	client = getEntOffset(ent) - 1;
	
	if(client >= maxclients->value) return;
	
	if(!(proxyinfo[client].clientcommand & BANCHECK))
		{
			dllglobals->ClientDisconnect(ent);
			
			copyDllInfo();
			
			// setup the reconnect info if required.
			if(lockDownServer)
				{
					unsigned int i;
					
					for(i = 0; i < maxclients->value; i++)
						{
							if(!reconnectproxyinfo[i].inuse)
								{
									q2a_strcpy(reconnectproxyinfo[i].name, proxyinfo[client].name);
									reconnectproxyinfo[i].inuse = TRUE;
									break;
								}
						}
				}
		}
		
		
	if(proxyinfo[client].stuffFile)
		{
			fclose(proxyinfo[client].stuffFile);
		}

	proxyinfo[client].inuse = 0;
	proxyinfo[client].admin = 0;
	proxyinfo[client].retries = 0;
	proxyinfo[client].rbotretries = 0;
	proxyinfo[client].clientcommand = 0;
	proxyinfo[client].charindex = 0;
	proxyinfo[client].ipaddress[0] = 0;
	proxyinfo[client].name[0] = 0;
	proxyinfo[client].skin[0] = 0;
	proxyinfo[client].stuffFile = 0;
	proxyinfo[client].checked_hacked_exe = 0;
	removeClientCommands(client);

//*** UPDATE START ***
	proxyinfo[client].userinfo_changed_count = 0;
	proxyinfo[client].userinfo_changed_start = ltime;
	proxyinfo[client].pcmd_noreply_count = 0;
	proxyinfo[client].pmod_noreply_count = 0;
	proxyinfo[client].private_command = 0;
	proxyinfo[client].pmod = 0;
	proxyinfo[client].pver = 0;
	proxyinfo[client].done_server_and_blocklist = 0;
	proxyinfo[client].cmdlist = 0;
	proxyinfo[client].newcmd_timeout = 0;
	proxyinfo[client].cmdlist_timeout = 0;
	proxyinfo[client].pmodver = 0;
	proxyinfo[client].gl_driver_changes = 0;
	proxyinfo[client].gl_driver[0] = 0;
	proxyinfo[client].speedfreeze = 0;
	proxyinfo[client].enteredgame = 0;
	proxyinfo[client].msec_bad = 0;
	proxyinfo[client].msec_start = 0;
	proxyinfo[client].frames_count = 0;
	proxyinfo[client].show_fps = false;
	proxyinfo[client].msec_last = 0;
	proxyinfo[client].msec_count = 0;
	proxyinfo[client].q2a_admin = 0;
	proxyinfo[client].q2a_bypass = 0;
	proxyinfo[client].vid_restart = false;
	proxyinfo[client].userid = -1;

}


void ClientBegin (edict_t *ent)
{
	int client;
	FILE *q2logfile;
	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->ClientBegin(ent);
			copyDllInfo();
			return;
		}
		
	client = getEntOffset(ent) - 1;
	
	if(!(proxyinfo[client].clientcommand & BANCHECK))
		{
			dllglobals->ClientBegin(ent);
			copyDllInfo();
		}
	else
		{
			// setup ent to be valid...
			ent->client->ps.fov = 10;
		}
		
	if(client >= maxclients->value)
		{
			return;
		}
		
	if(!proxyinfo[client].inuse)
		{
			proxyinfo[client].admin = 0;

//*** UPDATE START ***
			proxyinfo[client].speedfreeze = 0;
			proxyinfo[client].enteredgame = ltime;
			proxyinfo[client].msec_bad = 0;
			proxyinfo[client].msec_start = 0;
			proxyinfo[client].frames_count = 0;
			proxyinfo[client].show_fps = false;
			proxyinfo[client].msec_last = 0;
			proxyinfo[client].msec_count = 0;
			proxyinfo[client].q2a_admin = 0;
			proxyinfo[client].q2a_bypass = 0;
		}

	proxyinfo[client].userinfo_changed_count = 0;
	proxyinfo[client].userinfo_changed_start = ltime;
	proxyinfo[client].pcmd_noreply_count = 0;
	proxyinfo[client].pmod_noreply_count = 0;
	proxyinfo[client].private_command = 0;
	proxyinfo[client].pmod = 0;
	proxyinfo[client].pver = 0;
	proxyinfo[client].done_server_and_blocklist = 0;
	proxyinfo[client].cmdlist = 0;
	proxyinfo[client].newcmd_timeout = 0;
	proxyinfo[client].cmdlist_timeout = 0;
	proxyinfo[client].pmodver = 0;
	proxyinfo[client].gl_driver_changes = 0;
	proxyinfo[client].gl_driver[0] = 0;
//*** UPDATE END ***

	proxyinfo[client].inuse = 0;
	proxyinfo[client].retries = 0;
	proxyinfo[client].rbotretries = 0;
	proxyinfo[client].charindex = 0;
	proxyinfo[client].teststr[0] = 0;

	if(proxyinfo[client].clientcommand & CCMD_RECONNECT)
		{
			addCmdQueue(client, QCMD_RECONNECT, 1, 0, NULL);
		}
	else if(proxyinfo[client].clientcommand & CCMD_BANNED)
		{
			gi.cprintf(ent, PRINT_HIGH, "%s\n", proxyinfo[client].buffer);
			addCmdQueue(client, QCMD_DISCONNECT, 1, 0, proxyinfo[client].buffer);
		}
	else if(proxyinfo[client].clientcommand & CCMD_KICKED)
		{
			addCmdQueue(client, QCMD_DISCONNECT, 1, 0, "Kicked."); //UPDATE
		}
	else
		{
			addCmdQueue(client, QCMD_STARTUP, 0, 0, 0);
			
			if(customClientCmdConnect[0] || customServerCmdConnect[0])
				{
					addCmdQueue(client, QCMD_CONNECTCMD, 0, 0, 0);
				}
				
			if (proxyinfo[client].clientcommand & CCMD_CLIENTOVERFLOWED)
				{
					gi.cprintf(ent, PRINT_HIGH, "WARNING: Your userinfo looks to have overflowed. This may cause you problems during gameplay. Restart quake2 to clear your userinfo space.\n");
				}
				
			/* disabled until rewritten */
			//addCmdQueue(client, QCMD_CHECKVARTESTS, (float)checkvar_poll_time, 0, 0);
			
			sprintf(buffer, "%s/qconsole.log", moddir);
			q2logfile = fopen(buffer, "rt");
			if(q2logfile)
				{
					fseek(q2logfile, 0, SEEK_END);
					proxyinfo[client].logfilecheckpos = ftell(q2logfile);
					fclose(q2logfile);
				}
		}
		
}

void WriteGame (char *filename, qboolean autosave)
{
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->WriteGame(filename, autosave);
			copyDllInfo();
			return;
		}
		
	dllglobals->WriteGame(filename, autosave);
	copyDllInfo();
	
}


void ReadGame (char *filename)
{
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->ReadGame(filename);
			copyDllInfo();
			return;
		}
		
	
	dllglobals->ReadGame(filename);
	copyDllInfo();
	
}

void WriteLevel (char *filename)
{
	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->WriteLevel(filename);
			copyDllInfo();
			return;
		}
		
	
	dllglobals->WriteLevel(filename);
	copyDllInfo();
	
}

void ReadLevel (char *filename)
{
	
	if(!dllloaded) return;
	
	if(q2adminrunmode == 0)
		{
			dllglobals->ReadLevel(filename);
			copyDllInfo();
			return;
		}
		
	
	dllglobals->ReadLevel(filename);
	copyDllInfo();
	
}

