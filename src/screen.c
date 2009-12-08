/* ------------------------------------------------------
	screen.c  fot typist
---------------------------------------------------------
	Ver.2.0   1997-05-20	Kana Exercise
	by Takeshi Ogihara
	Modification for MinGW   May 2007   by OHKUBO Takuya
	Ver.3.0   2007-07-19	by Takeshi Ogihara
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#if !defined(__BORLANDC__)
# include <libc.h>
#endif

#include "screen.h"
#include "base.h"

#if !defined(__MINGW32__) && !defined(__BORLANDC__)

/* write 1 char into 2nd channel */
int add_ch(int cc)
{
	char buf;

	buf = cc & 0xff;
	write(2, &buf, 1);
	return 0;
}

void add_str(const char *s)
{
	int i;

	for (i = 0; s[i]; i++) ;
	if (i)
		write( 2, s, i );
}

#endif /* !defined(__MINGW32__) */


/* write 1 Unicode char or Kana char */
void add_kch(unsigned long kk)
#if (KANA_CODE == UTF_8)
{
	int sft, x, ch;
	char buf[4];

	for (sft = 24, x = 0; sft >= 0; sft -= 8) {
		ch = (kk >> sft) & 0xff;
		if (ch)
			buf[x++] = ch;
	}
	buf[x] = 0;
	if (x > 0)
		add_str(buf);
}
#else
{
	char buf[4];
	buf[0] = (kk >> 8) & 0xff;
	buf[1] = kk & 0xff;
	buf[2] = 0;
	add_str(buf);
}
#endif

#if !defined(__BORLANDC__)
void add_fmt(const char *fmt, ... )
{
	va_list argptr;
	char buffer[STR_SIZE];

	va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);
	add_str(buffer);
	va_end(argptr);
}
#endif

#if defined(MSDOS)
/* =================================================== */

#include <dos.h>

/*
    MS-DOS のファンクションコールによってキーボード入力を調べる。
    「を」のキー（Shift+0）は英数モードではキャラクタが入力できない。
    これをファンクションコールで調べると、00 と 0b の連続として知る
    ことができる。この時に、'~' を文字として返すことにする。
*/

int get_kch(void)
{
	union REGS inregs, outregs;

	inregs.h.ah = 0x08;	/* ファンクション08H */
	intdos( &inregs, &outregs );
	if ( outregs.h.al )	/* 通常の文字 */
		return ((int)outregs.h.al & 0xff);
	inregs.h.ah = 0x08;	/* ファンクション08H */
	intdos( &inregs, &outregs );
	if ( outregs.h.al == 0x0b )
		return '~';
	return ' ';
}

void stand_out(int kind)
{
	switch (kind) {
	case STANDOUT:
		add_str("\033[7m");
		break;
	case UNDERLINE:
		add_str("\033[4m");
		break;
	case BOLD:
		add_str("\033[1m");
		break;
	}
}

#endif /* end of MSDOS */


#if defined(__BORLANDC__)
/* =================================================== */

static int txtcolor = 0x07; /* Lightgray on Black */
static int revcolor = 0x70; /* Black on Lightgray */
static int stdcolor = 0x0e;

void init_screen(void)
{
    struct text_info ti;
    int c;

    gettextinfo(&ti);
    if ((c = ti.attribute) != 0) {
	txtcolor = c;
	revcolor = ((c & 0xF0) >> 4) | ((c & 0x0F) << 4);
	if ((c & 0x07) == 0x07) /* White or Lightgray text */
	    stdcolor = (c & 0xf0) | 0x0e; /* Yellow text */
	else
	    stdcolor = 0xe0; /* Black on Yellow */
    }
}

void stand_out(int kind)
{
	switch (kind) {
	case STANDOUT:
		textattr(stdcolor);
		break;
	case UNDERLINE:
		textattr(0xc0); /* Black on Red */
		break;
	case BOLD:
		textattr(revcolor);
		break;
	}
}

void stand_end(int kind)
{
    textattr(txtcolor);
}

#endif /* end of MSDOS */


#if defined(UNIX_CURSES)
/* =================================================== */

#define  TERMENTRY	2048
#define  TERMBUFFER	1024
#define  MIN_WIDTH	80
#define  MIN_HEIGHT	24

#ifdef TERMIO
# include <termio.h>
#else
# include <sgtty.h>
#endif

#ifdef TIOCGWINSZ
# include <sys/ioctl.h>
#endif

