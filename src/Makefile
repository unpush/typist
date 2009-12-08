# ------------------------------------------------------
#	typist
#
#   This program will print out exercise lines to the
#   display, and then check for correct typing.
# ------------------------------------------------------
#	Original: D. Jason Penney (penneyj@slc.com)
#	Tuned for ECIP NeXT: Takeshi Ogihara
#	Ver.1.0   1992-07-14
#	Ver.2.0   1997-04-21	Kana Exercise
#	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
# ------------------------------------------------------


# Following options modified depending on TARGET ENVIRONMENT:
#
# following options should work on most BSD systems:
#LIBS=-ltermcap
#DEBUG=-O -s

# following options for all SMI (Sun Microsystems) systems:
#LIBS=-ltermcap
# Note: Sun-3 needs -O2 for kbdbind.c (SIGSEGV in iropt)
#DEBUG=-O4

# Following options for Ultrix
#LIBS=-ltermcap
#DEBUG=-O2

# Following options for Sony NEWS
#LIBS=-ltermcap
#DEBUG=-O

# following options for AIX
#LIBS=
#DEBUG=-O

# following options for HP-UX
#LIBS=
#DEBUG=-O

# following options for LINUX
#LIBS=-ltermcap

# following options for NeXT Mach
#LIBS=-ltermcap
#DEBUG=-O -s -Wall
#MAB=-arch m68k -arch i386 -arch hppa -arch sparc

# following options for SGI IRIX
#LIBS=-ltermcap
#DEBUG=-O -s

##### INSTALL PARAMETERS #####
LIBS=-lncurses
DEBUG=-O2 -Wall -s
PREFIX=/usr/local
LESSONDIR=$(PREFIX)/lib/typist
TARGETDIR=$(PREFIX)/bin
MANPATH=$(PREFIX)/man
# NeXT ONLY!!  To make FAT binary.
#MAB=

CFLAGS=$(DEBUG) $(MAB) -DTERMIO
# -DKEYTYPE=\"e\"
# -DLESSONDIR=\"$(LESSONDIR)\"
# -DVANISHCHAR

## -DLESSONDIR		if (LESSONDIR != /usr/local/lib/typist) set this!
## -DKEYTYPE		if you want to set keyboard type (default == "j")
## -DMSDOS		for MSDOS system
## -DTERMIO		for SYS/V Unix with termio(7).
## -DSILENT		if you hate click beep
## -DVANISHCHAR		if the character after an error vanishes.

LDFLAGS=$(DEBUG) $(MAB)
CC=gcc

#-----------------------------------------------------------------------

SRCFILES=main.c typist.c typist.h lesson.c lineio.c fileinfo.c \
		sound.c screen.c kana.c
OBJFILES=main.o typist.o lesson.o lineio.o fileinfo.o sound.o \
		screen.o kana.o

#-----------------------------------------------------------------------

all: typist

typist: $(OBJFILES)
	$(CC) $(LDFLAGS) -o typist $(OBJFILES) $(LIBS)

clean:
	rm -f *.o

install: typist $(LESSONDIR)
	cp -p typist $(TARGETDIR)
	chmod 755 $(TARGETDIR)/typist
	cp -p ../lib/* $(LESSONDIR)
	chmod 644 $(LESSONDIR)/*
	cp -p ../doc/typist.1 $(MANPATH)/man1
	chmod 644 $(MANPATH)/man1/typist.1

$(LESSONDIR):
	mkdir $(LESSONDIR)
	chmod 755 $(LESSONDIR)

main.o typist.o lesson.o lineio.o fileinfo.o sound.o kana.o: typist.h
lesson.o lineio.o: lineio.h
screen.o typist.o: screen.h
