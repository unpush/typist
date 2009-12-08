/*
*   Typist
*       Version 3.0  2007.08.02  Roman-Kana mechanism by Takeshi OGIHARA
*   Copyright (c) 2007 Takeshi Ogihara. All rights reserved.
*/
#include <ctype.h>
#include "r2k.h"
#include "kana.h"

#define isHyphen(x)	((x) == '-' || (x) == '^')
#define Quote	'\''

enum {
	/* simple mode */
	m_none = 0,
	m_k, m_s, m_t, m_n, m_h, m_m, m_y, m_r, m_w, m_v, m_x,
	m_g, m_z, m_d, m_b, m_p, m_c, m_f, m_j, m_q,
	/* complex mode */
	m_ky, m_xk, m_sh, m_sy, m_ts, m_xt, m_xts,
	m_th, m_ty, m_tw, m_tp, m_ny, m_hy, m_hw, m_fy,
	m_my, m_ry, m_wh, m_wy, m_vy, m_xy, m_xw, m_gh, m_gy, m_jy,
	m_dh, m_dw, m_dp, m_dy, m_by, m_py, m_ch,
	m_Err
};
#define isSimpleMode(x)	((x) < m_ky)

static r2k_info info;

r2k_info *resetKanaInput(r2k_info *infp)
{
	if (infp == NULL)
		infp = &info;
	infp->mode = m_none;
	infp->kidx = infp->ridx = 0;
	return infp;
}

static kanachar sym2kana(int ch)
{
    if (isdigit(ch))
	return kchars[s_digit + (ch - '0')];
    switch (ch) {
    case '\n':
    case '\r': return '\n';
    case '-':
    case '^': return kchars[s_bar];
    case ' ': return kchars[s_sp];
    case ',': return kchars[s_comma];
    case '.': return kchars[s_peri];
    case '[': return kchars[s_kgk1];
    case ']': return kchars[s_kgk2];
    case '!': return kchars[s_bang];
    case '?': return kchars[s_ques];
    case '(': return kchars[s_mrk1];
    case ')': return kchars[s_mrk2];
    default: break;
    }
    return (kanachar)0;
}

static int sub_symbol(int inchar, r2k_info *infp)
{
    kanachar k;

    switch (infp->mode) {
    case m_none:
	k = sym2kana(inchar); 
	if (k == 0) {
	    infp->rbuf[0] = inchar;
	    infp->ridx = 1;
	    infp->mode = m_Err;
	}else {
    	    infp->kbuf[0] = k;
    	    infp->kidx = 1;
	}
	break;
    case m_n:
	infp->mode = m_none;
	infp->kbuf[0] = kchars[s_nn];
	infp->kidx = 1;
	if (infp->ridx > 0)
	    infp->ridx--;
	if (inchar != Quote) {
	    k = sym2kana(inchar); 
	    if (k == 0) {
		infp->rbuf[infp->ridx++] = inchar;
		infp->mode = m_Err;
	    }else
		infp->kbuf[infp->kidx++] = k;
	}
	break;
    case m_t:
	infp->rbuf[infp->ridx++] = inchar;
	infp->mode = (inchar == Quote) ? m_tp : m_Err;
	break;
    case m_d:
	infp->rbuf[infp->ridx++] = inchar;
	infp->mode = (inchar == Quote) ? m_dp : m_Err;
	break;
    default: /* Error */
	infp->rbuf[infp->ridx++] = inchar;
	infp->mode = m_Err;
	break;
    }
    return (infp->mode == m_Err) ? 1 : 0;
}

static int sub_vowel(int idx, r2k_info *infp)
{
    unsigned char *bytes;

    if (infp->mode == m_Err)
	return 1;
    bytes = kanatab[infp->mode][idx];
    infp->kidx = 0;
    if (bytes[0] == 0) {
	infp->mode = m_Err;
	return 1;
    }
    infp->kbuf[infp->kidx++] = kchars[bytes[0]];
    if (bytes[1] != 0)
	infp->kbuf[infp->kidx++] = kchars[bytes[1]];
    infp->ridx = 0;
    infp->mode = m_none;
    return 0;
}

static int srch_rule(const unsigned char rule[][2], int m)
{
    int i, ent;

    for (i = 0; (ent = rule[i][0]) != 0; i++) {
	if (ent == m)
	    return rule[i][1];
	if (ent > m)
	    break;
    }
    return 0; /* Not Found */
}