char *tgetstr(char *, char **);
char *tgoto(const char *, int, int);
int tgetnum(char *);
int tgetflag(char *);
int tgetent(char *, const char *);
int tputs(const char *, int, int (*)());


/* Terminal control strings */
static char	*tc_pad;	/* Padding */
static char	*tc_move;	/* Move cursor */
static char	*tc_clear;	/* Clear screen */
static char	*tc_so_on;	/* Standout ON */
static char	*tc_so_off;	/* Standout OFF */
static char	*tc_ul_on;	/* Underline ON */
static char	*tc_ul_off;	/* Underline OFF */
static char	*tc_bo_on;	/* Bold ON */
static char	*tc_bo_off;	/* Bold OFF */
static char	*tc_backspace;	/* Backspace */
static char	*tc_init;	/* Terminal initialize */
static char	*tc_restore;	/* Exit terminal de-intialization */
extern char	PC;		/* Padding char */

static int screen_width = MIN_WIDTH, screen_height = MIN_HEIGHT;


#ifdef TERMIO

void raw_mode(int sw)
{
	static int	firsttime = 1;
	struct termio *s;
	static struct termio save_term;
	static struct termio raw_term;

	if (sw) {
		s = &raw_term;
		if (firsttime) {
# ifdef TCGETA
			ioctl(2, TCGETA, s);	/* Get terminal info. */
# else
			tcgetattr(2, s);
# endif
			firsttime = 0;
			save_term = *s;
			s->c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL);
			s->c_oflag |=  (OPOST|ONLCR);
			s->c_oflag &= ~(OCRNL|ONOCR|ONLRET);
			s->c_cc[VMIN] = 1;
			s->c_cc[VTIME] = 0;
		}
	}else if (firsttime)
		return;
	else
		s = &save_term;
# ifdef TCSETAW
	ioctl(2, TCSETAW, s);	/* Set terminal info. */
# else
	tcsetattr(2, TCSANOW, s);
# endif
}

#else /* not TERMIO */

void raw_mode(int sw)
{
	static int	firsttime = 1;
	struct sgttyb *s;
	static struct sgttyb save_term;
	static struct sgttyb raw_term;

	if (sw) {
		s = &raw_term;
		if (firsttime) {
			ioctl(2, TIOCGETP, s);	/* Get terminal info. */
			firsttime = 0;
			save_term = *s;
			s->sg_flags |= CBREAK;
			s->sg_flags &= ~(ECHO|XTABS
#ifdef  LCRTKIL
				| LCRTKIL
#endif
			);
		}
	}else if (firsttime)
		return;
	else
		s = &save_term;
	ioctl(2, TIOCSETN, s);	/* Set terminal info. */
}
#endif /* TERMIO */


static int get_term_size(void)
{
#ifdef TIOCGWINSZ
	struct winsize w;

	screen_height = (ioctl(2, TIOCGWINSZ, &w) == 0)
		? w.ws_row : tgetnum("li");
 	screen_width = (ioctl(2, TIOCGWINSZ, &w) == 0)
		? w.ws_col : tgetnum("co");
#else
	screen_height = tgetnum("li");
 	screen_width = tgetnum("co");
#endif
	if (screen_height <= 0) screen_height = MIN_HEIGHT;
	if (screen_width <= 0) screen_width = MIN_WIDTH;
	if (screen_height < MIN_HEIGHT || screen_width < MIN_WIDTH) {
		fprintf(stderr, "ERROR: Window size(%dx%d) is too small.\n",
			screen_width, screen_height);
		fprintf(stderr, "       %dx%d is needed.\n",
			MIN_WIDTH, MIN_HEIGHT);
		return -1;
	}
	return 0;
}

