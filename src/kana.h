#ifndef __DEF_KANA__
#define __DEF_KANA__

#include "base.h"

typedef unsigned int kanachar;
  /* typedef unsigned long kanachar; If int has only 16 bits */
  /* typedef unichar kanachar;       You may use on Mac */

#define UTF_8		0
#define ShiftJIS	1
#define EUC_JP		2

enum {
	s_sp = 1, s_comma, s_peri, s_bar,
	s_kgk1, s_kgk2, s_mrk1, s_mrk2, s_ques, s_bang,
	s_digit = 11, s_xa = 21,
	s_xtu = 55, s_ro = 97, s_wo = 102, s_nn = 103
};
/*    0: zero
  1- 10: SP ÅA ÅB Å[ Åu Åv Åi Åj ÅH ÅI
 11- 20: ÇO ÇP ÇQ ÇR ÇS ÇT ÇU ÇV ÇW ÇX
 21- 30: Çü Ç† Ç° Ç¢ Ç£ Ç§ Ç• Ç¶ Çß Ç®
 31- 40: Ç© Ç™ Ç´ Ç¨ Ç≠ ÇÆ ÇØ Ç∞ Ç± Ç≤
 41- 50: Ç≥ Ç¥ Çµ Ç∂ Ç∑ Ç∏ Çπ Ç∫ Çª Çº
 51- 60: ÇΩ Çæ Çø Ç¿ Ç¡ Ç¬ Ç√ Çƒ Ç≈ Ç∆
 61- 70: Ç« Ç» Ç… Ç  ÇÀ ÇÃ ÇÕ ÇŒ Çœ Ç–
 71- 80: Ç— Ç“ Ç” Ç‘ Ç’ Ç÷ Ç◊ Çÿ ÇŸ Ç⁄
 81- 90: Ç€ Ç‹ Ç› Çﬁ Çﬂ Ç‡ Ç· Ç‚ Ç„ Ç‰
 91-100: ÇÂ ÇÊ ÇÁ ÇË ÇÈ ÇÍ ÇÎ ÇÏ ÇÌ ÇÓ
101-110: ÇÔ Ç ÇÒ Éî Éï Éñ É@ ÉB ÉD ÉF
111-114: ÉH ÉÉ ÉÖ Éá
*/

extern kanachar kchars[];

#if defined(KANA_LESSON)
extern kanachar	kanamap[256];
BoolType init_kanamap(const char *, const char *);
#endif

#endif /* __DEF_KANA__ */
