/* ------------------------------------------------------
	typist  -- tutor.c

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
	by Takeshi Ogihara
	Modification for MinGW    May 2007   by OHKUBO Takuya
	Ver.3.0   2007-08-02	by Takeshi Ogihara
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "typist.h"
#include "fileinfo.h"
#include "dictionary.h"
#include "screen.h"
#include "tutor.h"

int exerciseStart;
int newStart;
BoolType BreakLesson, SkipLesson, lessonBack;


void cleanup(int sig)	/* cleanup routine, esp. SIGINT */
{
    move_cursor(23, 0);
    stand_end(ALLATTR);
    add_str("\nSee you again!!");
    end_screen();
    write_rc();
#if defined(__MINGW32__)
    printf("\nSee you again!!");
#endif /* defined(__MINGW32__) */
    printf("\n");
    exit(1);
}


int wait_user(void)
/* Get any response from user before proceeding */
{
    int ch, help=0;

    for ( ; ; ) {
	stand_out(STANDOUT);
	move_cursor(23, 0);
#ifdef JPN
	add_str("次に進むには何かキーを押して下さい...");
#else
	add_str("Press any key to continue...");
#endif
	stand_end(STANDOUT);
	ch = get_ch();
	if (ch == ASCII_BEL) {
	    move_cursor(0, 0);
	    stand_out(STANDOUT);
	    add_str(loud ? "[BELL OFF]":"[BELL  ON]");
	    stand_end(STANDOUT);
	    loud = !loud;
	}else if (ch == '?' && help == 0) {
	    move_cursor(0, 0);
	    stand_out(STANDOUT);
#ifdef JPN
	    add_str("[ *ヘルプ* ] ^G=ベル ^H/BS=再挑戦 ^B=戻る ^L=メニュー ^F=スキップ ^C=終了");
#else
	    add_str("[ *HELP* ] ^G=BELL ^H/BS=REPEAT ^B=BACK ^L=MENU ^F=SKIP ^C=QUIT");
#endif
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
	}else if (ch == CTRL_B) {
	    lessonBack = TRUE;
	    break;
	}else
	    break;
    }
    return ch;
}


static unsigned long end_line_pool = 0;

void set_end_line(unsigned long remain)
{
    end_line_pool = remain;
}

void get_end_line(void)
/* Read end of line, should be smarter... */
{
    if (end_line_pool == 0)
	get_ch();
    else
	end_line_pool = 0;
}

#define BarWidth 40

void barchart(double strokeSpeed, double errRatio)
{
    char buf[BarWidth+4];
    const int width = BarWidth;
    const double upperlimit = 300.0; /* 300 strokes / min. */
    int x, i, correct, wrong;
    
    if (strokeSpeed >= upperlimit) {
	wrong = (int)(0.5 + (double)width * errRatio / strokeSpeed);
	correct = width - wrong;
    }else {
	correct = (int)(0.5 + (double)width * strokeSpeed / upperlimit);
	if (errRatio > 0.0) {
	    wrong = (int)(0.5 + (double)width * errRatio / upperlimit);
	    if (wrong == 0) wrong = 1;
	    correct -= wrong;
	}else
	    wrong = 0;
    }
    for (x = 0; x < width && x < correct; x++)
	buf[x] = 'O';
    for (i = 0; x < width && i < wrong; x++, i++)
	buf[x] = 'x';
    for ( ; x < width; x++)
	buf[x] = '.';
    buf[x] = 0;
    add_str(buf);
}

