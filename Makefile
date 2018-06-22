SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS   = -Wall -Wextra -mtune=native
WARN_OFF = -Wno-implicit-function-declaration
CFLAGS1  = $(CFLAGS) $(WARN_OFF)
LDFLAGS  = 
LDFLAGS1 = $(LDFLAGS)  -lcurl
LDFLAGS2 = $(LDFLAGS1) -ljansson

srcdir	 =./
builddir =build/ ## unsused til more examples

TARGETS	 = 1 2

.PHONY: all
all: $(TARGETS)

# save shadertoy shader to disk without using API
1: $(srcdir)1.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS1)

# save shadertoy shader to disk with API and conf-file for API-Key
2: $(srcdir)2.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS1)

.PHONY: clean
clean:
	@rm $(TARGETS) 2>/dev/null || true

