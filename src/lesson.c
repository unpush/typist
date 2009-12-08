/* ------------------------------------------------------
	typist

    This program will print out exercise lines to the
    display, and then check for correct typing.
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.2.0   1997-05-17	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "typist.h"
#include "lineio.h"
#include "screen.h"

char *get_lesson()	/* Ask user for desired lesson */
{
    static StrType response;
    int ch, n, item;
    struct Lesson *p;
    char *lp;

    if (LastLesson) {
	ch = LastLesson;
	n = (LastNum == FINISHED)? 1: (LastNum + 1);
    }else {
	ch = typeLessons[0].nam;
	n = 1;
    }

    move(0,0);
#ifdef JPN
    add_str("  以下からコースを選択して下さい:");
    item = 0;
    for (p = typeLessons; p->item; p++) {
	move(++item, 0);
	add_fmt("        %s (%c1 - %c%d) ", p->item, p->nam, p->nam, p->num);
	if (p->fin >= p->num)
	    add_str(" !!!終了!!!");
	else if (p->fin > 0)
	    add_fmt(" [%c%dまで終了]", p->nam, p->fin);
    }
    move(++item, 0);
    add_fmt(" 練習したいレッスンの名前(例 %c%d)を入力して下さい", ch, n);
    if (HasHelp)
	add_str("\n (終了=改行、ヘルプ=?) ------> ");
    else
	add_str("(終了=改行): ");
#else
    add_str("  Several lessons are available:");
    item = 1;
    for (p = typeLessons; p->item; p++) {
	move(item++, 8);
	add_fmt("%s (%c1 - %c%d) ", p->item, p->nam, p->nam, p->num);
	if (p->fin >= p->num)
	    add_str(" !!!Finish!!!");
	else if (p->fin > 0)
	    add_fmt(" [%c%d was done]", p->nam, p->fin);
    }
    move(++item, 0);
    add_fmt(" Type the desired lesson name(e.g. %c%d).\n", ch, n);
    add_str(" (Quit=RETURN) ------------> ");
#endif

    nocbreak();

    (void)gets(response);

    cbreak();
    for (lp = response; *lp == ' ' || *lp == '\t'; lp++) ;
    if (*lp == 0)
	cleanup(0);	/* EXIT */
    if (HasHelp && *lp == '?')
	return "?1";	/* HELP FILE */

    return lp;
}


BoolType find_lesson(name, cached_lines)
/* locate given lesson, leave lesson_file open so next line begins */
    char *name;
    char **cached_lines;
{
    StrType fullName;
    StrType current_line;
    BoolType done;
    FILE *lesson_file;
    int linesInLesson;
    int i, idx, lng;
    static char empty[2] = { '\0', '\0' };	/* explicit NULL string */

    if ((idx = check_lesson_name(name)) < 0) {
#ifdef JPN
	add_fmt("\n \"%s\" というレッスンはありません.\n", name);
#else
	add_fmt("\n %s: No such Lesson.\n", name);
#endif
	(void) wait_user();
	return FALSE;
    }
    /* gf: Added LessonDir */
    if (typeLessons[idx].path == NULL)
	sprintf(fullName, "%s%c%c.typ", LessonDir, DELIM, name[0]);
    else
	sprintf(fullName, "%s%c%s.typ",
			LessonDir, DELIM, typeLessons[idx].path);
    lesson_file = fopen(fullName, "r");
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
    for (i = 0; i < MAX_LINES && cached_lines[i]; i++) {
	if (cached_lines[i] != empty)
	    free(cached_lines[i]);
	cached_lines[i] = NULL;
    }
    linesInLesson = 0;
    done = FALSE;
    while (!done) {
	if ((lng = get_lesson_line(lesson_file, current_line)) < 0)
	    break;	/* EOF or End of Lesson */
	if (lng == 0)
	    cached_lines[linesInLesson] = empty;
	else {
	    cached_lines[linesInLesson] = (char *)malloc(lng + 1);
	    if (cached_lines[linesInLesson] == NULL) {
		fprintf(stderr, "SYSTEM ERROR: malloc() failure\n");
		cleanup(1);
	    }
	}
	strcpy(cached_lines[linesInLesson], current_line);
	if (++linesInLesson >= MAX_LINES - 1)
	    goto ERR_EXIT;
    }
    /* The next of the last line, cached_lines[linesInLesson] should be NULL */

    fclose(lesson_file);
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
	fclose(lesson_file);
	return FALSE;
}
