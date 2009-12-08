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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "typist.h"
#include "lineio.h"

static int	nestlevel = 0;


BoolType seek_lesson(fp, lname)
	FILE *fp;
	char *lname;
{
	StrType	buffer;
	char	target[16];
	int	n;

	nestlevel = 0;

	sprintf(target, "%c%d*", toupper(lname[0]), atoi(&lname[1]));
	n = strlen(target);
	while (fgets(buffer, STR_SIZE, fp)) {
	    if (buffer[0] == '*' && strncmp(buffer+1, target, n) == 0)
		return TRUE;
	}
	return FALSE;
}

static int get_line(fp, buffer)
/* Read a line from the lesson file */
	FILE *fp;
	StrType buffer;
{
    int lng;
    int	i, j, ifflag, sw;
    char key[16];

    if (feof(fp) || ferror(fp) || fgets(buffer, STR_SIZE, fp) == NULL) {
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
    ifflag = i = 0;
    if (strncmp(&buffer[1], "if", 2) == 0) {
	if (strncmp(&buffer[3], "not", 3) == 0)
	    ifflag = 2, i = 6;
	else
	    ifflag = 1, i = 3;
    }else if (strncmp(&buffer[1], "elif", 4) == 0)
	ifflag = 3, i = 5;
    if (ifflag == 0)
	return lng;

    for ( ; buffer[i] == ' ' || buffer[i] == '\t'; i++) ;
    for (j = 0; buffer[i] > ' '; )
	    key[j++] = buffer[i++];
    key[j] = 0;
    sw = (strcmp(key, Keytype) == 0);
    switch (ifflag) {
    case 1: /* if */
	    return (sw ? L_IFT : L_IFF);
    case 2: /* ifnot */
	    return (sw ? L_IFF : L_IFT);
    case 3: /* elif */
	    return (sw ? L_ELT : L_ELF);
    }
    return lng; /* never */
}


static int skip_endif(fp, buffer, srch)
	FILE	*fp;
	StrType	buffer;
	BoolType srch;
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

int get_lesson_line(fp, buffer)
    FILE	*fp;
    StrType	buffer;
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
		r = skip_endif(fp, buffer, FALSE);
		
	case L_END:
		if (nestlevel > 0) --nestlevel;
		break;
	default: break;
	}
    }
    return lng;
}
