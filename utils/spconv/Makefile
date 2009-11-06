#
# UNIX Makefile for SPCONV - SPECTRUM SNAPSHOT CONVERTER 1.09
#
# Chris Wells
#
# Called "makefile.unx" under dos, rename to "Makefile" [HDG]

#.SILENT:
.SUFFIXES:
.SUFFIXES: .c .o $(SUFFIXES)

CC = gcc
CFLAGS =	-g \
		-ansi \
		-pedantic \
		-Wall \
		-Wwrite-strings \
		-Wshadow \
		-Wformat \
		-Wpointer-arith \
		-Wconversion \
		-Wstrict-prototypes \
		-Wmissing-prototypes \
		-I/usr/local/lib/ansi-include

NROFF = gnroff -mandoc

SRCS = spconv.c
OBJS = spconv.o
BINARY = spconv
DOCS = spconv.doc

spconv: $(OBJS) 
	$(CC) $(OBJS) -o $(BINARY)

spconv.o: spconv.h

clean:
	rm -f $(OBJS) $(BINARY) $(DOCS)

docs: spconv.1
	$(NROFF) < spconv.1 > spconv.doc

zip:	spconv.zip
	echo "Spconv.zip up to date"

spconv.zip: docs
	zip spconv.zip spconv.doc spconv.1 spconv.h \
	spconv.c Makefile history.doc author.doc
