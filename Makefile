# this makefile is for x64 linux systems!
# adjust for your own system if needed
# -- MDVz0r

CFLAGS = -O -g -Wall -DLINUX -fPIC
#CFLAGS = -ffast-math -O3 -Wall -DLINUX -fPIC
LDFLAGS = -lcurl
ORIGDIR=src
CC=gcc

OBJS = g_main.o q2a_cmd.o q2a_init.o q2a_util.o q2a_run.o q2a_http.o

gamei386.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)
	#ldd $@

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
q2a_cmd.o: g_local.h q_shared.h game.h
q2a_init.o: g_local.h q_shared.h game.h
q2a_util.o: g_local.h q_shared.h game.h
q2a_run.o: g_local.h q_shared.h game.h
q2a_http.o: g_local.h q_shared.h game.h
