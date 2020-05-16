INCDIR=inc/
CDIR=c/
CC=gcc
CFLAGS=-I$(INCDIR) -DKXVER=3 -fPIC -shared -lpthread -Wno-incompatible-pointer-types

print-% : ; @echo $* = $($*)

CORE=$(CDIR)blog.c

all: clean eg1 eg2 eg3 

eg1:
	$(CC) $(CORE) c/eg1.c $(CFLAGS) -o lib/eg1.so

eg2:
	$(CC) $(CORE) c/eg2.c $(CFLAGS) -o lib/eg2.so

eg3: 
	$(CC) ${CORE} $(CFLAGS) c/eg3.c c/cb.c -o lib/eg3.so

.PHONY: clean

clean:
	rm -f lib/*
