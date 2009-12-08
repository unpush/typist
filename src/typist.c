/* ------------------------------------------------------
	typist

    This program will print out exercise lines to the
    display, and then check for correct typing.
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.1.1   1992-08-05	ESC  ->  Ctrl-F
	Ver.1.2   1994-11-10	for non-NeXT WS
	Ver.1.21  1994-12-07	do_para() bug
	Ver.1.3   1994-12-09	REPEAT & BELL
	Ver.1.4   1997-04-07	for Terminals don't CR
	Ver.1.41  1997-04-14	ANSI C
	Ver.2.0   1997-05-21	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "typist.h"
#include "screen.h"

#define ASCII_ESC	27
#define ASCII_GS	29
#define CTRL_C		3
#define CTRL_F		6
#define ASCII_BEL	7
#define ASCII_FF	12
#define ASCII_BS	8
#define ASCII_RUBOUT	127

#if defined(VMS)
# define ESCAPER ASCII_GS
#else
# define ESCAPER CTRL_F		/* was ASCII_ESC */
#endif

static char *bar = "ー";
static char *cached_lines[MAX_LINES];
static int exerciseStart;
static int newStart;
static BoolType BreakLesson, SkipLesson;


#ifdef __STDC__
static void get_end_line(void);
static void displaySpeed(int, long, int);
static void do_drill(void);
static void do_kana(int);
static void do_para(void);
static void do_quick(int);
static void do_type(void);
static void do_instruction(void);
static void do_clear(void);
static void lesson_loop(void);
static void give_lesson(char *);
#endif


void cleanup(sig)	/* cleanup routine, esp. SIGINT */
    int sig;
{
    move(23,0);
    stand_end(ALLATTR);
    add_str("\nSee you again!!");
    end_screen();
    write_rc();
    printf("\n");
    exit(1);
}


int wait_user()
/* Get any response from user before proceeding */
{
    int ch, help=0;

    for ( ; ; ) {
	stand_out(STANDOUT);
	move(23, 0);
#ifdef JPN
	add_str("次に進むには何かキーを押して下さい...");
#else
	add_str("Press any key to continue...");
#endif
	stand_end(STANDOUT);
	ch = get_ch();
	if (ch == ASCII_BEL) {
	    move(0, 0);
	    stand_out(STANDOUT);
	    add_str(loud ? "[BELL OFF]":"[BELL  ON]");
	    stand_end(STANDOUT);
	    loud = !loud;
	}else if (ch == '?' && help == 0) {
	    move(0, 0);
	    stand_out(STANDOUT);
	    add_str("[ *HELP* ] ^F=SKIP ^H/BS=REPEAT ^L=MENU ^G=BELL ^C=QUIT");
	    stand_end(STANDOUT);
	    help = 1;
	}else if (ch == 'q' || ch == 'Q') {
	    add_str(">> Quit? (y/n) >>");
	    ch = get_ch();
	    if (ch == 'y' || ch == 'Y')
		cleanup(0);
	    break;
	}else if (ch == CTRL_C) {
	    cleanup(0);
	    /* EXIT */
	    /* Some systems like Indy do not check interrupt by Control-C */
	}else
	    break;
    }
    return ch;
}

static void get_end_line()
/* Read end of line, should be smarter... */
{
	get_ch();
}


static void displaySpeed(totalChars, elapsed, errs)
    /* show how fast you were */
    int totalChars;
    long elapsed;
    int errs;
{
    double testTime, words, speed, adjustedSpeed;

    if (elapsed <= 0) elapsed = 1;
    testTime = (double)elapsed / 60.0; /* time in minutes */
    words = (double)totalChars / 5.0;
    speed = words / testTime;
    if ((words -= (double)errs) < 0.0)
	words = 0.0;
    adjustedSpeed = words / testTime;

    move(19, 18);
#ifdef JPN
    add_fmt("入力スピード       = %5.1f (語/分)", speed);
    move(20, 18);
    add_fmt("誤入力を除いた場合 = %5.1f (語/分)", adjustedSpeed);
    move(21, 30);
    add_fmt("(誤入力率 %5.2f％)", (double)100.0 * errs / totalChars);
#else
    add_fmt("raw speed      = %5.1f wpm", speed);
    move(20, 18);
    add_fmt("adjusted speed = %5.1f wpm", adjustedSpeed);
    move(21, 31);
    add_fmt("with %5.2f%% errors", (double)100.0 * errs / totalChars);
#endif
}

