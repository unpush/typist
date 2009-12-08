/* ------------------------------------------------------
	main.c	for typist
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for Japanese Users: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.2.0   1997-05-17	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>

#include "typist.h"

char kanamap[256][3];


#ifdef JPN

#define  KMAXLEN	128

BoolType init_kanamap()
{
	FILE	*fp;
	unsigned char	buf[KMAXLEN];
	int	i, cc;
	char	*p;
	static int	alreadyused = 0;
	StrType mapfile;

	if (alreadyused)
		return TRUE;
	for (i = 0; i < 256; i++) {
		p = &kanamap[i][0];
		p[0] = p[1] = ' ';
		p[2] = 0;
	}
	sprintf(mapfile, "%s%c%s.map", LessonDir, DELIM, Keytype);
	if ((fp = fopen(mapfile, "ra")) == NULL)
		return FALSE;
	while (fgets((char *)buf, KMAXLEN, fp) != NULL) {
		if ((cc = buf[0]) <= ' ') continue;
		p = &kanamap[cc][0];
		for (i = 1; buf[i] == ' ' || buf[i] == '\t'; i++)
			;
		p[0] = buf[i++];
		p[1] = buf[i];
		p[2] = 0;
	}
	alreadyused = 1;
	fclose(fp);
	return TRUE;
}

#endif /* JPN */
