/* ------------------------------------------------------
	kanatutor.c
---------------------------------------------------------
	Ver.2.0   1997-05-17	by Takeshi Ogihara
	Modification for MinGW    May 2007   by OHKUBO Takuya
	Ver.3.0   2007-09-06	by Takeshi Ogihara
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "typist.h"
#include "screen.h"
#include "tutor.h"
#include "kana.h"
#include "r2k.h"

static kanachar **kch_lines = NULL;
static unsigned char *kch_length = NULL;
static size_t kch_capacity = 0;

static void alloc_kch_lines(size_t sz)
{
    unsigned long x;
    
    for (x = 0; x < kch_capacity; x++) {
	if (kch_lines[x] != NULL) {
	    free(kch_lines[x]);
	    kch_lines[x] = NULL;
	}
	kch_length[x] = 0;
    }
    if (kch_capacity >= sz)
	return;

    if (kch_lines) {
	free(kch_lines);
	free(kch_length);
    }
    kch_lines = calloc(sz+1, sizeof(kanachar *));
    kch_length = calloc(sz+1, sizeof(unsigned char));
    kch_capacity = sz;
}

static void byte2kchar(const char *buf, int index)
{
	kanachar *kp, k0, kv;
	int i, x;
	int len = strlen(buf) + 1;
	kp = malloc(sizeof(kanachar) * len);
	for (i = x = 0; x < len; i++) {
		k0 = buf[x++] & 0xff;
		if (k0 == 0)
			break;
		if ((k0 & 0x80) == 0) {
			kp[i] = k0;
		}else {
			kv = (k0 << 8) | (buf[x++] & 0xff);
#if (KANA_CODE == UTF_8)
			if (k0 >= 0xe0) {
				kv = (kv << 8) | (buf[x++] & 0xff);
				if (k0 > 0xef)
					kv = (kv << 8) | (buf[x++] & 0xff);
			}
#endif
			kp[i] = kv;
		}    
	}
	kp[i] = 0;
	kch_lines[index] = kp;
	kch_length[index] = i;
}

static int display_lesson(BoolType drill, BoolType ruby)
{
	int i, x, ln, upper;
	int count = 0;

	alloc_kch_lines(newStart - exerciseStart);
	upper = newStart;
	if (ruby) --upper;
	for (i = exerciseStart, ln = 3; i < upper; i++, ln++) {
		move_cursor(ln, 0);
		add_str(cached_lines[i]);
		if (ruby) {
			++ln, ++i;
			move_cursor(ln, 0);
			add_str(cached_lines[i]);
		}
		x = i - exerciseStart;
		byte2kchar(cached_lines[i], x);
		count += kch_length[x];
		if (drill) {
			++count; /* +1 for NULL */
			++ln;
		}
	}
	return count;
}

static void clear_chars(int n)
{
	int i;
	for (i = 0; i < n; i++)
		put_bs(' ');
}

static BoolType 
match_kchars(r2k_info *p, const kanachar *text, BoolType quick,
    int *kcountp, char *dp)
{
    int i, err = 0;
    kanachar typk, othr;
    BoolType atNL = FALSE;

    *kcountp = 0;
    for (i = 0; i < p->kidx; i++, ++(*kcountp)) {
	if (text[i] == 0) {
	    atNL = TRUE;
	    break;
	}
	if ((typk = p->kbuf[i]) == text[i])
	    add_kch(typk); /* OK */
	else {
	    othr = 0;
	    if (isYoon(typk, &othr) && othr == text[i])
		add_kch(typk); /* OK */
	    else if (i > 0 && err && othr && !isYoon(text[i], NULL)) {
		(void)resetKanaInput(p);
		return FALSE;
		/* Because YO'ON typed by mistake generates 2 chars */
	    }else {
			if (quick) {
#ifdef VANISHCHAR
				if (dp) dp[i] = 1;
				add_str("XX");
#else
				stand_out(STANDOUT);
				add_kch(text[i]);
				stand_end(STANDOUT);
#endif
			}else
				add_str("XX");
			err++;
	    }
	}
    }
    if (atNL || (p->ridx > 0 && text[i] == 0)) {
	/* because CR at EOL may determine 'N' */
	set_end_line(' ');
	(void)resetKanaInput(p);
    }
    return (err == 0);
}

