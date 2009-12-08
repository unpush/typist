/* ------------------------------------------------------
	screen.h  fot typist
---------------------------------------------------------
	Ver.2.0   1997-05-20	Kana Exercise
	by Takeshi Ogihara
	Modification for MinGW    May 2007   by OHKUBO Takuya
	Ver.3.0   2007-07-19	by Takeshi Ogihara
------------------------------------------------------ */

#include <ctype.h>
#include <stdarg.h>

/* ==================================
Function/Macro List

int add_ch(int);
void add_str(const char *);
void add_kch(unsigned long);
void add_fmt(const char *fmt, ... );
int get_kch(void);
void stand_out(int);
void stand_end(int);

void init_screen();
void end_screen();
void cbreak_mode();
void move_cursor(int y, int x);
void put_bs(int c);
void clear_screen();
void get_ch();
void put_2bs(unsigned long);

================================== */

/* This code allows 4 types of terminal control:
  1. old-type DOS
  2. Borland C++ (and other C on Windows, maybe)
  3. MinGW (Minimalist GNU for Win32)
  4. UNIX like curses & termio

!! Definition of functions and macros are separated because of simplicity.

*/
#if !defined(MSDOS) && !defined(__BORLANDC__) && !defined(__MINGW32__)
#define UNIX_CURSES
#endif

#define  ALLATTR	0
#define  STANDOUT	1
#define  UNDERLINE	2
#define  BOLD		3

/* ===================== MSDOS ===================== */
#if defined(MSDOS)
#include <conio.h>
#include <io.h>

int add_ch(int);
void add_str(const char *);
void add_kch(unsigned long);
void add_fmt(const char *fmt, ... );
int get_kch(void);
void stand_out(int);
#define  stand_end(x)	add_str("\033[m")

#define  init_screen()	{ }
#define  end_screen()	{ }
#define  cbreak_mode(x)	{ }
#define  move_cursor(y, x)	add_fmt("\033[%d;%dH", (y)+1, (x)+1)
#define  put_bs(c)	add_fmt("\b%c\b", (c))
#define  clear_screen()	add_str("\033[2J")
#define  get_ch()	getch()
void put_2bs(unsigned long);

#endif /* end of MSDOS */

/* ==================== BORLANDC ==================== */
#if defined(__BORLANDC__)
#include <conio.h>
#include <io.h>

#define  add_ch(cc)	putch(cc)
#define  add_str(x)	cputs(x)
void add_kch(unsigned long);
#define add_fmt		cprintf
#define get_kch(void)	getch()
void init_screen(void);
void stand_out(int);
void stand_end(int);

#define  cbreak_mode(x) { }
#define  end_screen()	{ }
#define  move_cursor(y, x)	gotoxy((x)+1, (y)+1)
#define  put_bs(c)	cprintf("\b%c\b", (c))
#define  clear_screen()	clrscr()
#define  get_ch()	getch()
void put_2bs(unsigned long);

#endif /* end of __BORLANDC__ */

/* ==================== MINGW32 ==================== */
#if defined(__MINGW32__)
#include <curses.h>

#define  add_ch(c)      do { addch(c);  refresh(); } while(0)
#define  add_str(s)     do { addstr(s); refresh(); } while(0)
void add_kch(unsigned long);
void add_fmt(const char *fmt, ... );
#define  get_kch()      getch()
#define  stand_out(c)   standout()
#define  stand_end(c)   standend()

#define  init_screen()	initscr()
#define  end_screen()	endwin()
#define  cbreak_mode(x) ((x) ? cbreak() : nocbreak())
#define  move_cursor(y, x)	do { move(y, x); refresh(); } while(0)
#define  put_bs(c)      add_fmt("\b%c\b", (c))
#define  clear_screen()	clear()
#define  get_ch()       getch()
void put_2bs(unsigned long);

#define  gets(s)        do { echo(); getstr(s); noecho(); } while(0)

#endif /* end of __MINGW32__ */

/* ================== UNIX_CURSES ================== */
#if defined(UNIX_CURSES)
#include <curses.h>

void raw_mode(int);
void goto_screen(int, int);

int add_ch(int);
void add_str(const char *);
void add_kch(unsigned long);
void add_fmt(const char *fmt, ... );
#define  get_kch()	get_ch()
void stand_out(int);
void stand_end(int);

void init_screen(void);
void end_screen(void);

#define  cbreak_mode(x) raw_mode(x)
#define  move_cursor(y, x)	goto_screen(y, x)

void put_bs(int);
void clear_screen(void);
int get_ch(void);
void put_2bs(unsigned long);

#endif /* end of UNIX_CURSES */

