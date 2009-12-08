/* ------------------------------------------------------
	typist.h
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
		(ogihara@rd.ecip.osaka-u.ac.jp)
	Ver.1.0   1992-07-14
	Ver.1.1   1992-08-05	ESC  ->  Ctrl-F
	Ver.1.2   1994-11-10	for non-NeXT WS
	Ver.1.4   1997-04-07	ANSI C
	Ver.2.0   1997-05-17	Kana Exercise  by Takeshi Ogihara
	Modification for MinGW    May 2007   by OHKUBO Takuya
	Ver.3.0   2007-08-23	by Takeshi Ogihara
------------------------------------------------------ */

#include "base.h"

#ifndef  __DEF_TYPIST__
#define  __DEF_TYPIST__

#define  TYPDATA	"TYPDATA"
#define  TYPLIB		"TYPLIB"

#if defined(MSDOS) || defined(__MINGW32__) || defined(WinXP)

#define  DELIM		'\\'
# ifndef LESSONDIR
#   define  LESSONDIR	"c:\\lib\\typist"
# endif

#define   TYPISTRC	"typist.dat"

#else /* if UNIX */

#define  DELIM		'/'
# ifndef LESSONDIR
#   define  LESSONDIR	"/usr/local/lib/typist"
# endif

#define   TYPISTRC	".typistrc"
#endif /* defined(MSDOS) || ... */

#define   CLICKSND	"click.snd"
#define   FINISHED	0x1000

struct Lesson {
    char nam;
    short num, fin;
    char *item;
    char *path;
};

extern BoolType loud;
extern BoolType HasHelp;
extern BoolType isHttp;
extern struct Lesson typeLessons[];
extern char *LessonDir;
extern char *keyboard_type;
extern char LastLesson;
extern short LastNum;
extern char **cached_lines;
extern char *cached_attr;

void sndbeep(void);
void init_sndbeep(const char *);
void typist(const char *);
const char *get_lesson(void);
BoolType find_lesson(const char *);
int wait_user(void);
void cleanup(int);

#endif  /* __DEF_TYPIST__ */
