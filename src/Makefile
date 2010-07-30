OBJS = g_main.o q2a_cmd.o q2a_init.o q2a_util.o q2a_run.o q2a_lua.o q2a_lua_import.o q2a_lua_export.o

$(GAME_NAME): q2a_lua_plugman.h $(OBJS)
	$(CC) -shared -o ../$@ $(OBJS) $(LDFLAGS)

clean: 
	/bin/rm -f $(OBJS) q2a_lua_plugman.h

$*.o: $*.c
	$(CC) $(CFLAGS) -c $*.c

q2a_lua_plugman.h: q2a_lua_plugman.lua
	@# this is not the best way to "trim" Lua, but with some care it works just fine
	echo "#define LUA_PLUGMAN \"`sed 's/--.*$$//g' q2a_lua_plugman.lua | tr -s '\t' '\0' | tr '\n' ' ' | sed 's/\\\/\\\\\\\\\\\\\\\\/g' | sed 's/"/\\\\"/g'`\"" > q2a_lua_plugman.h

all: $(GAME_NAME)

# DO NOT DELETE

g_main.o: g_local.h q_shared.h game.h
q2a_cmd.o: g_local.h q_shared.h game.h
q2a_init.o: g_local.h q_shared.h game.h
q2a_util.o: g_local.h q_shared.h game.h
q2a_run.o: g_local.h q_shared.h game.h
q2a_lua.o: g_local.h q_shared.h game.h q2a_lua_plugman.h
q2a_lua_import.o: g_local.h q_shared.h game.h q2a_lua_plugman.h
q2a_lua_export.o: g_local.h q_shared.h game.h q2a_lua_plugman.h
q2a_lua_plugman.h: q2a_lua_plugman.lua
