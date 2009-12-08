/* ------------------------------------------------------
      sound
---------------------------------------------------------
	by Takeshi Ogihara
		(ogihara@rd.ecip.osaka-u.ac.jp)
	Ver.1.0   1992-07-11
	Ver.1.2   1994-11-10	for non-NeXT WS
------------------------------------------------------ */

#ifdef SILENT
void sndbeep_init(dir)
    char *dir;
{ }
void sndbeep() { }
#else

#include <stdio.h>

#ifdef NeXT

#include <libc.h>
#include <sys/file.h>
#include <sound/sound.h>
#include "typist.h"

static SNDSoundStruct *rawsnd;
static int sw;

void sndbeep_init(dir) /* NeXT */
    char *dir;
{
    StrType fullName;

    sprintf(fullName, "%s/%s", dir, CLICKSND);
    sw = (access(fullName, R_OK) == 0);
    if (sw)
	SNDReadSoundfile(fullName, &rawsnd);
}

void sndbeep() /* NeXT */
{
    if (sw) {
	SNDStartPlaying(rawsnd,2,5,1, 0, 0);
	SNDWait(0);
    }else {
	putchar('\007');
	fflush(stdout);
    }
}

#else

void sndbeep_init()
{
}

void sndbeep()
{
    putchar('\007');
    fflush(stdout);
}
#endif /* NeXT */

#endif /* SILENT */

#ifdef TESTMODE
main()
{
    sndbeep_init("../lib");
    sndbeep();
    sleep(2);
    sndbeep();
}
#endif
