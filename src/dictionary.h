/* ------------------------------------------------------
	dictionary -- typist
---------------------------------------------------------
	Ver.3.0   2007-06-05	by Takeshi Ogihara
------------------------------------------------------ */

#ifndef  __DEF_DIC__
#define  __DEF_DIC__

#include "base.h"

void clear_dictionary(void);
const char *get_value(const char *keystr);
BoolType is_true_value(const char *keystr);
void add_dictionary(const char *keystr, const char *valstr);

#endif
