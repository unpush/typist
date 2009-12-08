/* ------------------------------------------------------
	typist

    This program will print out exercise lines to the
    display, and then check for correct typing.
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.2.0   1997-04-21	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#ifndef  __DEF_LINEIO__
#define  __DEF_LINEIO__

#define  L_EOF		(-1)	/* EOF */
#define  L_EOLESSON	(-2)	/* EOF */
#define  L_IFT		(-3)	/* #if / #ifnot (true) */
#define  L_IFF		(-4)	/* #if / #ifnot (false) */
#define  L_ELT		(-5)	/* #elif (true) and #else */
#define  L_ELF		(-6)	/* #elif (false) */
#define  L_END		(-7)	/* #endif */

# ifdef __STDC__
BoolType seek_lesson(FILE *, char *);
int get_lesson_line(FILE *, char *);
#else
extern BoolType seek_lesson();
extern int get_lesson_line();
#endif

#endif  /* __DEF_LINEIO__ */
