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

#ifdef LINUXAXP
	#define DLLNAME   "gameaxp.real.so"
#elif defined(SOLARIS_INTEL)
	#define DLLNAME   "gamei386.real.so"
#elif defined(SOLARIS_SPARC)
	#define DLLNAME   "gamesparc.real.so"
#elif defined (LINUX) && defined (__x86_64__)
	#define DLLNAME "gamex86_64.real.so"
#elif defined (LINUX) && defined (i386)
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

char *q2a_version = "Q2Admin Version " Q2ADMINVERSION;
char dllname[256];
char moddir[256];

void ShutdownGame (void)
{
	q2a_http_shutdown();
	q2a_lua_shutdown();

	if(!dllloaded) return;

	gi.TagFree(playerinfo);
		
	dllglobals->Shutdown();
	
#ifdef __GNUC__
	dlclose(hdll);
#elif defined(WIN32)
	FreeLibrary(hdll);
#endif
	
	dllloaded = FALSE;
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
	
	import->bprintf = bprintf_internal;
	import->cprintf = cprintf_internal;
	import->dprintf = dprintf_internal;
	import->AddCommandString = AddCommandString_internal;

	//import->Pmove = Pmove_internal;
	//import->linkentity = linkentity_internal;
	//import->unlinkentity = unlinkentity_internal;
	//import->linkentity = linkentity;
	//import->unlinkentity = unlinkentity;
	
	globals.Init = InitGame;
	globals.Shutdown = ShutdownGame;
	globals.SpawnEntities = SpawnEntities;
	
	// why are we catching these?
	globals.WriteGame = WriteGame;
	globals.ReadGame = ReadGame;
	globals.WriteLevel = WriteLevel;
	globals.ReadLevel = ReadLevel;
	
	globals.ClientThink = ClientThink;
	globals.ClientConnect = ClientConnect;
	globals.ClientUserinfoChanged = ClientUserinfoChanged;
	globals.ClientDisconnect = ClientDisconnect;
	globals.ClientBegin = ClientBegin;
	globals.ClientCommand = ClientCommand;
	
	globals.RunFrame = G_RunFrame;
	
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
