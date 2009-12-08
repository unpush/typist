#include "r2k.h"
#include "kana.h"

enum {
	/* SP ÅA ÅB Å[ Åu Åv Åi Åj ÅH ÅI */
	/* ÇO ÇP ÇQ ÇR ÇS ÇT ÇU ÇV ÇW ÇX */
	/* Çü Ç† Ç° Ç¢ Ç£ Ç§ Ç• Ç¶ Çß Ç® */
	xa=s_xa, aa, xi, ii, xu, uu, xe, ee, xo, oo,
	/* Ç© Ç™ Ç´ Ç¨ Ç≠ ÇÆ ÇØ Ç∞ Ç± Ç≤ */
	ka, ga, ki, gi, ku, gu, ke, ge, ko, go, 
	/* Ç≥ Ç¥ Çµ Ç∂ Ç∑ Ç∏ Çπ Ç∫ Çª Çº */
	sa, za, si, zi, su, zu, se, ze, so, zo, 
	/* ÇΩ Çæ Çø Ç¿ Ç¡ Ç¬ Ç√ Çƒ Ç≈ Ç∆ Ç« */
	ta, da, ti, di, xtu, tu, du, te, de, to, d0,
	/* Ç» Ç… Ç  ÇÀ ÇÃ */
	na, ni, nu, ne, no,
	/* ÇÕ ÇŒ Çœ Ç– Ç— Ç“ Ç” Ç‘ Ç’ Ç÷ Ç◊ Çÿ ÇŸ Ç⁄ Ç€ */
	ha, ba, pa, hi, bi, pi, hu, bu, pu, he, be, pe, ho, bo, po,
	/* Ç‹ Ç› Çﬁ Çﬂ Ç‡ Ç· Ç‚ Ç„ Ç‰ ÇÂ ÇÊ */
	ma, mi, mu, me, mo, xya, ya, xyu, yu, xyo, yo,
	/* ÇÁ ÇË ÇÈ ÇÍ ÇÎ ÇÏ ÇÌ ÇÓ ÇÔ Ç ÇÒ */
	ra, ri, ru, re, ro, xwa, wa, wyi, wye, wo, nn,
	/* Éî Éï Éñ É@ ÉB ÉD ÉF ÉH ÉÉ ÉÖ Éá */
	vu, xka, xke, xax, xix, xux, xex, xox, xyax, xyux, xyox
};

