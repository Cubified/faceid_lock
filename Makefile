all: userimage faceid

CC=gcc
LD=ld

LIBS=-lX11 -lXft -lfreenect -lfreenect_sync -lpython2.7
CFLAGS=-Iinclude -I/usr/include/freetype2

FLAGS=-Os -pipe -s
DEBUGFLAGS=-Og -pipe -g -std=c11 -pedantic

USERIMAGE_INPUT=img/placeholder.jpg
USERIMAGE_OUTPUT=img/placeholder.o

INPUT=$(USERIMAGE_OUTPUT) src/faceid.c
OUTPUT=faceid_lock

INSTALLDIR=$(HOME)/.local/bin

RM=/bin/rm

userimage:
	if [ ! -e $(USERIMAGE_OUTPUT) ]; then $(LD) -r -b binary $(USERIMAGE_INPUT) -o $(USERIMAGE_OUTPUT); fi

faceid:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) $(FLAGS)

ldclean:
	if [ -e $(USERIMAGE_OUTPUT) ]; then $(RM) $(USERIMAGE_OUTPUT); fi

debug:
	$(MAKE) userimage
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS) $(DEBUGFLAGS)

install:
	test -d $(INSTALLDIR) || mkdir -p $(INSTALLDIR)
	install -pm 755 $(OUTPUT) $(INSTALLDIR)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