static kanachar kana_match(int ch, kanachar current)
#if defined(KANA_LESSON)
{
	if (current == kanamap[ch])
		return current;
	if (ch == '\\' && current == kchars[s_bar])
		return kchars[s_bar];
#ifdef MACKEYBOARD
	if (ch == '0' && current == kchars[s_wo])
		return kchars[s_wo];
	if (ch == '_' && current == kchars[s_ro])
		return kchars[s_ro];
#endif
	return 0; /* Error */
}
#else
{	return current; }
#endif /* KANA_LESSON */

void do_kana(BoolType ruby, BoolType roman)	/* \K, Kana drill */
{
    int ch, i, j, x, ln;
    int tries, oncemore;
    int errors;
    int totalChars, strokes;
    int thisLength;
    long startTime = 0L, endTime;
    kanachar *thisLine;
    kanachar k;
    r2k_info *rkinfo;
    int qchars, cv, kc;
    
    rkinfo = resetKanaInput(NULL);
    qchars = 0;
    for (tries = 0; ; tries++) { /* a try */

	/* display drill pattern */
	totalChars = display_lesson(TRUE, ruby);

	/* run the drill */
	errors = 0;
	strokes = 0;
	for (i = exerciseStart, ln = 4; i < newStart; i++, ln += 2) {
	    if (ruby) {
		ln++;
		i++;
	    }
	    set_end_line(0);
	    move_cursor(ln, 0);
	    x = i - exerciseStart;
	    thisLine = kch_lines[x];
	    thisLength = kch_length[x];

	    for (j = 0; j < thisLength; ) {
		ch = get_kch();
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
		if (roman) {
		    cv = roman2kana(ch, rkinfo);
		    if (cv != 0) { /* Error roman -> kana */
			if (qchars) {
			    clear_chars(qchars);
			    qchars = 0;
			    add_str("XX");
			    ++j;
			}
			rkinfo = resetKanaInput(rkinfo);
			if (loud) sndbeep();
			errors++;
		    }else if (rkinfo->kidx > 0) {
			if (qchars) {
			    clear_chars(qchars);
			    qchars = 0;
			}
			if (!match_kchars(rkinfo, &thisLine[j], FALSE, &kc, NULL)) {
			    if (loud) sndbeep();
			    errors++;
			}else if (!convKana(rkinfo)) {
			    add_ch(ch);
			    qchars++;
			}
			j += kc;
			rkinfo->kidx = 0;
		    }else {
			add_ch(ch);
			qchars++;
		    }
		}else { /* not roman */
		    k = kana_match(ch, thisLine[j]);
		    if (k)
			add_kch(k);
		    else {
			add_str("XX");
			if (loud) sndbeep();
			errors++;
		    }
		    ++j;
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
	add_str("‚à‚¤‚P‰ñ...");
#else
	add_str("Try again...");
#endif
    }
    return;

    ALL_DONE:
	SkipLesson = TRUE;
}


void do_quick(BoolType ruby, BoolType roman)	/* \Q, quick typing for Kana */
{
    int ch, i, j, x, ln;
    kanachar *thisLine;
    kanachar k;
    int thisLength;
    int totalChars, strokes;
    int errors;
    int oncemore = 0;
    long startTime = 0L, endTime;
    r2k_info *rkinfo;
    int qchars, cv, kc;
    char *dp = NULL;
#ifdef VANISHCHAR
    StrType	damn;
    int		dx;
#endif
    
    rkinfo = resetKanaInput(NULL);
    qchars = 0;
    do {
	if (oncemore) {
	    clear_screen();
	    move_cursor(0, 0);
#ifdef JPN
	    add_str("‚à‚¤‚P‰ñ...");
#else
	    add_str("Try again...");
#endif
	}

	/* print out practice text */
	totalChars = display_lesson(FALSE, ruby);

	strokes = 0;
	errors = 0;
	for (i = exerciseStart, ln = 3; i < newStart; i++, ln++) {
	    if (ruby) {
		ln++;
		i++;
	    }
	    set_end_line(0);
	    move_cursor(ln, 0);
	    x = i - exerciseStart;
	    thisLine = kch_lines[x];
	    thisLength = kch_length[x];
	    for (j = 0; j < thisLength; ) {
		ch = get_kch();
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
#ifdef VANISHCHAR
		damn[j] = 0;
#endif
		/* Allow backspace */
		if (ch == ASCII_BS || ch == ASCII_RUBOUT) {
		    if (qchars > 0) {
			if (qchars & 1)
			    add_ch(' ');
			if (qchars > 2) {
			    if (j+1 >= thisLength) {
				put_bs(' ');
				put_bs(' ');
			    }else
				put_2bs(thisLine[j+1]);
			}
			put_2bs(thisLine[j]);
			qchars = 0;
		    }else if (j > 0) {
			--j;
			put_2bs(thisLine[j]);
		    }
		    rkinfo = resetKanaInput(rkinfo);
		    continue; /* but notice that errors accumulate */
		}
		if (roman) {
		    cv = roman2kana(ch, rkinfo);
		    if (cv != 0) { /* Error roman -> kana */
			if (qchars) {
			    clear_chars(qchars);
			    qchars = 0;
#ifdef VANISHCHAR
			    damn[j] = 1;
			    add_str("XX");
#else
			    stand_out(STANDOUT);
			    add_kch(thisLine[j]);
			    stand_end(STANDOUT);
#endif
			    ++j;
			}
			rkinfo = resetKanaInput(rkinfo);
			if (loud) sndbeep();
			errors++;
		    }else if (rkinfo->kidx > 0) {
			if (qchars) {
			    clear_chars(qchars);
			    qchars = 0;
			}
#ifdef VANISHCHAR
			dp = &damn[j];
#endif
			if (!match_kchars(rkinfo, &thisLine[j], TRUE, &kc, dp)) {
			    if (loud) sndbeep();
			    errors++;
			}else if (!convKana(rkinfo)) {
			    add_ch(ch);
			    qchars++;
#if defined(HALF_KANJI_ERROR)
			    if (qchars & 1) {
				add_ch(' '); put_bs(' ');
			    }
#endif
			}
			j += kc;
			rkinfo->kidx = 0;
		    }else {
			add_ch(ch);
			qchars++;
#if defined(HALF_KANJI_ERROR)
			if (qchars & 1) {
			    add_ch(' '); put_bs(' ');
			}
#endif
		    }
		}else { /* not roman */
		    k = kana_match(ch, thisLine[j]);
		    if (k)
			add_kch(k);
		    else {
#ifdef VANISHCHAR
			damn[j] = 1;
			add_str("XX");
#else
			stand_out(STANDOUT);
			add_kch(thisLine[j]);
			stand_end(STANDOUT);
#endif
			if (loud) sndbeep();
			errors++;
		    }
		    ++j;
		}
	    }
	    get_end_line();
#ifdef VANISHCHAR
	    move_cursor(ln, 0);
	    dx = 0;
	    for (j = 0; j < thisLength; j++) {
		if (damn[j] != dx) {
		    if (dx)
			stand_end(STANDOUT);
		    else
			stand_out(STANDOUT);
		    dx = damn[j];
		}
		add_kch(thisLine[j]);
	    }
	    if (dx) stand_end(STANDOUT);
#endif
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
    clear_screen();
    SkipLesson = TRUE;
}