void displaySpeed(int totalChars, int strokes, long elapsed, int errs)
    /* show how fast you were */
{
    double testTime, words, speed, adjustedSpeed;
    double strokeSpeed, errRatio;
    const int leftMgn = 4;

    if (elapsed <= 0) elapsed = 1;
    if (errs > strokes)
	errs = strokes;
    testTime = (double)elapsed / 60.0; /* time in minutes */
    strokeSpeed = (double)strokes / testTime;
    errRatio = (double)errs / testTime;
    words = (double)totalChars / 5.0;
    speed = words / testTime;
    if ((words -= (double)errs) < 0.0)
	words = 0.0;
    adjustedSpeed = words / testTime;

    move_cursor(19, leftMgn);
#if JPN
    add_fmt("打鍵スピード(打/分) = %5.1f     ", strokeSpeed);
    barchart(strokeSpeed, errRatio);
    move_cursor(20, leftMgn);
    add_fmt("入力スピード(語/分) = %5.1f", adjustedSpeed);
    if (errs)
	add_fmt(" (ミス含:%5.1f)", speed);
    move_cursor(21, leftMgn+15);
    add_fmt("(誤入力率 %4.1f％)", 100.0 * errs / strokes);
#else
    add_fmt("Strokes / min.   = %5.1f     ", strokeSpeed);
    barchart(strokeSpeed, errRatio);
    move_cursor(20, leftMgn);
    add_fmt("Type Speed (wpm) = %5.1f", adjustedSpeed);
    if (errs)
	add_fmt(" (raw:%5.1f)", speed);
    move_cursor(21, leftMgn+15);
    add_fmt("with %4.1f%% errors", 100.0 * errs / strokes);
#endif
}

static void do_drill(void)	/* \D, a drill */
{
    int ch, i, j, ln;
    int tries, oncemore;
    int errors;
    int totalChars, strokes;
    char *thisLine;
    int thisLength;
    long startTime = 0L, endTime;

    for (tries = 0; ; tries++) { /* a try */
	totalChars = 0;
	errors = 0;

	/* display drill pattern */
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln += 2) {
	    move_cursor(ln, 0);
	    add_str(cached_lines[i]);
	    totalChars += strlen(cached_lines[i]) + 1; /* +1 for NULL */
	}

	/* run the drill */
	strokes = 0;
	for (i = exerciseStart, ln = 4; i < newStart; i++, ln += 2) {
	    thisLine = cached_lines[i];
	    thisLength = strlen(thisLine);

	    set_end_line(0);
	    move_cursor(ln, 0);
	    for (j = 0; j < thisLength; j ++) {
		ch = get_ch();
		if (strokes++ == 0)
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
		    if (ch == CTRL_B) {
			lessonBack = TRUE;
			clear_screen();
			return;
		    }
		    if (ch == CTRL_C)
			cleanup(0);
		}
		if (ch != thisLine[j]) {
		    add_ch('X');
		    if (loud) sndbeep();
		    errors++;
		} else {
		    add_ch(ch);
		}
	    }
	    get_end_line();
	}
	endTime = time(NULL);

	displaySpeed(totalChars, strokes, endTime - startTime, errors);
	ch = wait_user();
	clear_screen();
	if (ch == ESCAPER)
	    goto ALL_DONE;
	if (ch == ASCII_FF) {
	    BreakLesson = TRUE;
	    goto ALL_DONE;
	}
	if (ch == CTRL_B) { /* lessonBack = TRUE; */
	    return;
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

	move_cursor(0, 0);
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
    int totalChars, strokes;
    int errors;
    int oncemore = 0;
    long startTime = 0L, endTime;
    
    do {
	if (oncemore) {
	    clear_screen();
	    move_cursor(0, 0);
#ifdef JPN
	    add_str("もう１回...");
#else
	    add_str("Try again...");
#endif
	}
	totalChars = 0;
	/* print out practice text */
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
	    move_cursor(ln, 0);
	    add_str(cached_lines[i]);
	    totalChars += strlen(cached_lines[i]);
	}

	strokes = 0;
	errors = 0;
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
	    set_end_line(0);
	    move_cursor(ln, 0);
	    thisLine = cached_lines[i];
	    thisLength = strlen(thisLine);
	    for (j = 0; j < thisLength; j ++) {
		ch = get_ch();
		if (strokes++ == 0)
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
		    if (ch == CTRL_B) {
			lessonBack = TRUE;
			clear_screen();
			return;
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
		    move_cursor(ln, j+1);
		}
#endif
	    }
	    get_end_line();
	}
	endTime = time(NULL);
 
	displaySpeed(totalChars, strokes, endTime - startTime, errors);
	if ((ch = wait_user()) == ASCII_FF) {
	    BreakLesson = TRUE;
	    goto ALL_DONE;
	}
	/* if (ch == CTRL_B) then return normally. */
	oncemore = (ch == ASCII_BS || ch == ASCII_RUBOUT);
    }while (oncemore);
    clear_screen();
    return;

