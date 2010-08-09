//
// q2admin
//
// g_main.c
//
// copyright 2000 Shane Powell
// copyright 2009 Toni Spets
//

#include "g_local.h"

#ifdef _WIN32

#include <windows.h>
HINSTANCE hdll;
#define DLLNAME		"gamex86.real.dll"

#else // if not win32

#include <dlfcn.h>
void *hdll = NULL;

#if defined (__x86_64__)
#define DLLNAME		"gamex86_64.real.so"
#elif defined (__i386__)
#define DLLNAME		"gamei386.real.so"
#else
#error Unknown arch
#endif

#endif

typedef game_export_t  *GAMEAPI (game_import_t *import);

game_import_t  gi;
game_export_t  globals;
game_export_t  *dllglobals;

char moddir[256];

void Init (void)
{
	if(hdll == NULL) return;

	gi.dprintf ("Q2Admin %s running %s\n", Q2A_VERSION, moddir);

	gi.cvar ("*Q2Admin", Q2A_VERSION, CVAR_SERVERINFO|CVAR_NOSET);

	q2a_lua_init();

	dllglobals->Init();
	copyDllInfo();
}

void Shutdown (void)
{
	q2a_lua_shutdown();

	if(hdll == NULL) return;

	dllglobals->Shutdown();

#ifdef _WIN32
	FreeLibrary(hdll);
#else
	dlclose(hdll);
#endif

	hdll = NULL;
}

void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
	if(hdll == NULL) return;

	q2a_lua_SpawnEntities(mapname, entities, spawnpoint);

	dllglobals->SpawnEntities(mapname, entities, spawnpoint);
	copyDllInfo();
}

qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	qboolean ret;
	int client = getClientOffset(ent);

	if(hdll == NULL) return FALSE;

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

	if(hdll == NULL) return;

	q2a_lua_ClientBegin(client);

	dllglobals->ClientBegin(ent);
	copyDllInfo();
}

void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	int client = getClientOffset(ent);

	if(hdll == NULL) return;

	q2a_lua_ClientUserinfoChanged(client, userinfo);

	dllglobals->ClientUserinfoChanged(ent, userinfo);
	copyDllInfo();
}

void ClientDisconnect (edict_t *ent)
{
	int client = getClientOffset(ent);

	if(hdll == NULL) return;

	q2a_lua_ClientDisconnect(client);

	dllglobals->ClientDisconnect(ent);
	copyDllInfo();
}

void ClientCommand (edict_t *ent)
{
	int client = getClientOffset(ent);
	
	if(hdll == NULL) return;

	if(q2a_lua_ClientCommand(client))
		return;

	dllglobals->ClientCommand(ent);
	copyDllInfo();
}

void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	int client = getClientOffset(ent);
	
	if(hdll == NULL) return;
		
	q2a_lua_ClientThink(client);
	
	dllglobals->ClientThink(ent, ucmd);
	
	copyDllInfo();
}

void RunFrame(void)
{
	if(hdll == NULL) return;

	q2a_lua_RunFrame();

	dllglobals->RunFrame();
	copyDllInfo();
}

void ServerCommand (void)
{
	char *cmd;

	if(hdll == NULL) return;

	cmd = gi.argv(1);

	if(!q2a_strcmp(cmd, "lua_reload")) {
		q2a_lua_reload();
		return;
	}

	if(!q2a_strcmp(cmd, "lua_init")) {
		q2a_lua_init();
		return;
	}

	if(!q2a_strcmp(cmd, "lua_shutdown")) {
		q2a_lua_shutdown();
		return;
	}

	if(!q2a_strcmp(cmd, "lua_restart")) {
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
	char dllname[256];
	cvar_t *game;
	GAMEAPI *getapi;

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
	
	game = gi.cvar ("game", "baseq2", 0);
	q2a_strcpy(moddir, game->string);
	
	if(moddir[0] == 0)
		q2a_strcpy(moddir, "baseq2");

	sprintf(dllname, "%s/%s", moddir, DLLNAME);
#ifdef _WIN32
	hdll = LoadLibrary(dllname);
#else
	hdll = dlopen(dllname, RTLD_NOW);
#endif
	
	if(hdll == NULL)
	{
		gi.dprintf ("Q2A: Unable to load DLL %s.\n", dllname);
		return &globals;
	}

	gi.dprintf("Q2A: Loaded game DLL: %s.\n", dllname);
		
#ifdef _WIN32
	getapi = (GAMEAPI *)GetProcAddress (hdll, "GetGameAPI");
#else
	getapi = (GAMEAPI *)dlsym(hdll, "GetGameAPI");
#endif
	
	if(getapi == NULL)
	{
#ifdef _WIN32
		FreeLibrary(hdll);
#else
		dlclose(hdll);
#endif
		hdll = NULL;

		gi.dprintf ("Q2A: No \"GetGameApi\" entry in DLL %s.\n", dllname);
		return &globals;
	}
		
	dllglobals = (*getapi)(import);

	copyDllInfo();
	
	return &globals;
}