unsigned char kanatab[][5][2] = {
	{ { aa, 0 }, { ii, 0 }, { uu, 0 }, { ee, 0 }, { oo, 0 } },
	{ { ka, 0 }, { ki, 0 }, { ku, 0 }, { ke, 0 }, { ko, 0 } },
	{ { sa, 0 }, { si, 0 }, { su, 0 }, { se, 0 }, { so, 0 } },
	{ { ta, 0 }, { ti, 0 }, { tu, 0 }, { te, 0 }, { to, 0 } },
	{ { na, 0 }, { ni, 0 }, { nu, 0 }, { ne, 0 }, { no, 0 } },
	{ { ha, 0 }, { hi, 0 }, { hu, 0 }, { he, 0 }, { ho, 0 } },
	{ { ma, 0 }, { mi, 0 }, { mu, 0 }, { me, 0 }, { mo, 0 } },
	{ { ya, 0 }, { ii, 0 }, { yu, 0 }, { ii, xe }, { yo, 0 } },
	{ { ra, 0 }, { ri, 0 }, { ru, 0 }, { re, 0 }, { ro, 0 } },
	{ { wa, 0 }, { uu, xi }, { uu, 0 }, { uu, xe }, { wo, 0 } },
	{ { vu, xax }, { vu, xix }, { vu, 0 }, { vu, xex }, { vu, xox } },
	{ { xa, 0 }, { xi, 0 }, { xu, 0 }, { xe, 0 }, { xo, 0 } },
	{ { ga, 0 }, { gi, 0 }, { gu, 0 }, { ge, 0 }, { go, 0 } },
	{ { za, 0 }, { zi, 0 }, { zu, 0 }, { ze, 0 }, { zo, 0 } },
	{ { da, 0 }, { di, 0 }, { du, 0 }, { de, 0 }, { d0, 0 } },
	{ { ba, 0 }, { bi, 0 }, { bu, 0 }, { be, 0 }, { bo, 0 } },
	{ { pa, 0 }, { pi, 0 }, { pu, 0 }, { pe, 0 }, { po, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { hu, xa }, { hu, xi }, { hu, 0 }, { hu, xe }, { hu, xo } },
	{ { zi, xya }, { zi, 0 }, { zi, xyu }, { zi, xe }, { zi, xyo } },
	{ { ku, xa }, { ku, xi }, { ku, xu }, { ku, xe }, { ku, xo } },
	{ { ki, xya }, { 0, 0 }, { ki, xyu }, { 0, 0 }, { ki, xyo } },
	{ { xka, 0 }, { 0, 0 }, { 0, 0 }, { xke, 0 }, { 0, 0 } },
	{ { si, xya }, { si, 0 }, { si, xyu }, { si, xe }, { si, xyo } },
	{ { si, xya }, { 0, 0 }, { si, xyu }, { si, xe }, { si, xyo } },
	{ { tu, xa }, { tu, xi }, { tu, 0 }, { tu, xe }, { tu, xo } },
	{ { 0, 0 }, { 0, 0 }, { xtu, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { xtu, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { te, xi }, { te, xyu }, { 0, 0 }, { 0, 0 } },
	{ { ti, xya }, { 0, 0 }, { ti, xyu }, { ti, xe }, { ti, xyo } },
	{ { 0, 0 }, { 0, 0 }, { to, xu }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { te, xi }, { to, xu }, { 0, 0 }, { 0, 0 } },
	{ { ni, xya }, { 0, 0 }, { ni, xyu }, { ni, xe }, { ni, xyo } },
	{ { hi, xya }, { 0, 0 }, { hi, xyu }, { hi, xe }, { hi, xyo } },
	{ { hu, xa }, { hu, xi }, { hu, xyu }, { hu, xe }, { hu, xo } },
	{ { 0, 0 }, { hu, xi }, { hu, xyu }, { hu, xe }, { 0, 0 } },
	{ { mi, xya }, { 0, 0 }, { mi, xyu }, { 0, 0 }, { mi, xyo } },
	{ { ri, xya }, { 0, 0 }, { ri, xyu }, { 0, 0 }, { ri, xyo } },
	{ { 0, 0 }, { uu, xi }, { 0, 0 }, { uu, xe }, { uu, xo } },
	{ { 0, 0 }, { wyi, 0 }, { 0, 0 }, { wye, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { vu, xyux }, { 0, 0 }, { 0, 0 } },
	{ { xya, 0 }, { 0, 0 }, { xyu, 0 }, { 0, 0 }, { xyo, 0 } },
	{ { xwa, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { gu, xa }, { gu, xi }, { gu, xu }, { gu, xe }, { gu, xo } },
	{ { gi, xya }, { 0, 0 }, { gi, xyu }, { 0, 0 }, { gi, xyo } },
	{ { zi, xya }, { 0, 0 }, { zi, xyu }, { zi, xe }, { zi, xyo } },
	{ { 0, 0 }, { de, xi }, { de, xyu }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { d0, xu }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { de, xi }, { d0, xu }, { 0, 0 }, { 0, 0 } },
	{ { di, xya }, { 0, 0 }, { di, xyu }, { di, xe }, { di, xyo } },
	{ { bi, xya }, { 0, 0 }, { bi, xyu }, { bi, xe }, { bi, xyo } },
	{ { pi, xya }, { 0, 0 }, { pi, xyu }, { pi, xe }, { pi, xyo } },
	{ { ti, xya }, { ti, 0 }, { ti, xyu }, { ti, xe }, { ti, xyo } }
};

unsigned char yoontab[YoonTabSize][2] = {
	{ xya, xyax },
	{ xyu, xyux },
	{ xyo, xyox },
	{ xa,  xax },
	{ xi,  xix },
	{ xu,  xux },
	{ xe,  xex },
	{ xo,  xox }
};

#ifdef _ALONE_
#include <stdio.h>
int main(void)
{
    int i, j, x, k;

    for (i = 0; i < 53; i++) {
        for (j = 0; j < 5; j++) {
	    unsigned char *pp = kanatab[i][j];
	    putchar('\t');
	    if (pp[0] == 0)
		putchar('@');
	    else {
		for (x = 0; x < 2; x++) {
		    if ((k = kchars[pp[x]]) == 0)
			    break;
#if (KANA_CODE == UTF_8)
		    putchar((k >> 16) & 0xff);
#endif
		    putchar((k >> 8) & 0xff);
		    putchar(k & 0xff);
		}
	    }
        }
	putchar('\n');
    }
    return 0;
}
#endif
