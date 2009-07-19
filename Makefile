LUA_CFLAGS = $(shell pkg-config --cflags lua5.1)
LUA_LDFLAGS = $(shell pkg-config --libs lua5.1)
LDFLAGS = -lm -lcurl $(LUA_LDFLAGS)
CC=gcc

CFLAGS = -O -g -Wall -DLINUX -fPIC $(LUA_CFLAGS)
#CFLAGS = -ffast-math -O3 -Wall -DLINUX -fPIC $(LUA_CFLAGS)

PLATFORM=$(shell uname -s|tr A-Z a-z)

ifneq ($(PLATFORM),linux)
ifneq ($(PLATFORM),freebsd)
ifneq ($(PLATFORM),darwin)
	$(error OS $(PLATFORM) is currently not supported)
endif
endif
endif

ARCH:=$(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc/ -e s/sparc64/sparc/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/alpha/axp/)

SHLIBEXT=so
GAME_NAME=game$(ARCH).$(SHLIBEXT)

MAKE_FLAGS = CC=$(CC) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" GAME_NAME=$(GAME_NAME)

all:
	cd src && make all $(MAKE_FLAGS)

clean:
	cd src && make clean $(MAKE_FLAGS)
	rm -f $(GAME_NAME)
