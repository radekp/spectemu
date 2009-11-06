CC=gcc
CXX=g++

FLAGS=-Wall
CPPFLAGS=

CFLAGS=$(FLAGS)
CXXFLAGS=$(FLAGS)

PROGS=filt spload spt2tap tap2spt taplist recs tzxlist

all: $(PROGS)

tzxlist_objs=tzxlist.o tapefile.o
tzxlist: $(tzxlist_objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o tzxlist $(tzxlist_objs)

clean:
	rm -f *.o
	rm -f $(PROGS)

