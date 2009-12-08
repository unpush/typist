/* ------------------------------------------------------
      sound
---------------------------------------------------------
	by Takeshi Ogihara
		(ogihara@rd.ecip.osaka-u.ac.jp)
	Ver.1.0   1992-07-11
	Ver.1.2   1994-11-10	for non-NeXT WS
	Ver.3.0   2007-05-30	by Takeshi Ogihara
------------------------------------------------------ */

#ifdef SILENT
void init_sndbeep(const char *dir) { }
void sndbeep(void) { }
#else

#include <stdio.h>

void init_sndbeep(const char *dir) { }

void sndbeep(void)
{
    putchar('\007');
    fflush(stdout);
}

#endif /* SILENT */