static void do_drill(void)	/* \D, a drill */
{
    int ch, i, j, ln;
    int tries, oncemore;
    int errors;
    int totalChars;
    char *thisLine;
    int thisLength;
    long startTime = 0L, endTime;

    for (tries = 0; ; tries++) { /* a try */
	totalChars = 0;
	errors = 0;

	/* display drill pattern */
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln += 2) {
	    move(ln, 0);
	    add_str(cached_lines[i]);
	    totalChars += strlen(cached_lines[i]) + 1; /* +1 for NULL */
	}

	/* run the drill */
	for (i = exerciseStart, ln = 4; i < newStart; i++, ln += 2) {
	    thisLine = cached_lines[i];
	    thisLength = strlen(thisLine);

	    move(ln, 0);
	    for (j = 0; j < thisLength; j ++) {
		ch = get_ch();
		if (i == exerciseStart && j == 0)
		    startTime = time(NULL);
		    /* timer doesn't run till first stroke */
		if (ch < ' ') {
		    if (ch == ESCAPER) {
			clear_screen();
			goto ALL_DONE;
		    }
		    if (ch == ASCII_FF) {
			BreakLesson = TRUE;
			clear_screen();
			goto ALL_DONE;
		    }
		    if (ch == CTRL_C)
			cleanup(0);
		}
		if (ch != thisLine[j]) {
		    add_ch('X');
		    if (loud) sndbeep();
		    errors++;
		} else
		    add_ch(ch);
	    }
	    get_end_line();
	}
	endTime = time(NULL);

	displaySpeed(totalChars, endTime - startTime, errors);
	ch = wait_user();
	clear_screen();
	if (ch == ESCAPER)
	    goto ALL_DONE;
	if (ch == ASCII_FF) {
	    BreakLesson = TRUE;
	    goto ALL_DONE;
	}
	oncemore = (ch == ASCII_BS || ch == ASCII_RUBOUT);

/* Exit condition:
 *	tries = 0  =>  errors = 0
 *	tries = 1  =>  errors = 1
 *	tries = 2  =>  errors = 2
 *	tries >= 3  =>  exit
 */
	if (!oncemore && (tries >= 3 || errors <= tries))
	    break;

	move(0, 0);
#ifdef JPN
	add_str("もう１回...");
#else
	add_str("Try again...");
#endif
    }
    return;

    ALL_DONE:
	SkipLesson = TRUE;
}

