uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
OPTIMIZATION?=-O2
DEPENDENCY_TARGETS= #lua

# Default settings
# -pedantic is strictly ansi standard, maybe -pedantic-errors is prefer
# http://stackoverflow.com/questions/2855121/what-is-the-purpose-of-using-pedantic-in-gcc-g-compiler
# STD=-std=c99 -pedantic
STD=-std=gnu99
# WARN=-Wall -W
WARN=-Wstrict-prototypes -Wall -Wextra -Wno-sign-compare -Wpointer-arith -Wformat -Wformat-security -Wswitch-enum -Wunused-parameter -Wbad-function-cast -Wcast-align -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-field-initializers -fno-strict-aliasing
OPT=$(OPTIMIZATION)

PREFIX?=/usr/local
INSTALL_BIN=$(PREFIX)/bin
INSTALL=install

# Default allocator
ifeq ($(uname_S),Linux)
	# MALLOC=jemalloc
	MALLOC=libc
else
	MALLOC=libc
endif

# Backwards compatibility for selecting an allocator
ifeq ($(USE_TCMALLOC),yes)
	MALLOC=tcmalloc
endif

ifeq ($(USE_TCMALLOC_MINIMAL),yes)
	MALLOC=tcmalloc_minimal
endif

ifeq ($(USE_JEMALLOC),yes)
	MALLOC=jemalloc
endif

ifeq ($(USE_JEMALLOC),no)
	MALLOC=libc
endif

# Override default settings if possible
-include .make-settings

FINAL_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS) $(REDIS_CFLAGS)
FINAL_LDFLAGS=$(LDFLAGS) $(REDIS_LDFLAGS) $(DEBUG)
FINAL_LIBS=-lm
# https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Debugging-Options.html#Debugging-Options
DEBUG=-g -ggdb

ifeq ($(uname_S),SunOS)
	# SunOS
	INSTALL=cp -pf
	FINAL_CFLAGS+= -D__EXTENSIONS__ -D_XPG6
	FINAL_LIBS+= -ldl -lnsl -lsocket -lresolv -lpthread -lrt
else
ifeq ($(uname_S),Darwin)
	# Darwin (nothing to do)
else
ifeq ($(uname_S),AIX)
        # AIX
        FINAL_LDFLAGS+= -Wl,-bexpall
        FINAL_LIBS+= -pthread -lcrypt -lbsd

else
	# All the other OSes (notably Linux)
	# -rdynamic
	#      Pass the flag -export-dynamic to the ELF linker, on targets that
	#      support it. This instructs the linker to add all symbols, not only
	#      used ones, to the dynamic symbol table. This option is needed for
	#      some uses of dlopen or to allow obtaining backtraces from within a
	#      program
	FINAL_LDFLAGS+= -rdynamic
	FINAL_LIBS+= -lpthread
endif
endif
endif

# Include paths to dependencies
# FINAL_CFLAGS+= -I../deps/lua/src

ifeq ($(MALLOC),tcmalloc)
	FINAL_CFLAGS+= -DUSE_TCMALLOC
	FINAL_LIBS+= -ltcmalloc
endif

ifeq ($(MALLOC),tcmalloc_minimal)
	FINAL_CFLAGS+= -DUSE_TCMALLOC
	FINAL_LIBS+= -ltcmalloc_minimal
endif

ifeq ($(MALLOC),jemalloc)
	DEPENDENCY_TARGETS+= jemalloc
	FINAL_CFLAGS+= -DUSE_JEMALLOC -I../deps/jemalloc/include
	FINAL_LIBS+= ../deps/jemalloc/lib/libjemalloc.a -ldl
endif

REDIS_CC=$(QUIET_CC)$(CC) $(FINAL_CFLAGS)
REDIS_LD=$(QUIET_LINK)$(CC) $(FINAL_LDFLAGS)
REDIS_INSTALL=$(QUIET_INSTALL)$(INSTALL)

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

REDIS_SERVER_NAME=poll-loop
REDIS_SERVER_OBJ=main.o util.o ovs-thread.o fatal-signal.o signals.o sset.o hmap.o hash.o random.o timeval.o entropy.o socket-util.o dynamic-string.o seq.o ovs-rcu.o poll-loop.o guarded-list.o latch.o

