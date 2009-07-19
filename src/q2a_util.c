//
// q2admin
//
// q2a_util.c
//
// copyright 2000 Shane Powell
// copyright 2009 Toni Spets
//

#include "g_local.h"

// required for proxy testing
void stuffcmd(edict_t *e, char *s)
{
	gi.WriteByte (11);
	gi.WriteString (s);
	gi.unicast (e, true);
}


#if defined(WIN32)
#define Q_stricmp stricmp
#else
#define Q_stricmp strcasecmp
#endif

/* why the fuck int size is added to every alloc?! -hifi */
char *q2a_malloc (int size)
{
	char *mem = gi.TagMalloc(size + sizeof(int), TAG_GAME);

	if(!mem)
		return NULL;
	
	*(int *)mem = size;
	
	return mem + sizeof(int);
}

char *q2a_realloc (char *oldmem, int newsize)
{
	int oldsize;
	int *start = (int *)(oldmem - sizeof(int));
	char *newmem;

	/* ANSI C */
	if(oldmem == NULL)
		return q2a_malloc(newsize);

	oldsize = *start;

	if(oldsize >= newsize)
		return oldmem;

	newmem = gi.TagMalloc(newsize + sizeof(int), TAG_GAME);
	*(int *)newmem = newsize;
	newmem += sizeof(int);

	q2a_memcpy(newmem, oldmem, newsize - oldsize);

	gi.TagFree(start);

	return newmem;
}


void q2a_free (char *mem)
{
	/* ANSI C */
	if(mem == NULL)
		return;

	gi.TagFree(mem - sizeof(int));
}


/*
===============
Info_ValueForKey
 
Searches the string for the given
key and returns the associated value, or an empty string.
===============
*/
char *Info_ValueForKey(char *s, char *key)
{
	char pkey[512];
	static char value[2][512];// use two buffers so compares
	// work without stomping on each other
	static int valueindex;
	char *o;
	
	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
		{
			o = pkey;
			while (*s != '\\')
				{
					if (!*s)
						return "";
					*o++ = *s++;
				}
			*o = 0;
			s++;
			
			o = value[valueindex];
			
			while (*s != '\\' && *s)
				{
					if (!*s)
						return "";
					*o++ = *s++;
				}
			*o = 0;
			
			if (!q2a_strcmp (key, pkey) )
				return value[valueindex];
				
			if (!*s)
				return "";
			s++;
		}
}


/*
==================
Info_Validate
 
Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate(char *s)
{
	if(q2a_strstr(s,"\""))
		return false;
	if(q2a_strstr(s,";"))
		return false;
	return true;
}


void copyDllInfo(void)
{
	globals.apiversion = dllglobals->apiversion;
	globals.edict_size = dllglobals->edict_size;
	globals.edicts = dllglobals->edicts;
	globals.num_edicts = dllglobals->num_edicts;
	globals.max_edicts = dllglobals->max_edicts;
}


qboolean getLogicalValue(char *arg)
{
	if(Q_stricmp(arg,"Yes")== 0 || Q_stricmp(arg,"1")== 0 || Q_stricmp(arg,"Y")== 0)
	{
		return TRUE;
	}
		
	return FALSE;
}


void q_strupr(char *c)
{
	while(*c)
	{
		if(islower((*c)))
			{
				*c = toupper((*c));
			}
			
		c++;
	}
}
