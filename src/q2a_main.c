//
// q2admin
//
// g_main.c
//
// copyright 2000 Shane Powell
// copyright 2009 Toni Spets
//

#include "g_local.h"

#ifdef __GNUC__
#include <dlfcn.h>
#elif defined(WIN32)
#include <windows.h>
#endif

#ifdef __GNUC__
void *hdll = NULL;

#if defined (__x86_64__)
	#define DLLNAME "gamex86_64.real.so"
#elif defined (__i386__)
	#define DLLNAME "gamei386.real.so"
#else
	#error Unknown GNUC OS
#endif

#elif defined(WIN32)
HINSTANCE hdll;
#define DLLNAME   "gamex86.dll"
#define DLLNAMEMODDIR "gamex86.real.dll"
#else
#error Unknown OS
#endif

typedef game_export_t  *GAMEAPI (game_import_t *import);

qboolean soloadlazy;

game_import_t  gi;
game_export_t  globals;
game_export_t  *dllglobals;

cvar_t *gamedir, *q2a_config, *maxclients, *gamename;

qboolean quake2dirsupport = TRUE;

qboolean dllloaded = FALSE;

char dllname[256];
char moddir[256];

void Init (void)
{
	if(!dllloaded) return;
	
	dllglobals->Init();
	
	copyDllInfo();

	gi.dprintf ("Q2Admin %s running %s\n", Q2A_VERSION, moddir);

	q2a_config = gi.cvar ("q2a_config", "config.lua", 0);
	gi.cvar ("*Q2Admin", Q2A_VERSION, CVAR_SERVERINFO|CVAR_NOSET);

	q2a_lua_init();
}

void Shutdown (void)
{
	q2a_lua_shutdown();

	if(!dllloaded) return;

	dllglobals->Shutdown();
	
#ifdef __GNUC__
	dlclose(hdll);
#elif defined(WIN32)
	FreeLibrary(hdll);
#endif
	
	dllloaded = FALSE;
}

void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
	if(!dllloaded) return;

	q2a_lua_SpawnEntities(mapname, entities, spawnpoint);

	dllglobals->SpawnEntities(mapname, entities, spawnpoint);
	copyDllInfo();
}

qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	qboolean ret;
	int client = getClientOffset(ent);

	if(!dllloaded) return FALSE;

	// if any lua ClientConnect return false, so do we
	if(!q2a_lua_ClientConnect(client, userinfo))
		return FALSE;

	ret = dllglobals->ClientConnect(ent, userinfo);
	copyDllInfo();
	return ret;
}

void ClientBegin (edict_t *ent)
{
	int client = getClientOffset(ent);

	if(!dllloaded) return;

	q2a_lua_ClientBegin(client);

	dllglobals->ClientBegin(ent);
	copyDllInfo();
}

void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	int client = getClientOffset(ent);

	if(!dllloaded) return;

	q2a_lua_ClientUserinfoChanged(client, userinfo);

	dllglobals->ClientUserinfoChanged(ent, userinfo);
	copyDllInfo();
}

void ClientDisconnect (edict_t *ent)
{
	int client = getClientOffset(ent);

	if(!dllloaded) return;

	q2a_lua_ClientDisconnect(client);

	dllglobals->ClientDisconnect(ent);
	copyDllInfo();
}

void ClientCommand (edict_t *ent)
{
	int client = getClientOffset(ent);
	
	if(!dllloaded) return;

	if(q2a_lua_ClientCommand(client))
		return;

	dllglobals->ClientCommand(ent);
	copyDllInfo();
}

void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	int client = getClientOffset(ent);
	
	if(!dllloaded) return;
		
	q2a_lua_ClientThink(client);
	
	dllglobals->ClientThink(ent, ucmd);
	
	copyDllInfo();
}

void RunFrame(void)
{
	if(!dllloaded) return;

	q2a_lua_RunFrame();

	dllglobals->RunFrame();
	copyDllInfo();
}

void ServerCommand (void)
{
	char *cmd;

	if(!dllloaded) return;

	cmd = gi.argv(1);

	if(!q2a_strcmp(cmd, "lua_reload")) {
		q2a_lua_shutdown();
		q2a_lua_init();
		return;
	}

	if(q2a_lua_ServerCommand(cmd))
		return;
		
	dllglobals->ServerCommand();
	copyDllInfo();
}

/*
=================
GetGameAPI
 
Returns a pointer to the structure with all entry points
and global variables
=================
*/
game_export_t *GetGameAPI(game_import_t *import)
{
	GAMEAPI *getapi;
#ifdef __GNUC__
	int loadtype;
#endif

	dllloaded = FALSE;
	gi = *import;
	
	globals.Init = Init;
	globals.Shutdown = Shutdown;
	globals.SpawnEntities = SpawnEntities;
	
	globals.ClientThink = ClientThink;
	globals.ClientConnect = ClientConnect;
	globals.ClientUserinfoChanged = ClientUserinfoChanged;
	globals.ClientDisconnect = ClientDisconnect;
	globals.ClientBegin = ClientBegin;
	globals.ClientCommand = ClientCommand;
	
	globals.RunFrame = RunFrame;
	
	globals.ServerCommand = ServerCommand;
	
	gamedir = gi.cvar ("game", "baseq2", 0);
	q2a_strcpy(moddir, gamedir->string);
	
	if(moddir[0] == 0)
		q2a_strcpy(moddir, "baseq2");

#ifdef __GNUC__
	loadtype = soloadlazy ? RTLD_LAZY : RTLD_NOW;
	sprintf(dllname, "%s/%s", moddir, DLLNAME);
	hdll = dlopen(dllname, loadtype);
#elif defined(WIN32)
	if(quake2dirsupport)
		sprintf(dllname, "%s/%s", moddir, DLLNAME);
	else
		sprintf(dllname, "%s/%s", moddir, DLLNAMEMODDIR);
	
	hdll = LoadLibrary(dllname);
#endif
	
	if(hdll == NULL)
	{
		// try the baseq2 directory...
		sprintf(dllname, "baseq2/%s", DLLNAME);
		
#ifdef __GNUC__
		hdll = dlopen(dllname, loadtype);
#elif defined(WIN32)
		hdll = LoadLibrary(dllname);
#endif
		
#ifdef __GNUC__
		sprintf(dllname, "%s/%s", moddir, DLLNAME);
#elif defined(WIN32)
		if(quake2dirsupport)
			sprintf(dllname, "%s/%s", moddir, DLLNAME);
		else
			sprintf(dllname, "%s/%s", moddir, DLLNAMEMODDIR);
#endif
		
		if(hdll == NULL)
		{
			gi.dprintf ("Unable to load DLL %s.\n", dllname);
			return &globals;
		} else {
			gi.dprintf ("Unable to load DLL %s, loading baseq2 DLL.\n", dllname);
		}
	}

	gi.dprintf("Q2A: Loaded game DLL: %s.\n", dllname);
		
#ifdef __GNUC__
	getapi = (GAMEAPI *)dlsym(hdll, "GetGameAPI");
#elif defined(WIN32)
	getapi = (GAMEAPI *)GetProcAddress (hdll, "GetGameAPI");
#endif
	
	if(getapi == NULL)
	{
#ifdef __GNUC__
		dlclose(hdll);
#elif defined(WIN32)
		FreeLibrary(hdll);
#endif
		
		gi.dprintf ("No \"GetGameApi\" entry in DLL %s.\n", dllname);
		return &globals;
	}
		
	dllglobals = (*getapi)(import);
	dllloaded = TRUE;
	copyDllInfo();
	import->cprintf = gi.cprintf;
	
	return &globals;
}