all: $(REDIS_SERVER_NAME)
	@echo ""
	@echo ""

.PHONY: all

# Deps (use make dep to generate this)
include Makefile.dep

dep:
	$(REDIS_CC) -MM *.c > Makefile.dep

.PHONY: dep

persist-settings: distclean
	echo STD=$(STD) >> .make-settings
	echo WARN=$(WARN) >> .make-settings
	echo OPT=$(OPT) >> .make-settings
	echo MALLOC=$(MALLOC) >> .make-settings
	echo CFLAGS=$(CFLAGS) >> .make-settings
	echo LDFLAGS=$(LDFLAGS) >> .make-settings
	echo REDIS_CFLAGS=$(REDIS_CFLAGS) >> .make-settings
	echo REDIS_LDFLAGS=$(REDIS_LDFLAGS) >> .make-settings
	echo PREV_FINAL_CFLAGS=$(FINAL_CFLAGS) >> .make-settings
	echo PREV_FINAL_LDFLAGS=$(FINAL_LDFLAGS) >> .make-settings
	#-(cd ../deps && $(MAKE) $(DEPENDENCY_TARGETS))

.PHONY: persist-settings

# Prerequisites target
.make-prerequisites:
	@touch $@

# Clean everything, persist settings and build dependencies if anything changed
ifneq ($(strip $(PREV_FINAL_CFLAGS)), $(strip $(FINAL_CFLAGS)))
.make-prerequisites: persist-settings
endif

ifneq ($(strip $(PREV_FINAL_LDFLAGS)), $(strip $(FINAL_LDFLAGS)))
.make-prerequisites: persist-settings
endif

# redis-server
$(REDIS_SERVER_NAME): $(REDIS_SERVER_OBJ)
	$(REDIS_LD) -o $@ $^ $(FINAL_LIBS)

# Because the jemalloc.h header is generated as a part of the jemalloc build,
# building it should complete before building any other object. Instead of
# depending on a single artifact, build all dependencies first.
%.o: %.c .make-prerequisites
	$(REDIS_CC) -c $<

clean:
	rm -rf $(REDIS_SERVER_NAME) *.o *.gcda *.gcno *.gcov redis.info lcov-html

.PHONY: clean

distclean: clean
	-(rm -f .make-*)

.PHONY: distclean

lcov:
	$(MAKE) gcov
	@(set -e; cd ..; ./runtest --clients 1)
	@geninfo -o redis.info .
	@genhtml --legend -o lcov-html redis.info

gcov:
	$(MAKE) REDIS_CFLAGS="-fprofile-arcs -ftest-coverage -DCOVERAGE_TEST" REDIS_LDFLAGS="-fprofile-arcs -ftest-coverage"

noopt:
	$(MAKE) OPTIMIZATION="-O0"

valgrind:
	$(MAKE) OPTIMIZATION="-O0" MALLOC="libc"

install: all
	@mkdir -p $(INSTALL_BIN)
	$(REDIS_INSTALL) $(REDIS_SERVER_NAME) $(INSTALL_BIN)
	@ln -sf $(REDIS_SERVER_NAME) $(INSTALL_BIN)/$(REDIS_SENTINEL_NAME)




#http://stackoverflow.com/questions/12634114/will-adding-the-rdynamic-linker-option-to-gcc-g-impact-performance
#The -rdynamic option instructs the linker to add symbols to the symbol tables that are not normally needed at run time. It means there are more, possibly many more, symbols that the dynamic linker needs to weed through at run time for symbol resolution.

#Specifically, since symbol table lookups in GNU based systems are implemented using a hash, having more symbols increases the chance that there would be hash collisions. Since all symols that collide in the hash table sit in a list, the run time linker needs to traverse the list and compare, using memcmp, each symbol name. Having more symbols collide in the hash meaning having longer lists and so it will take more time to resolve each dynamic symbol.

#This situation is slightly worse for C++ then C, with the multitude of identically prefixed symbol names due to class names.

#In practice, this only effects the very first time that a symbol is used and so, unless your application is very large and contains a lot of symbols, it will not be felt.

#In the rare case that your application is that large, tricks like prelinking can be used to overcome the overhead.
