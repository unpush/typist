#ifndef __DEF_R2K__
#define __DEF_R2K__

#include "kana.h"

#define YoonTabSize	8
extern unsigned char kanatab[][5][2];
extern unsigned char yoontab[YoonTabSize][2];

#define kBUFSIZE	4
#define rBUFSIZE	8
typedef struct {
	kanachar kbuf[kBUFSIZE];
	char	rbuf[rBUFSIZE];
	int	kidx;
	int	ridx;
	int	mode;
} r2k_info;

r2k_info *resetKanaInput(r2k_info *infp);
int roman2kana(int inchar, r2k_info *infp);
BoolType isYoon(kanachar a, kanachar *p);

#define convKana(p)	((p)->mode == 0)

#endif /* __DEF_R2K__ */
