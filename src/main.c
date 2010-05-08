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
	Ver.3.0   2007-09-06	by Takeshi Ogihara
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(MSDOS) || defined(__BORLANDC__)
# include <process.h>
# include <time.h>
#endif

#include "typist.h"
#include "fileinfo.h"
#include "http.h"
#ifdef JPN
#include "kana.h"
#endif

char *LessonDir = LESSONDIR;
BoolType isHttp = FALSE;
#ifdef KEYTYPE
char *keyboard_type = KEYTYPE;
#else
# ifdef JPN
char *keyboard_type = "j";
# else
char *keyboard_type = "e";
# endif
#endif

#ifdef SILENT
BoolType loud = FALSE;	/* without BEEP */
#else
BoolType loud = TRUE;	/* use BEEP */
#endif

static void usage(void)
{
#ifdef JPN
    printf(
"�g����: typist [�I�v�V����] [lesson]\n"
"�I�v�V����:\n"
"    -q       �����o���Ȃ��ŗ��K\n"
"    -V       �o�[�W�������\n"
"    -Ldir    ���b�X���f�[�^�̂���f�B���N�g�����w��\n"
"             Web�̃f�B���N�g�����w�肷��ꍇ: -Lhttp://www.edu/typlib\n"
"    -ks      �L�[�{�[�h�̎�ނ��w�� (s = e/j/k)\n"
"    lesson:  ���K���������b�X����(t1, s3 �Ȃ�)\n");
#else
    printf(
"Usage: typist [options] [lesson]\n"
"Options:\n"
"    -q       quiet mode\n"
"    -V       print current version\n"
"    -Ldir    specify lesson-data directory\n"
"             For a web directory, e.g. -Lhttp://www.edu/typlib\n"
"    lesson:  Lesson-name you want to do.\n");
#endif
    exit(1);
}

static void version(void)
{
    printf("typist on terminal...\n"
    "  Original:  26 Nov 91,   D. Jason Penney (penneyj@slc.com)\n"
    "  Tuned for Japanese Users on Unix, Linux, DOS and Windows\n"
    "  Ver. 3.0  2007-08-27\t (C) Takeshi Ogihara\n");
    exit(0);
}

int main(int argc, char **argv)
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
	case 'L':
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
	    keyboard_type = &argv[i][2];
	    break;
	case 'V':
	    version(); break;
	default:
	    printf("Error: %s ?\n", argv[i]);
	    usage(); break;
	}
    }

    isHttp = (strncmp(LessonDir, "http://", 7) == 0);
    if (isHttp)
	init_sock();
    if (!get_index()) {
#ifdef JPN
	printf("�G���[: ���b�X���̏�񂪓ǂݍ��߂܂���.\n");
	printf("\t�f�B���N�g�� %s ���Ԉ���Ă��܂�.\n", LessonDir);
#else
	printf("Error: can't get index for lessons.\n");
	printf("\tDirectory %s may be wrong.\n", LessonDir);
#endif
	exit(1);
    }
#if defined(KANA_LESSON)
    if (!init_kanamap(LessonDir, keyboard_type)) {
	printf("�G���[: ���ȃL�[�}�b�v��񂪓ǂݍ��߂܂���.\n");
	exit(1);
    }
#endif
    if (rc) {
	read_rc(rc);
	fclose(rc);
    }
    
    typist(lesson);
    if (isHttp)
	final_sock();
    return 0;
}
