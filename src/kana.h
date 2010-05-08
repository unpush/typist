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
  1- 10: SP �A �B �[ �u �v �i �j �H �I
 11- 20: �O �P �Q �R �S �T �U �V �W �X
 21- 30: �� �� �� �� �� �� �� �� �� ��
 31- 40: �� �� �� �� �� �� �� �� �� ��
 41- 50: �� �� �� �� �� �� �� �� �� ��
 51- 60: �� �� �� �� �� �� �� �� �� ��
 61- 70: �� �� �� �� �� �� �� �� �� ��
 71- 80: �� �� �� �� �� �� �� �� �� ��
 81- 90: �� �� �� �� �� �� �� �� �� ��
 91-100: �� �� �� �� �� �� �� �� �� ��
101-110: �� �� �� �� �� �� �@ �B �D �F
111-114: �H �� �� ��
*/

extern kanachar kchars[];

#if defined(KANA_LESSON)
extern kanachar	kanamap[256];
BoolType init_kanamap(const char *, const char *);
#endif

#endif /* __DEF_KANA__ */
