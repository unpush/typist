/* ------------------------------------------------------
	tutor.h
---------------------------------------------------------
	Ver.3.0   2007-08-01	by Takeshi Ogihara
------------------------------------------------------ */

#ifndef __DEF_TYPIST_LOCAL__
#define __DEF_TYPIST_LOCAL__

#define ASCII_ESC	27
#define ASCII_GS	29
#define CTRL_B		2
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

extern int exerciseStart;
extern int newStart;
extern BoolType BreakLesson, SkipLesson, lessonBack;

void set_end_line(unsigned long);
void get_end_line(void);
void displaySpeed(int, int, long, int);

/* Kana Lessons */
void do_kana(BoolType ruby, BoolType roman);
void do_quick(BoolType ruby, BoolType roman);

#endif /* __DEF_TYPIST_LOCAL__ */
