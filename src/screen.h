/* ------------------------------------------------------
	screen.h  fot typist
---------------------------------------------------------
	Ver.2.0   1997-05-20	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#include <ctype.h>

# ifdef __STDC__
# include <stdarg.h>
int add_ch(int);
void add_str(char *);
void add_fmt(char *fmt, ... );
void stand_out(int);
void stand_end(int);
# else   /* __STDC__ */
# include <varargs.h>
extern int add_ch();
extern void add_str();
extern void add_fmt();
extern void stand_out();
extern void stand_end();
# endif  /* __STDC__ */

#define  ALLATTR	0
#define  STANDOUT	1
#define  UNDERLINE	2
#define  BOLD		3


#ifdef  MSDOS

/* This header info. is only for MS-DOS terminals. */

#include <conio.h>

#ifdef __STDC__
int get_kch(void);
#else
extern int get_kch();
#endif

#define  init_screen()	{ }
#define  cbreak()	{ }
#define  nocbreak()	{ }
#define  end_screen()	{ }
#define  move(y, x)	add_fmt("\033[%d;%dH", (y)+1, (x)+1)
#define  clear_screen()	add_str("\033[2J")
#define  put_bs(c)	add_fmt("\b%c\b", (c))
#define  put_2bs(s)	add_fmt("\b\b%c%c\b\b", *(s), *((s)+1))
#define  get_ch()	getch()

#else /* MSDOS */

#ifdef __STDC__
void raw_mode(int);
void init_screen(void);
void end_screen(void);
void clear_screen(void);
void goto_screen(int, int);
void put_bs(int);
void put_2bs(char *);
int get_ch(void);
#else
extern void raw_mode();
extern void init_screen();
extern void end_screen();
extern void clear_screen();
extern void goto_screen();
extern void put_bs();
extern void put_2bs();
extern int get_ch();
#endif

#define  init_scr()	init_screen()
#define  cbreak()	raw_mode(1)
#define  nocbreak()	raw_mode(0)
#define  move(y, x)	goto_screen(y, x)
#define  get_kch()	get_ch()

#endif /* not MSDOS */