static void do_kana(ruby)	/* \K, Kana drill */
    int ruby;
{
    int ch, i, j, ln;
    int tries, oncemore;
    int errors;
    int totalChars, firststroke;
    char *thisLine;
    int thisLength;
    long startTime = 0L, endTime;
    char *p;

    for (tries = 0; ; tries++) { /* a try */
	totalChars = 0;
	errors = 0;

	/* display drill pattern */
	if (ruby) {
	    for (i = exerciseStart, ln = 3; i < newStart; i++, ln += 3) {
		move(ln, 0);
		add_str(cached_lines[i]);
		move(ln+1, 0);
		add_str(cached_lines[++i]);
		totalChars += strlen(cached_lines[i]) + 1; /* +1 for NULL */
	    }
	}else {
	    for (i = exerciseStart, ln = 3; i < newStart; i++, ln += 2) {
		move(ln, 0);
		add_str(cached_lines[i]);
		totalChars += strlen(cached_lines[i]) + 1; /* +1 for NULL */
	    }
	}
	totalChars /= 2;

	/* run the drill */
	firststroke = 1;
	for (i = exerciseStart, ln = 4; i < newStart; i++, ln += 2) {
	    if (ruby) {
		ln++;
		i++;
	    }
	    move(ln, 0);
	    thisLine = cached_lines[i];
	    thisLength = strlen(thisLine);

	    for (j = 0; j < thisLength; j ++) {
		ch = get_kch();
		if (firststroke) {
		    startTime = time(NULL);
		    firststroke = 0;
		    /* timer doesn't run till first stroke */
		}
		if (ch < ' ') {
		    if (ch == ESCAPER) {
			clear_screen();
			goto ALL_DONE;
		    }
		    if (ch == ASCII_FF) {
			BreakLesson = TRUE;
			clear_screen();
			goto ALL_DONE;
		    }
		    if (ch == CTRL_C)
			cleanup(0);
		}
		p = kanamap[ch];
		if (p[0] == thisLine[j] && p[1] == thisLine[j+1])
		    add_str(p);
		else if (ch == '\\'
		    && thisLine[j] == bar[0] && thisLine[j+1] == bar[1])
		    add_str(bar);
		else {
		    add_str("XX");
		    if (loud) sndbeep();
		    errors++;
		}
		j++;
	    }
	    get_end_line();
	}
	endTime = time(NULL);

	displaySpeed(totalChars, endTime - startTime, errors);
	ch = wait_user();
	clear_screen();
	if (ch == ESCAPER)
	    goto ALL_DONE;
	if (ch == ASCII_FF) {
	    BreakLesson = TRUE;
	    goto ALL_DONE;
	}
	oncemore = (ch == ASCII_BS || ch == ASCII_RUBOUT);

/* Exit condition:
 *	tries = 0  =>  errors = 0
 *	tries = 1  =>  errors = 1
 *	tries = 2  =>  errors = 2
 *	tries >= 3  =>  exit
 */
	if (!oncemore && (tries >= 3 || errors <= tries))
	    break;

	move(0, 0);
#ifdef JPN
	add_str("もう１回...");
#else
	add_str("Try again...");
#endif
    }
    return;

    ALL_DONE:
	SkipLesson = TRUE;
}

static void do_para(void)	/* \P, practice paragraph */
{
    int ch, i, j, ln;
    char *thisLine;
    int thisLength;
    int totalChars = 0;
    int errors = 0;
    int oncemore = 0;
    long startTime = 0L, endTime;
    
    do {
	if (oncemore) {
	    clear_screen();
	    move(0, 0);
#ifdef JPN
	    add_str("もう１回...");
#else
	    add_str("Try again...");
#endif
	}
	totalChars = 0;
	/* print out practice text */
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
	    move(ln, 0);
	    add_str(cached_lines[i]);
	    totalChars += strlen(cached_lines[i]);
	}

	for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
	    move(ln, 0);
	    thisLine = cached_lines[i];
	    thisLength = strlen(thisLine);
	    for (j = 0; j < thisLength; j ++) {
		ch = get_ch();
		if (i == exerciseStart && j == 0)
		    startTime = time(NULL);
		    /* timer doesn't run till first stroke */
		if (ch < ' ') {
		    if (ch == ESCAPER) {
			goto ALL_DONE;
		    }
		    if (ch == ASCII_FF) {
			BreakLesson = TRUE;
			goto ALL_DONE;
		    }
		    if (ch == CTRL_C)
			cleanup(0);
		}
		/* Allow backspace */
		if (j != 0 && (ch == ASCII_BS || ch == ASCII_RUBOUT)) {
		    put_bs(thisLine[--j]);
		    j--;
		    continue; /* but notice that errors accumulate */
		}
		if (ch != thisLine[j]) {
		    stand_out(STANDOUT);
		    add_ch(thisLine[j]);
		    stand_end(STANDOUT);
		    if (loud) sndbeep();
		    errors++;
		} else
		    add_ch(ch);
#ifdef VANISHCHAR
		if (j+1 < thisLength) {
		    add_ch(thisLine[j+1]);
		    move(ln, j+1);
		}
#endif
	    }
	    get_end_line();
	}
	endTime = time(NULL);
 
	displaySpeed(totalChars, endTime - startTime, errors);
	if ((ch = wait_user()) == ASCII_FF) {
	    BreakLesson = TRUE;
	    goto ALL_DONE;
	}
	oncemore = (ch == ASCII_BS || ch == ASCII_RUBOUT);
    }while (oncemore);
    clear_screen();
    return;

