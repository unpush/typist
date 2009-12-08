/* ------------------------------------------------------
	fileinfo.c	for typist
	1992-07-14  by T.Ogihara (ogihara@seg.kobe-u.ac.jp)
	Ver.1.3   1994-12-09	REPEAT & BELL
	Ver.1.41  1997-04-13	ANSI C
	Ver.2.0   1997-04-21	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typist.h"

#define   RCMAX    256
#define   TLMAX    20

struct Lesson typeLessons[TLMAX];
BoolType HasHelp = FALSE;
char LastLesson = 0;
short LastNum = 1;

extern char *getenv();

static StrType rcpath;
static char buf[RCMAX];
static char origLastLesson = 0;

BoolType get_index()
{
    int i, j, k;
    char *a, *b;
    StrType xpath;
    FILE *rc;
    struct Lesson *p;

    sprintf(xpath, "%s%ctypist.idx", LessonDir, DELIM);
    if ((rc = fopen(xpath, "ra")) == NULL)
	return FALSE;
    i = 0;
    p = typeLessons;
    while(fgets(buf, RCMAX, rc) != NULL) {
	if (buf[0] == '#') /* comment */
	    continue;
	if (buf[0] == '?') { /* Help: must be the last */
	    p->nam = '?';
	    p->num = 1;
	    p->fin = 0;
	    p->item = NULL;
	    p->path = a = (char *)malloc(strlen(buf));
	    for (k = 1; buf[k] > ' '; k++) *a++ = buf[k];
	    *a = 0;
	    HasHelp = TRUE;
	    p++;
	    break;
	}else {
	    p->nam = buf[0];
	    p->num = atoi(&buf[1]);
	    for (j=2; buf[j] != 0 && buf[j] != '\"'; j++) ;
	    if (buf[j] == 0)
		continue;
	    for (k= ++j; buf[k] != 0 && buf[k] != '\"'; k++) ;
	    p->item = a = (char *)malloc(k - j + 1);
	    while (j < k) *a++ = buf[j++];
	    *a = 0;
	    for (j = k+1; buf[j] == ' ' || buf[j] == '\t'; j++) ;
	    for (k = j; (buf[k] & 0xff) > ' '; k++) ;
	    if (k == j)
		p->path = NULL;
	    else {
		p->path = a = (char *)malloc(k - j + strlen(Keytype));
		for ( ; j < k; j++) {
		    if (buf[j] == '%') { /* % は Keytype で置き換える */
		        b = Keytype;
			while (*b) *a++ = *b++;
		    }else
			*a++ = buf[j];
		}
		*a = 0;
#ifdef MSDOS
		for (a = p->path; *a; a++)
		    if (*a == '/') *a = DELIM;
#endif
	    }
	    p->fin = 0;
	}
	i++, p++;
    }
    p->nam = 0;
    p->num = 0;
    p->fin = 0;
    p->item = NULL;
    p->path = NULL;
    fclose(rc);
    return (i>0);
}

int check_lesson_name(s)
    char *s;
{
    int i,n;

    if (s == NULL || s[0] == 0)
	return -1;
    for (i=0; typeLessons[i].nam; i++) {
	if (typeLessons[i].nam == s[0]) {
	    n = atoi(&s[1]);
	    return (n > 0 && n <= typeLessons[i].num)? i: -1;
	}
    }
    return -1;
}

FILE *open_rc()
{
    int i, cc;
    char *h;
    FILE *rc;
    static char key[8];

    if ((h = getenv(TYPDATA)) != NULL) {
	strcpy(rcpath, h);
    }else {
	if ((h = getenv("HOME")) == NULL) {
	    rcpath[0] = 0;
	    return NULL;
	}
	sprintf(rcpath, "%s%c%s", h, DELIM, TYPISTRC);
    }
    if ((rc = fopen(rcpath, "ra")) == NULL)
	return NULL;
    if ((cc = fgetc(rc)) == EOF) {
	fclose(rc);
	return NULL;
    }else if (cc == '*') { /* Version 1.5 or later */
	for (i = 0; (cc = fgetc(rc)) != '\n'; i++)
	    key[i++] = cc;
	key[i] = 0;
	if (key[0])
	    Keytype = key;
    }else
	ungetc(cc, rc);
    return rc;
}

void read_rc(rc)
    FILE *rc;
{
    int ch, n;
    struct Lesson *p;

    if (fgets(buf, RCMAX, rc) == NULL)
	return;
    LastLesson = origLastLesson = buf[0];
    while (fgets(buf, RCMAX, rc) != NULL) {
	ch = buf[0];
	n = atoi(&buf[1]);
	for (p = typeLessons; p->item; p++) {
	    if (ch == p->nam) {
		p->fin = (p->num > n)? n: FINISHED;
		if (ch == LastLesson) LastNum = p->fin;
		break;
	    }
	}
    }
}

void write_rc()
{
    FILE *rc;
    struct Lesson *p;
    int n;

    if (rcpath[0] == 0) return;
    if ((rc = fopen(rcpath, "wa")) == NULL) {
	fprintf(stderr,"ERROR: Can't write into \"%s\".\n", rcpath);
	return;
    }
    if (LastLesson)
	origLastLesson = LastLesson;
    fprintf(rc,"*%s\n", Keytype);
    if (origLastLesson) {
	fprintf(rc,"%c\n", origLastLesson);
	for (p = typeLessons; p->item; p++)
	    if ((n = p->fin) > 0) {
		if (n > p->num) n = p->num;
		fprintf(rc,"%c%d\n", p->nam, n);
	    }
    }
    (void) fclose(rc);
    LastLesson = 0;
}

void update_lesson(lname)
	char *lname;
{
	struct Lesson *p;
	int ch, n;

	ch = lname[0];
	n = atoi(&lname[1]);
	for (p = typeLessons; p->item; p++) {
		if (ch == p->nam) {
			LastLesson = ch;	/* ...think H */
			if (p->num > n) {
				if (p->fin < n) p->fin = n;
				LastNum = n;
			}else
				LastNum = p->fin = FINISHED;
			break;
		}
	}
}
