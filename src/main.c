/* ------------------------------------------------------
	main.c	for typist
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for Japanese Users: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.1.1   1992-08-05	ESC  ->  Ctrl-F
	Ver.1.2   1994-11-10	for non-NeXT WS
	Ver.1.3   1994-12-09	REPEAT & BELL
	Ver.1.41  1997-04-13	ANSI C
	Ver.2.0   1997-05-20	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#ifdef  MSDOS
# include <process.h>
# include <time.h>
#endif

#include "typist.h"

char *LessonDir = LESSONDIR;
#ifdef KEYTYPE
char *Keytype = KEYTYPE;
#else
# ifdef JPN
char *Keytype = "j";
# else
char *Keytype = "e";
# endif
#endif

#ifdef SILENT
BoolType loud = FALSE;	/* without BEEP */
#else
BoolType loud = TRUE;	/* use BEEP */
#endif

static void usage()
{
#ifdef JPN
    printf("\
使い方: typist [オプション] [lesson]\n\
オプション:\n\
    -q       音を出さないで練習\n\
    -V       バージョン情報\n\
    -Ldir    レッスンデータのあるディレクトリを指定\n\
    -ks      キーボードの種類を指定 (s = e/j/k)\n\
Lesson:  練習したいレッスン名(t1, s3 など)\n");
#else
    printf("\
Usage: typist [options] [lesson]\n\
Options:\n\
    -q       quiet mode\n\
    -V       print current version\n\
    -Ldir    specify lesson-data directory\n\
Lesson:  Lesson-name you want to do.\n");
#endif
    exit(1);
}

static void version()
{
    printf("\
Typist...\n\
    Original:  26 Nov 91,   D. Jason Penney (penneyj@slc.com)\n\
    Tuned for Japanese Users on Unix WS and MS-DOS PC:\n\
	       Takeshi Ogihara (ogihara@seg.kobe-u.ac.jp)\n\
    Ver. 2.0  1997-05-20\tKana exercise\n");
    exit(0);
}

int main(argc, argv)
    int argc;
    char *argv[];
{
    int i;
    char *lesson = NULL;
    char *lpath;
    FILE *rc;

    rc = open_rc();
    if ((lpath = getenv(TYPLIB)) != NULL)
	LessonDir = lpath;
    for (i=1; i<argc; i++) {
	if (argv[i][0] != '-') {
	    lesson = argv[i];
	    break;
	}
	switch (argv[i][1]) {
	case 'q':
	    loud = FALSE;  break;
	case 'L':  /* DEBUG */
	    if (argv[i][2] <= ' ') {
		printf("Error: -L needs dir\n");
		exit(1);
	    }
	    LessonDir = &argv[i][2];
	    break;
	case 'k':  /* Keyboard */
	    if (argv[i][2] <= ' ') {
		printf("Error: -k needs keyboard type\n");
		exit(1);
	    }
	    Keytype = &argv[i][2];
	    break;
	case 'V':
	    version(); break;
	default:
	    printf("Error: %s ?\n", argv[i]);
	    usage(); break;
	}
    }
    if (!get_index()) {
#ifdef JPN
	printf("エラー: レッスンの情報が読み込めません.\n");
	printf("\tディレクトリ %s が間違っています.\n", LessonDir);
#else
	printf("Error: can't get index for lessons.\n");
	printf("\tDirectory %s may be wrong.\n", LessonDir);
#endif
	exit(1);
    }
#ifdef JPN
    if (!init_kanamap()) {
	printf("エラー: かなキーマップ情報が読み込めません.\n");
	exit(1);
    }
#endif
    if (rc) {
	read_rc(rc);
	fclose(rc);
    }
    
    typist(lesson);
    return 0;
}
