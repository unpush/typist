/* ------------------------------------------------------
	typist

    This program will print out exercise lines to the
    display, and then check for correct typing.
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.2.0   1997-04-21	Kana Exercise  by Takeshi Ogihara
	Ver.3.0   2007-06-01	by Takeshi Ogihara
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lineio.h"
#include "typist.h"
#include "dictionary.h"
#include "http.h"

#define  L_EOF		(-1)	/* EOF */
#define  L_EOLESSON	(-2)	/* EOF */
#define  L_IFT		(-3)	/* #if / #ifnot (true) */
#define  L_IFF		(-4)	/* #if / #ifnot (false) */
#define  L_ELT		(-5)	/* #elif (true) and #else */
#define  L_ELF		(-6)	/* #elif (false) */
#define  L_END		(-7)	/* #endif */
#define  L_PARAM	(-8)	/* #param */


static int	nestlevel = 0;


static int copy_keystr(const char *buf, char *dest)
{
    int x, i;
    for (x = 0; buf[x] == ' ' || buf[x] == '\t'; x++)
	;
    for (i = 0; buf[x] > ' '; x++, i++)
	dest[i] = buf[x];
    dest[i] = 0;
    return x;
}

BoolType seek_lesson(FILEsIO *fp, const char *lname)
{
	StrType	buffer;
	char	target[16];
	int	n;

	nestlevel = 0;

	sprintf(target, "%c%d*", toupper(lname[0]), atoi(&lname[1]));
	n = strlen(target);
	while (iogets(buffer, STR_SIZE, fp)) {
	    if (buffer[0] == '*' && strncmp(buffer+1, target, n) == 0)
		return TRUE;
	}
	return FALSE;
}

static int get_line(FILEsIO *fp, StrType buffer)
/* Read a line from the lesson file */
{
    enum{
	iff_none = 0, iff_if, iff_ifnot, iff_elif
    };
    int lng;
    int	i, ifflag, sw;
    char key[16], val[16];

    if (iogets(buffer, STR_SIZE, fp) == NULL) {
	buffer[0] = '\0';
	return L_EOF;
    }
    lng = strlen(buffer);
    if (lng >= 1 && buffer[lng - 1] == '\n')
	buffer[--lng] = '\0';
    if (lng == 0)
	return lng;
    if (buffer[0] == '*' && buffer[1] == '\0')
	return L_EOLESSON;
    if (buffer[0] != '#')
	return lng;

    if (strncmp(&buffer[1], "else", 4) == 0)
	return L_ELT;
    if (strncmp(&buffer[1], "endif", 5) == 0)
	return L_END;
    if (strncmp(&buffer[1], "param", 5) == 0) {
	i = copy_keystr(&buffer[6], key);
	i = copy_keystr(&buffer[6+i], val);
	add_dictionary(key, val);
	return L_PARAM;
    }
    ifflag = iff_none;
    i = 0;
    if (strncmp(&buffer[1], "if", 2) == 0) {
	if (strncmp(&buffer[3], "not", 3) == 0)
	    ifflag = iff_ifnot, i = 6;
	else
	    ifflag = iff_if, i = 3;
    }else if (strncmp(&buffer[1], "elif", 4) == 0)
	ifflag = iff_elif, i = 5;
    if (ifflag == iff_none)
	return lng;

    (void)copy_keystr(&buffer[i], key);
    sw = (strcmp(key, keyboard_type) == 0);
    switch (ifflag) {
    case iff_if: /* if */
	    return (sw ? L_IFT : L_IFF);
    case iff_ifnot: /* ifnot */
	    return (sw ? L_IFF : L_IFT);
    case iff_elif: /* elif */
	    return (sw ? L_ELT : L_ELF);
    }
    return lng; /* never */
}


static int skip_endif(FILEsIO *fp, StrType buffer, BoolType srch)
{
	int	lng, r;

	while ((lng = get_line(fp, buffer)) != L_EOF && lng != L_EOLESSON) {
		if (lng >= 0) continue;
		if (lng == L_END)
			return L_END;
		if (lng == L_IFT || lng == L_IFF) { /* nest */
			r = skip_endif(fp, buffer, FALSE);
			if (r == L_EOF || r == L_EOLESSON)
				return r;
			continue;
		}
		/* search next #elif / #else */
		if (srch && lng == L_ELT)
			return lng;
	}
	return lng; /* L_EOF or L_EOLESSON */
}

int get_lesson_line(FILEsIO *fp, StrType buffer)
{
    int	lng;
    int	r;

    while ((lng = get_line(fp, buffer)) < 0) {
	switch (lng) {
	case L_EOF:
	case L_EOLESSON:
		return lng;
	case L_IFT:
		++nestlevel;
		break;
	case L_IFF:
		++nestlevel;
		r = skip_endif(fp, buffer, TRUE);
		if (r == L_EOF || r == L_EOLESSON)
			return r;
		if (r == L_END)
			--nestlevel;
		/* else L_ELT */
		break;
	case L_ELT:
	case L_ELF: /* only after true branch */
		r = skip_endif(fp, buffer, FALSE);
		if (r == L_EOF || r == L_EOLESSON)
			return r;
		/* else L_END */
		if (nestlevel > 0) --nestlevel;
		break;
	case L_END:
		if (nestlevel > 0) --nestlevel;
		break;
	case L_PARAM:
	default: break;
	}
    }
    return lng;
}