ALL_DONE:
    SkipLesson = TRUE;
    clear_screen();
}

static void do_quick(ruby)	/* \Q, quick typing for Kana */
    int ruby;
{
    int ch, i, j, ln;
    char *thisLine;
    int thisLength;
    int totalChars = 0;
    int errors = 0;
    int oncemore = 0;
    int firststroke;
    long startTime = 0L, endTime;
    char *p;
#ifdef VANISHCHAR
    StrType	damn;
    int		dx;
#endif
    
    do {
	if (oncemore) {
	    clear_screen();
	    move(0, 0);
#ifdef JPN
	    add_str("もう１回...");
#else
	    add_str("Try again...");
#endif
	}
	totalChars = 0;
	/* print out practice text */
	if (ruby) {
	    for (i = exerciseStart, ln = 3; i < newStart; i++, ln += 2) {
		move(ln, 0);
		add_str(cached_lines[i]);
		move(ln+1, 0);
		add_str(cached_lines[++i]);
		totalChars += strlen(cached_lines[i]);
	    }
	}else {
	    for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
		move(ln, 0);
		add_str(cached_lines[i]);
		totalChars += strlen(cached_lines[i]);
	    }
	}
	totalChars /= 2;
    
	firststroke = 1;
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
	    if (ruby) {
		ln++;
		i++;
	    }
	    move(ln, 0);
	    thisLine = cached_lines[i];
	    thisLength = strlen(thisLine);
	    for (j = 0; j < thisLength; j += 2) {
		ch = get_kch();
		if (firststroke) {
		    startTime = time(NULL);
		    firststroke = 0;
		    /* timer doesn't run till first stroke */
		}
		if (ch < ' ') {
		    if (ch == ESCAPER) {
			goto ALL_DONE;
		    }
		    if (ch == ASCII_FF) {
			BreakLesson = TRUE;
			goto ALL_DONE;
		    }
		    if (ch == CTRL_C)
			cleanup(0);
		}
#ifdef VANISHCHAR
		damn[j] = 0;
#endif
		/* Allow backspace */
		if (j != 0 && (ch == ASCII_BS || ch == ASCII_RUBOUT)) {
		    put_2bs(&thisLine[j-2]);
		    j -= 4;
		    continue; /* but notice that errors accumulate */
		}
		p = kanamap[ch];
		if (p[0] == thisLine[j] && p[1] == thisLine[j+1])
		    add_fmt("%.2s", &thisLine[j]);
		else if (ch == '\\'
		    && thisLine[j] == bar[0] && thisLine[j+1] == bar[1])
		    add_str(bar);
		else {
#ifdef VANISHCHAR
		    damn[j] = 1;
		    add_str("XX");
#else
		    stand_out(STANDOUT);
		    add_fmt("%.2s", &thisLine[j]);
		    stand_end(STANDOUT);
#endif
		    if (loud) sndbeep();
		    errors++;
		}
	    }
	    get_end_line();
#ifdef VANISHCHAR
	    move(ln, 0);
	    dx = 0;
	    for (j = 0; j < thisLength; j += 2) {
		if (damn[j] != dx) {
		    if (dx)
			stand_end(STANDOUT);
		    else
			stand_out(STANDOUT);
		    dx = damn[j];
		}
		add_fmt("%.2s", &thisLine[j]);
	    }
	    if (dx) stand_end(STANDOUT);
#endif
	}
	endTime = time(NULL);
 
	displaySpeed(totalChars, endTime - startTime, errors);
	if ((ch = wait_user()) == ASCII_FF) {
	    BreakLesson = TRUE;
	    goto ALL_DONE;
	}
	oncemore = (ch == ASCII_BS || ch == ASCII_RUBOUT);
    }while (oncemore);
    clear_screen();
    return;

