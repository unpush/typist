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
	Ver.2.0   1997-05-17	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#ifndef  __DEF_TYPIST__
#define  __DEF_TYPIST__

#define  JPN		1	/* Japanese MODE (don't remove) */

#if defined(sony_news)
extern int errno;
#endif

#define   MAX_LINES	800	/* was 1500 */
#define   STR_SIZE	255	/* was 1024 */
typedef char StrType[STR_SIZE + 1];
typedef unsigned char BoolType;

#if !defined(FALSE)
# define  FALSE		0
#endif
#if !defined(TRUE)
# define  TRUE		1
#endif

#if !defined(NULL)
# define  NULL		0
#endif

#define  TYPDATA	"TYPDATA"
#define  TYPLIB		"TYPLIB"

#ifdef  MSDOS

#define  DELIM		'\\'
# ifndef LESSONDIR
#   define  LESSONDIR	"c:\\lib\\typist"
# endif

#define   TYPISTRC	"typist.dat"

#else /* MSDOS */

#define  DELIM		'/'
# ifndef LESSONDIR
#   define  LESSONDIR	"/usr/local/lib/typist"
# endif

#define   TYPISTRC	".typistrc"
#endif /* MSDOS */

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
extern struct Lesson typeLessons[];
extern char *LessonDir;
extern char *Keytype;
extern char LastLesson;
extern short LastNum;


#ifdef __STDC__

void sndbeep(void);
void sndbeep_init(char *);
void typist(char *);
BoolType get_index(void);
int check_lesson_name(char *);
FILE *open_rc(void);
void read_rc(FILE *);
void write_rc(void);
char *get_lesson(void);
BoolType find_lesson(char *, char **);
void update_lesson(char *);
int wait_user(void);
void cleanup(int);

#else   /* __STDC__ */

extern void sndbeep();
extern void sndbeep_init();
extern void typist();
extern BoolType get_index();
extern int check_lesson_name();
extern FILE *open_rc();
extern void read_rc();
extern void write_rc();
extern char *get_lesson();
extern BoolType find_lesson();
extern void update_lesson();
extern int wait_user();
extern void cleanup();

#endif  /* __STDC__ */


extern char	kanamap[256][3];

# ifdef __STDC__
BoolType init_kanamap(void);
# else
extern BoolType init_kanamap();
# endif

#endif  /* __DEF_TYPIST__ */
