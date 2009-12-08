/* ------------------------------------------------------
	typist
---------------------------------------------------------
	Original: D. Jason Penney (penneyj@slc.com)
	Tuned for ECIP NeXT: Takeshi Ogihara
	Ver.1.0   1992-07-14
	Ver.2.0   1997-04-21	Kana Exercise
	by Takeshi Ogihara  (ogihara@seg.kobe-u.ac.jp)
	Ver.3.0   2007-08-24	by Takeshi Ogihara
------------------------------------------------------ */

#ifndef  __DEF_LINEIO__
#define  __DEF_LINEIO__

#include "base.h"
#include "http.h"

BoolType seek_lesson(FILEsIO *fp, const StrType lname);
int get_lesson_line(FILEsIO *, StrType);

#endif  /* __DEF_LINEIO__ */
