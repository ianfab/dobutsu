CC=c99
CFLAGS=-O3 -DNDEBUG

# number of threads used during table base generation
NPROC=2

# customize this if your system uses a different path structure
PREFIX=/usr/local
TBDIR=$(PREFIX)/share/dobutsu
BINDIR=$(PREFIX)/games
MANDIR=$(PREFIX)/share/man/man6
LIBEXECDIR=$(PREFIX)/lib

# replace with dobutsu.tb if you want to waste more space for a faster
# program start
# TBFILE=dobutsu.tb
TBFILE=dobutsu.tb.xz

GENTBOBJ=gentb.o tbgenerate.o poscode.o unmoves.o moves.o
VALIDATETBOBJ=$(XZOBJ) validatetb.o tbvalidate.o tbaccess.o notation.o poscode.o validation.o moves.o
DOBUTSUOBJ=$(XZOBJ) dobutsu.o position.o ai.o notation.o tbaccess.o validation.o poscode.o moves.o
XZOBJ=xz/xz_crc32.o xz/xz_dec_lzma2.o xz/xz_dec_stream.o

all: gentb validatetb dobutsu

gentb: $(GENTBOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o gentb $(GENTBOBJ) $(LDLIBS) -lpthread

validatetb: $(VALIDATETBOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o validatetb $(VALIDATETBOBJ) $(LDLIBS)

dobutsu: $(DOBUTSUOBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o dobutsu $(DOBUTSUOBJ) $(LDLIBS) -lm

dobutsu-stub:
	echo '#!/bin/sh' >dobutsu-stub
	echo >>dobutsu-stub
	echo '[ -z "$$DOBUTSU_TABLEBASE" ] && DOBUTSU_TABLEBASE="$(TBDIR)/$(TBFILE)"' >>dobutsu-stub
	echo 'export DOBUTSU_TABLEBASE' >>dobutsu-stub
	echo 'exec "$(LIBEXECDIR)/dobutsu" "$$@"' >>dobutsu-stub
	chmod a+x dobutsu-stub

dobutsu.tb.xz: dobutsu.tb
	rm -f dobutsu.tb.xz
	@# dictionary size must be harmonized with code in tbaccess.c
	xz -k -4 -e -C crc32 dobutsu.tb

dobutsu.tb: gentb
	./gentb -j $(NPROC) dobutsu.tb

clean:
	rm -f *.o xz/*.o gentb validatetb dobutsu dobutsu-stub

distclean: clean
	rm -f dobutsu.tb dobutsu.tb.xz dobutsu.6.gz

install: dobutsu dobutsu-stub $(TBFILE) 
	mkdir -p $(TBDIR)
	cp dobutsu.tb.xz $(TBDIR)/$(TBFILE)
	mkdir -p $(LIBEXECDIR)
	cp dobutsu $(LIBEXECDIR)/dobutsu
	mkdir -p $(BINDIR)
	cp dobutsu-stub $(BINDIR)/dobutsu
	mkdir -p $(MANDIR)
	cp dobutsu.6 $(MANDIR)/dobutsu.6

.PHONY: clean all distclean install

.POSIX:
