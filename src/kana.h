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
  1- 10: SP 、 。 ー 「 」 （ ） ？ ！
 11- 20: ０ １ ２ ３ ４ ５ ６ ７ ８ ９
 21- 30: ぁ あ ぃ い ぅ う ぇ え ぉ お
 31- 40: か が き ぎ く ぐ け げ こ ご
 41- 50: さ ざ し じ す ず せ ぜ そ ぞ
 51- 60: た だ ち ぢ っ つ づ て で と
 61- 70: ど な に ぬ ね の は ば ぱ ひ
 71- 80: び ぴ ふ ぶ ぷ へ べ ぺ ほ ぼ
 81- 90: ぽ ま み む め も ゃ や ゅ ゆ
 91-100: ょ よ ら り る れ ろ ゎ わ ゐ
101-110: ゑ を ん ヴ ヵ ヶ ァ ィ ゥ ェ
111-114: ォ ャ ュ ョ
*/

extern kanachar kchars[];

#if defined(KANA_LESSON)
extern kanachar	kanamap[256];
BoolType init_kanamap(const char *, const char *);
#endif

#endif /* __DEF_KANA__ */
