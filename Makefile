SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS   = -Wall -Wextra -mtune=native
WARN_OFF = -Wno-implicit-function-declaration
CFLAGS1  = $(CFLAGS) $(WARN_OFF)
LDFLAGS  = 
LDFLAGS1 = $(LDFLAGS)  -lcurl
LDFLAGS2 = $(LDFLAGS)  -ljansson
LDFLAGS3 = $(LDFLAGS1) $(LDFLAGS2)
srcdir	 =./
builddir =build/ ## unsused til more examples

TARGETS	 = 1 2 3 4 5 6 7 8

.PHONY: all
all: $(TARGETS)

# save shadertoy shader to disk without using API
1: $(srcdir)1.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS1)

# save shadertoy shader to disk with API and conf-file for API-Key
2: $(srcdir)2.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS1)

# parse file with jansson and get some output
3: $(srcdir)3.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS2)

# get ressource from shadertoy after parsing json
4: $(srcdir)4.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS3)

# check type
5: $(srcdir)5.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS3)

# check type & existence, pass filename
6: $(srcdir)6.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS3)

# latest - clean
7: $(srcdir)7.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS3)

# like 7 but without using API - API changed Shader is now an Array
8: $(srcdir)8.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS3)

.PHONY: clean
clean:
	@rm $(TARGETS) 2>/dev/null || true

