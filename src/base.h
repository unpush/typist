/* ------------------------------------------------------
	base.h
---------------------------------------------------------
	Ver.3.0   2007-09-06	by Takeshi Ogihara
------------------------------------------------------ */

#ifndef  __DEF_BASE__
#define  __DEF_BASE__

#define  JPN		1	/* Japanese MODE (don't remove) */
#if defined(MACKEYBOARD) && !defined(KANA_LESSON)
# define  KANA_LESSON	1
#endif
/* #define KANA_LESSON 1  ...  If you use Kana Lessons */
/* Without Japanese Input Program, JIS106 keyboard doesn't generate
   any code with Shift+'0' (especially Windows systems).
   This is for kana character 'WO'.
*/

#if 1 /* for Windows XP */
#define  KANA_CODE	1	/* ShiftJIS */
#if !defined(WinXP)
# define  WinXP		1
# define  USE_WINSOCK   1
#endif
#endif /* for Windows XP */

#define   STR_SIZE	255	/* was 1024 */
typedef char StrType[STR_SIZE + 1];
typedef unsigned char BoolType;

#if !defined(FALSE)
# define  FALSE		0
#endif
#if !defined(TRUE)
# define  TRUE		1
#endif

#if !defined(NULL)
# define  NULL		0
#endif

#endif /* __DEF_BASE__ */