ALL_DONE:
    SkipLesson = TRUE;
    clear_screen();
}

static void do_type()	/* \T, type out a lesson */
{
    int i, ln;

    clear_screen();
    for (i = exerciseStart, ln = 1; i < newStart; i++, ln++) {
	move(ln, 0);
	add_str(cached_lines[i]);
    }
    if (wait_user() == ASCII_FF)
	BreakLesson = TRUE;
    clear_screen();
}

static void do_instruction()	/* \I, instruction line */
{
    clear_screen();
    move(0, 0);
    add_str(cached_lines[exerciseStart]);
}

static void do_clear()		/* \B, clear screen */
{
    clear_screen();
    move(1, 0);
    add_str(cached_lines[exerciseStart]);
    if (wait_user() == ASCII_FF)
	BreakLesson = TRUE;
}

static int get_exercise(startLine, newStart)
/* determine length and kind of exercise */
    int startLine;
    int *newStart;
{
    char *thisLine;
    int lineLength;
    int	kind = 'I';	/* default... */;

    if (cached_lines[startLine] == NULL)
	return -1; /* end of lesson */
    *newStart = startLine;
    /* Determine end of exercise */
    while (TRUE) {
	if ((thisLine = cached_lines[*newStart]) == NULL)
	    break; /* done */
	lineLength = strlen(thisLine);
	if (thisLine[lineLength - 2] == '\\') { /* expected terminator */
	    kind = thisLine[lineLength - 1];
	    thisLine[lineLength - 2] = '\0'; /* remove */
	    (*newStart) ++;
	    break;
	}
	(*newStart) ++;
    }
    return kind;
}

static void lesson_loop()	/* dance through lesson file */
{
    int	kind;

    exerciseStart = 0;
    while ((kind = get_exercise(exerciseStart, &newStart)) > 0) {
	BreakLesson = FALSE;
	switch (kind) {
	case 'B':
	  do_clear();
	  break;

	case 'P':
	  do_para();
	  break;
	case 'Q':	/* Kana */
	  do_quick(0);
	  break;
	case 'q':	/* Kana */
	  do_quick(1);
	  break;

	case 'D':
	  do_drill();
	  break;
	case 'K':
	  do_kana(0);	/* Kana */
	  break;
	case 'k':
	  do_kana(1);	/* Kana */
	  break;

	case 'T':
	  do_type();
	  break;

	case 'I':
	  do_instruction();
	  break;

	default:
	  fprintf(stderr,
		"DATA ERROR: Unknown Exercise (near line %d of this lesson)\n",
		newStart - 1);
	  cleanup(1);
	  break;
	}
	if (BreakLesson)
	    break;
	exerciseStart = newStart;
    }
}

static void give_lesson(lname)
    char *lname;
{
    SkipLesson = FALSE;
    lesson_loop();
    if (!BreakLesson && !SkipLesson)
	update_lesson(lname);
}


void typist(lname)
    char *lname;
{
    char *lessonName;
    int i;

    init_screen();
    clear_screen();
    sndbeep_init(LessonDir);
    signal(SIGINT, cleanup);
    cbreak();

    for (i = 0; i < MAX_LINES; i ++)
	cached_lines[i] = NULL;

    /* Start and format screen.  Open and read exercise file.
       Inquire for lesson to run */
    if (lname != NULL) {
	if (find_lesson(lname, cached_lines))
	    give_lesson(lname);
    }

    while (TRUE) {
	clear_screen();
	lessonName = get_lesson();
	if (!find_lesson(lessonName, cached_lines))
	    continue;
	give_lesson(lessonName);
    }
}