ALL_DONE:
    SkipLesson = TRUE;
    clear_screen();
}


static void do_type(void)	/* \T, type out a lesson */
{
    int i, ln;

    clear_screen();
    for (i = exerciseStart, ln = 1; i < newStart; i++, ln++) {
	move_cursor(ln, 0);
	add_str(cached_lines[i]);
    }
    if (wait_user() == ASCII_FF)
	BreakLesson = TRUE;
    clear_screen();
}

static void do_instruction(void)	/* \I, instruction line */
{
    clear_screen();
    move_cursor(0, 0);
    add_str(cached_lines[exerciseStart]);
}

static void do_clear(void)		/* \B, clear screen */
{
    clear_screen();
    move_cursor(1, 0);
    add_str(cached_lines[exerciseStart]);
    if (wait_user() == ASCII_FF)
	BreakLesson = TRUE;
}

static int get_exercise(int startLine, int *newStart)
/* determine length and kind of exercise */
{
    int	kind;

    if (cached_lines[startLine] == NULL)
	return -1; /* end of lesson */
    *newStart = startLine;
    /* Determine end of exercise */
    kind = 0;
    while (kind == 0) {
	if (cached_lines[*newStart] == NULL)
	    break; /* End of Lesson */
	kind = cached_attr[*newStart];	/* terminator */
	(*newStart) ++;
    }
    return (kind ? kind : 'I');		/* 'I' is default.  Error.. */;
}

static int previous_exercise(int start){	int idx, one;	if (start <= 0)		return start;	/* error */	idx = start - 1;	one = 1; /* to skip previous mark */	while (idx >= 0) {	    if (cached_attr[idx] != 0 && cached_attr[idx] != 'I' && one-- <= 0)		break;	    idx--;	}	return (idx + 1);}

static void lesson_loop(void)	/* dance through lesson file */
{
    int	kind;
    BoolType flag;

    exerciseStart = 0;
    while ((kind = get_exercise(exerciseStart, &newStart)) > 0) {
	BreakLesson = lessonBack = FALSE;
	switch (kind) {
	case 'B':
	  do_clear();
	  break;

	case 'P':
	  do_para();
	  break;
	case 'Q':	/* Kana */
	  flag = !is_true_value("KANA_INPUT");
	  do_quick(FALSE, flag);
	  break;
	case 'q':	/* Kana */
	  flag = !is_true_value("KANA_INPUT");
	  do_quick(TRUE, flag);
	  break;

	case 'D':
	  do_drill();
	  break;
	case 'K':
	  flag = !is_true_value("KANA_INPUT");
	  do_kana(FALSE, flag);	/* Kana */
	  break;
	case 'k':
	  flag = !is_true_value("KANA_INPUT");
	  do_kana(TRUE, flag);	/* Kana */
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
	if (lessonBack) {		exerciseStart = previous_exercise(exerciseStart);		lessonBack = FALSE;	}else /* Next Exercise */		exerciseStart = newStart;
    }
}

static void give_lesson(const char *lname)
{
    SkipLesson = FALSE;
    lesson_loop();
    if (!BreakLesson && !SkipLesson)
	update_lesson(lname);
}


void typist(const char *lname)
{
    const char *lessonName;

    init_screen();
    clear_screen();
    init_sndbeep(LessonDir);
    signal(SIGINT, cleanup);
    cbreak_mode(1);

    /* Start and format screen.  Open and read exercise file.
       Inquire for lesson to run */
    if (lname != NULL) {
	if (find_lesson(lname))
	    give_lesson(lname);
    }

    while (TRUE) {
	clear_screen();
	lessonName = get_lesson();
	if (!find_lesson(lessonName))
	    continue;
	give_lesson(lessonName);
    }
}
