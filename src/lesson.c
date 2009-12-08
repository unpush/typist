/* ------------------------------------------------------
	typist

    This program will print out exercise lines to the
    display, and then check for correct typing.
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.2.0   1997-05-17	Kana Exercise
	Ver.3.0   2007-08-24	by Takeshi Ogihara
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "typist.h"
#include "fileinfo.h"
#include "lineio.h"
#include "dictionary.h"
#include "screen.h"
#include "http.h"

#define CAPACITY_INC	200

char **cached_lines = NULL;
char *cached_attr = NULL;
static size_t cacheCapacity = 0;
static char empty[2] = { '\0', '\0' };	/* explicit NULL string */

static void clear_cache(void)
{
    size_t i;

    if (cached_lines) {
	for (i = 0; i < cacheCapacity && cached_lines[i]; i++) {
	    if (cached_lines[i] != empty)
		free(cached_lines[i]);
	    cached_lines[i] = NULL;
	    cached_attr[i] = 0;
	}
    }else {
	cacheCapacity = CAPACITY_INC;
	cached_lines = calloc(cacheCapacity, sizeof(char *));
	cached_attr = calloc(cacheCapacity, sizeof(char));
    }
}

static void increse_cache(void)
{
    size_t i;
    size_t newCapacity = cacheCapacity + CAPACITY_INC;
    char **cache = calloc(newCapacity, sizeof(char *));
    char *attr = calloc(newCapacity, sizeof(char));
    for (i = 0; i < cacheCapacity; i++) {
	cache[i] = cached_lines[i];
	attr[i] = cached_attr[i];
    }
    free(cached_lines);
    free(cached_attr);
    cached_lines = cache;
    cached_attr = attr;
    cacheCapacity = newCapacity;
}

const char *get_lesson(void)	/* Ask user for desired lesson */
{
    static StrType response;
    int ch, n, i, item;
    struct Lesson *p;
    char *lp;

    if (LastLesson) {
	ch = LastLesson;
	n = (LastNum == FINISHED)? 1: (LastNum + 1);
    }else {
	ch = typeLessons[0].nam;
	n = 1;
    }

    move_cursor(0, 0);
#ifdef JPN
    add_str("  以下からコースを選択して下さい:");
    item = 0;
    for (p = typeLessons; p->item; p++) {
	move_cursor(++item, 0);
	add_fmt("        %s (%c1 - %c%d) ", p->item, p->nam, p->nam, p->num);
	if (p->fin >= p->num)
	    add_str(" !!!終了!!!");
	else if (p->fin > 0)
	    add_fmt(" [%c%dまで終了]", p->nam, p->fin);
    }
    move_cursor(++item, 0);
    add_fmt(" 練習したいレッスンの名前(例 %c%d)を入力して下さい", ch, n);
	if (HasHelp) {
		move_cursor(item+1, 0);
		add_str(" (終了=改行、ヘルプ=?) ------> ");
	}else
		add_str(" (終了=改行): ");
#else
    add_str("  Several lessons are available:");
    item = 1;
    for (p = typeLessons; p->item; p++) {
	move_cursor(item++, 8);
	add_fmt("%s (%c1 - %c%d) ", p->item, p->nam, p->nam, p->num);
	if (p->fin >= p->num)
	    add_str(" !!!Finish!!!");
	else if (p->fin > 0)
	    add_fmt(" [%c%d was done]", p->nam, p->fin);
    }
    move_cursor(++item, 0);
    add_fmt(" Type the desired lesson name(e.g. %c%d).", ch, n);
    move_cursor(item+1, 0);
    add_str(" (Quit=RETURN) ------------> ");
#endif

    cbreak_mode(0);

    (void)fgets(response, STR_SIZE, stdin);  /* avoid gets() */

    cbreak_mode(1);
    for (i = 0; i < STR_SIZE && response[i] != '\n' && response[i] != '\r'; i++)
	;
    response[i] = 0;
    for (lp = response; *lp == ' ' || *lp == '\t'; lp++)
	;
    if (*lp == 0)
	cleanup(0);	/* EXIT */
    if (HasHelp && *lp == '?')
	return "?1";	/* HELP FILE */

    return lp;
}


BoolType find_lesson(const char *name)
/* locate given lesson, leave lesson_file open so next line begins */
{
    StrType fullName;
    StrType current_line;
    char slash;
    FILEsIO *lesson_file;
    size_t linesInLesson;
    int idx, lng, kind;

    if ((idx = check_lesson_name(name)) < 0) {
	stand_out(BOLD);
#ifdef JPN
	add_fmt("\n \"%s\" というレッスンはありません.\n", name);
#else
	add_fmt("\n %s: No such Lesson.\n", name);
#endif
	stand_end(BOLD);
	(void) wait_user();
	return FALSE;
    }
    /* gf: Added LessonDir */
    slash = isHttp ? '/' : DELIM;
    if (typeLessons[idx].path == NULL)
	sprintf(fullName, "%s%c%c.typ", LessonDir, slash, name[0]);
    else
	sprintf(fullName, "%s%c%s.typ",
			LessonDir, slash, typeLessons[idx].path);
    lesson_file = isHttp ? open_url(fullName)
		: makeFilesio( fopen(fullName, "r") );
    if (lesson_file == NULL) {
	stand_out(BOLD);
	add_fmt("OPEN ERROR: %s (errno=%d)\n", fullName, errno);
	stand_end(BOLD);
	(void) wait_user();
	return FALSE;
    }

    if (!seek_lesson(lesson_file, name))
	goto ERR_EXIT;

    /* clear previous lesson */
    clear_dictionary();
    clear_cache();
    linesInLesson = 0;
    while ((lng = get_lesson_line(lesson_file, current_line)) >= 0) {
	/* Loop ends if EOF or End of Lesson */
	kind = 0;
	if (lng >= 2 && current_line[lng - 2] == '\\') { /* expected terminator */
	    kind = current_line[lng - 1];
	    current_line[lng -= 2] = '\0'; /* remove */
	}
	if (lng == 0)
	    cached_lines[linesInLesson] = empty;
	else {
	    char *pp = (char *)malloc(lng + 1);
	    if (pp == NULL) {
		fprintf(stderr, "SYSTEM ERROR: malloc() failure\n");
		cleanup(1);
	    }
	    strcpy(pp, current_line);
	    cached_lines[linesInLesson] = pp;
	}
	cached_attr[linesInLesson] = kind;
	if (++linesInLesson >= cacheCapacity - 1)
	    increse_cache();
    }
    /* The next of the last line, cached_lines[linesInLesson] should be NULL */

    ioclose(lesson_file);
    return TRUE;

ERR_EXIT:
	stand_out(BOLD);
#ifdef JPN
	add_fmt("レッスン%s は利用できません.\n", &name[1]);
#else
	add_fmt("Lesson %s is not available.\n", &name[1]);
#endif
	stand_end(BOLD);

	(void) wait_user();
	ioclose(lesson_file);
	return FALSE;
}