/* Get terminal information from termcap. */
static void get_term(void)
{
	char termentry[TERMENTRY];
	static char buffer[TERMBUFFER];
	char *term, *sp, *w;

 	term = w = getenv("TERM");
	if (term == NULL) {
		w = "vt100";	/* VT100 is default */
		term = "none";
	}
 	if (tgetent(termentry, w) <= 0)
 		strcpy(termentry, "dumb:co#80:li#24:");
	if (get_term_size() != 0)
		goto FATAL;

	sp = buffer;
	if ((tc_pad = tgetstr("pc", &sp)) != NULL)
		PC = *tc_pad;
	if ((tc_init = tgetstr("ti", &sp)) == NULL)
		tc_init = "";
	if ((tc_restore = tgetstr("te", &sp)) == NULL)
		tc_restore = "";

	tc_clear = tgetstr("cl", &sp);
	if (tc_clear == NULL || *tc_clear == 0) {
		fprintf(stderr, "ERROR: can't clear screen (term=%s)\n", term);
		tc_clear = "\f\n";	/* Formfeed */
	}
	tc_move = tgetstr("cm", &sp);
	if (tc_move == NULL || *tc_move == 0) {
		fprintf(stderr, "ERROR: can't move cursor (term=%s)\n", term);
		goto FATAL;
	}

	w = NULL;
	if ((tc_so_on = tgetstr("so", &sp)) == NULL) {
		tc_so_on = tc_so_off = "";
	}else if ((tc_so_off = tgetstr("se", &sp)) != NULL)
		w = tc_so_off;
	if ((tc_ul_on = tgetstr("us", &sp)) == NULL) {
		tc_ul_on = tc_so_on;
		tc_ul_off = tc_so_off;
	}else if ((tc_ul_off = tgetstr("ue", &sp)) != NULL)
		w = tc_ul_off;
	if ((tc_bo_on = tgetstr("md", &sp)) == NULL) {
		tc_bo_on = tc_so_on;
		tc_bo_off = tc_so_off;
	}else if ((tc_bo_off = tgetstr("me", &sp)) != NULL)
		w = tc_bo_off;
	if (w == NULL) {
		tc_so_on = tc_ul_on = tc_bo_on = "";
		tc_so_off = tc_ul_off = tc_bo_off = "";
	}else {
		if (tc_so_off == NULL) tc_so_off = w;
		if (tc_ul_off == NULL) tc_ul_off = w;
		if (tc_bo_off == NULL) tc_bo_off = w;
	}

	if (tgetflag("bs"))
		tc_backspace = "\b";
	else {
		tc_backspace = tgetstr("bc", &sp);
		if (tc_backspace == NULL || *tc_backspace == 0) {
			tc_backspace = tgetstr("le", &sp);
			if (tc_backspace == NULL || *tc_backspace == 0)
				tc_backspace = "\b";
		}
	}
	return;
FATAL:
	exit(1);
}


void init_screen(void)
{
	get_term();
	tputs(tc_init, screen_height, add_ch);
}

void end_screen(void)
{
	tputs(tc_restore, screen_height, add_ch);
	raw_mode(0);
}

void stand_out(int kind)
{
	switch (kind) {
	case STANDOUT:
		tputs(tc_so_on, 1, add_ch);
		break;
	case UNDERLINE:
		tputs(tc_ul_on, 1, add_ch);
		break;
	case BOLD:
		tputs(tc_bo_on, 1, add_ch);
		break;
	}
}

void stand_end(int kind)
{
	switch (kind) {
	case STANDOUT:
		tputs(tc_so_off, 1, add_ch);
		break;
	case UNDERLINE:
		tputs(tc_ul_off, 1, add_ch);
		break;
	case ALLATTR: /* turn off all attribute */
	case BOLD:
		tputs(tc_bo_off, 1, add_ch);
		break;
	}
}

void put_bs(int cc)
{
	tputs(tc_backspace, 1, add_ch);
	add_ch(cc);
	tputs(tc_backspace, 1, add_ch);
}

void goto_screen(int y, int x)
{
	tputs(tgoto(tc_move, x, y), screen_height, add_ch);
}

void clear_screen(void)
{
	tputs(tgoto(tc_move, 0, 0), screen_height, add_ch);
	tputs(tc_clear, screen_height, add_ch);
}

int get_ch(void)
{
	unsigned char ch;

	read(0, (char *)&ch, 1);
	return (int)ch;
}

/* =================================================== */
#endif /* UNIX_CURSES */


#if defined(UNIX_CURSES)

void put_2bs(unsigned long kk)
{
	tputs(tc_backspace, 1, add_ch);
	tputs(tc_backspace, 1, add_ch);
	add_kch(kk);
	tputs(tc_backspace, 1, add_ch);
	tputs(tc_backspace, 1, add_ch);
}

#else

void put_2bs(unsigned long kk)
{
	add_str("\b\b");
	add_kch(kk);
	add_str("\b\b");
}

#endif /* UNIX_CURSES */


#ifdef __TEST
int main(void) {
	char cc;

	init_screen();
	raw_mode(1);
	read(0, &cc, 1);
	goto_screen(5, 20);
	add_ch(cc);
	fprintf(stderr, "漢字\n");
	raw_mode(0);
	goto_screen(23, 0);
	end_screen();
	return 0;
}
#endif