static int sub_conso(int ch, r2k_info *infp)
{
    /* These entries should be in the increasing order */
    static const unsigned char hrule[][2] = {
	{ m_k, m_q }, { m_s, m_sh }, { m_t, m_th }, { m_w, m_wh },
	{ m_g, m_gh }, { m_d, m_dh }, { m_c, m_ch }, { 0, 0 }
    };
    static const unsigned char yrule[][2] = {
	{ m_k, m_ky }, { m_s, m_sy }, { m_t, m_ty }, { m_n, m_ny },
	{ m_h, m_hy }, { m_m, m_my }, { m_r, m_ry }, { m_w, m_wy },
	{ m_v, m_vy }, { m_x, m_xy }, { m_g, m_gy }, { m_z, m_jy },
	{ m_d, m_dy }, { m_b, m_by }, { m_p, m_py }, { m_c, m_ty },
	{ m_f, m_fy }, { m_j, m_jy }, { m_tp, m_th }, { m_hw, m_fy },
	{ m_dp, m_dh }, { 0, 0 }
    };
    static const unsigned char wrule[][2] = {
	{ m_k, m_q }, { m_t, m_tw }, { m_h, m_hw },
	{ m_x, m_xw }, { m_g, m_gh }, { m_d, m_dw }, { 0, 0 }
    };
    int newval = 0;

    switch (ch) {
    case 'K':
	if (infp->mode == m_x)
	    newval = m_xk;
	break;
    case 'S':
	if (infp->mode == m_t)
	    newval = m_ts;
	else if (infp->mode == m_xt)
	    newval = m_xts;
	break;
    case 'T':
	if (infp->mode == m_x)
	    newval = m_xt;
	break;
    case 'H':
	newval = srch_rule(hrule, infp->mode);
	break;
    case 'Y':
	newval = srch_rule(yrule, infp->mode);
	break;
    case 'W':
	newval = srch_rule(wrule, infp->mode);
	break;
    default:
	break;
    }
    return newval;
}

int roman2kana(int inchar, r2k_info *infp) /* 0: Normal return */
{
    int x, ch;
    int nxmode, rtn;
#define B_vowel   0x40
#define B_conso   0x00
#define B_BMask   0xc0
#define B_VMask   0x3f
    static unsigned char alptab[] = {
/*	a,           b,   c,   d,   e,           f,   g,   h	*/
	(B_vowel|0), m_b, m_c, m_d, (B_vowel|3), m_f, m_g, m_h,
/*	i,           j,   k,   l, m,   n,   o,           p	*/
	(B_vowel|1), m_j, m_k, 0, m_m, m_n, (B_vowel|4), m_p,
/*	q,   r,   s,   t,   u,           v,   w,   x,   y,   z */
	m_q, m_r, m_s, m_t, (B_vowel|2), m_v, m_w, m_x, m_y, m_z
    };

    if (!isalpha(inchar) || inchar == 'L' || inchar == 'l')
	return sub_symbol(inchar, infp);

    ch = toupper(inchar);
    x = ch - 'A';
    if ((alptab[x] & B_BMask) == B_vowel)
	return sub_vowel((alptab[x] & B_VMask), infp);

    if (ch == 'N') {
	if (infp->mode == m_n) {
	    infp->kbuf[0] = kchars[s_nn];
	    infp->kidx = 1;
	    infp->ridx = 0;
	    infp->mode = m_none;
	    return 0;
	}else if (infp->mode == m_none) {
	    infp->rbuf[0] = inchar;
	    infp->ridx = 1;
	    infp->mode = m_n;
	    return 0;
	}else {
	    infp->mode = m_Err;
	    return 1;
	}
    }

/*  for (x = 0; x < N_conso; x++)             */
/*	if ("KSTNHMYRWVXGZDBPCFJQ"[x] == ch)  */
/*		break;                        */
    nxmode = alptab[x];
    if (infp->mode == m_none) {
	infp->rbuf[0] = inchar;
	infp->ridx = 1;
	infp->mode = nxmode;
	return 0;
    }
/* xtu */
    if (isSimpleMode(infp->mode)
	&& infp->ridx == 1 && toupper(infp->rbuf[0]) == ch) {
	    infp->kbuf[0] = kchars[s_xtu];
	    infp->kidx = 1;
	    return 0;
    }
/* n */
    if (infp->mode == m_n) {
	if (nxmode == m_y) {
	    infp->rbuf[infp->ridx++] = inchar;
	    infp->mode = m_ny;
	}else {
	    infp->kbuf[0] = kchars[s_nn];
	    infp->kidx = 1;
	    infp->rbuf[0] = inchar;
	    infp->ridx = 1;
	    infp->mode = nxmode;
	}
	return 0;
    }

    rtn = sub_conso(ch, infp);
    if (rtn == 0)
	return 1; /*err*/
    infp->rbuf[infp->ridx++] = inchar;
    infp->mode = rtn;

    return 0;
}

BoolType isYoon(kanachar a, kanachar *p)
{
    static kanachar tab[YoonTabSize][2];
    static BoolType initflag = 0;
    int i, k;

    if (initflag == 0) {
	for (i = 0; i < YoonTabSize; i++) {
	    tab[i][0] = kchars[yoontab[i][0]];
	    tab[i][1] = kchars[yoontab[i][1]];
	}
	initflag = 1;
    }
    for (i = 0; i < YoonTabSize; i++) {
	for (k = 0; k < 2; k++) {
	    if (tab[i][k] == a) {
		if (p != NULL)
		    *p = tab[i][1-k];
		return TRUE;
	    }
	}
    }
    return FALSE;
}

