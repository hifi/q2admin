# this makefile is for x64 linux systems!
# adjust for your own system if needed
# -- MDVz0r

LUA_CFLAGS = $(shell pkg-config --cflags lua5.1)
LUA_LDFLAGS = $(shell pkg-config --libs lua5.1)
CFLAGS = -O -g -Wall -DLINUX -fPIC $(LUA_CFLAGS)
#CFLAGS = -ffast-math -O3 -Wall -DLINUX -fPIC $(LUA_CFLAGS)
LDFLAGS = -lcurl $(LUA_LDFLAGS)
ORIGDIR=src
CC=gcc

OBJS = g_main.o q2a_cmd.o q2a_init.o q2a_util.o q2a_run.o q2a_http.o q2a_lua.o q2a_linux.o

gamei386.so: q2a_lua_plugman.h $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)
	#ldd $@

clean: 
	/bin/rm -f $(OBJS) gamei386.so q2a_lua_plugman.h

install: gamei386.so
	cp gamei386.so /home/hifi/q2serv/baseq2/

$*.o: $*.c
	$(CC) $(CFLAGS) -c $*.c

$*.c: $(ORIGDIR)/$*.c
	tr -d '\015' < $(ORIGDIR)/$*.c > $*.c

$*.h: $(ORIGDIR)/$*.h
	tr -d '\015' < $(ORIGDIR)/$*.h > $*.h

q2a_lua_plugman.h: q2a_lua_plugman.lua
	@# this is not the best way to "trim" Lua, but with some care it works just fine
	echo "#define LUA_PLUGMAN \"`sed 's/--.*$$//g' q2a_lua_plugman.lua | tr -s '\t' '\0' | tr '\n' ' ' | sed 's/\\\/\\\\\\\\\\\\\\\\/g' | sed 's/"/\\\\"/g'`\"" > q2a_lua_plugman.h

# DO NOT DELETE

g_main.o: g_local.h q_shared.h game.h
q2a_cmd.o: g_local.h q_shared.h game.h
q2a_init.o: g_local.h q_shared.h game.h
q2a_util.o: g_local.h q_shared.h game.h
q2a_run.o: g_local.h q_shared.h game.h
q2a_http.o: g_local.h q_shared.h game.h
q2a_linux.o: g_local.h q_shared.h game.h
q2a_lua.o: g_local.h q_shared.h game.h q2a_lua_plugman.h
q2a_lua_plugman.h: q2a_lua_plugman.lua
