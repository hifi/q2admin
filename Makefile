# this makefile is for x64 linux systems!
# adjust for your own system if needed
# -- MDVz0r

#CFLAGS = -O -g -DNDEBUG -DLINUX -Dstricmp=Q_stricmp -fPIC
CFLAGS = -ffast-math -O3 -w -DGAME_INCLUDE -DLINUX -fPIC
LDFLAGS = 
ORIGDIR=src
CC=gcc

OBJS = g_main.o zb_clib.o zb_cmd.o zb_init.o zb_log.o zb_msgqueue.o zb_util.o zb_zbot.o zb_zbotcheck.o

gamei386.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)
	ldd $@

clean: 
	/bin/rm -f $(OBJS) gamei386.so

install: gamei386.so
	cp gamei386.so /home/hifi/q2serv/baseq2/

$*.o: $*.c
	$(CC) $(CFLAGS) -c $*.c

$*.c: $(ORIGDIR)/$*.c
	tr -d '\015' < $(ORIGDIR)/$*.c > $*.c

$*.h: $(ORIGDIR)/$*.h
	tr -d '\015' < $(ORIGDIR)/$*.h > $*.h

# DO NOT DELETE

g_main.o: g_local.h q_shared.h game.h
zb_checkvar.o: g_local.h q_shared.h game.h
zb_clib.o: g_local.h q_shared.h game.h
zb_cmd.o: g_local.h q_shared.h game.h
zb_init.o: g_local.h q_shared.h game.h
zb_log.o: g_local.h q_shared.h game.h
zb_msgqueue.o: g_local.h q_shared.h game.h
zb_util.o: g_local.h q_shared.h game.h
zb_zbot.o: g_local.h q_shared.h game.h
zb_zbotcheck.o: g_local.h q_shared.h game.h
