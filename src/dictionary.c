#include <string.h>
#include <stdlib.h>
#if !defined(__BORLANDC__)
# include <libc.h>
#endif
#include "base.h"

typedef struct _entry {
	char *key;
	char *val;
	/* struct _entry *next;
	If you want to use list. You can use hash table, of course */
} entry;

#define MAX_DIC		20

static entry thedic[MAX_DIC];
static int dicindex = 0;

void clear_dictionary(void)
{
	int i;
	for (i = 0; i < dicindex; i++) {
		if (thedic[i].key)
			free(thedic[i].key);
		if (thedic[i].val)
			free(thedic[i].val);
	}
	dicindex = 0;
}

static entry *get_entry(const char *keystr)
{
	int i;

	for (i = 0; i < dicindex; i++)
		if (strcmp(keystr, thedic[i].key) == 0)
			return &thedic[i];
	return NULL;
}

const char *get_value(const char *keystr)
{
	entry *ep = get_entry(keystr);
	return ep->val;
}

BoolType is_true_value(const char *keystr)
{
	entry *ep;
	int ch;

	if ((ep = get_entry(keystr)) == NULL)
		return FALSE;
	if (ep->val == NULL)
		return TRUE;
	ch = ep->val[0];
	if (ch == 'N' || ch == 'n' || ch == '0')
		return FALSE;
	return TRUE;
}

void add_dictionary(const char *keystr, const char *valstr)
{
	entry *ep = get_entry(keystr);
	if (ep) {
		if (ep->val)
			free(ep->val);
	}else {
		ep = &thedic[dicindex++];
		ep->key = strdup(keystr);
	}
	ep->val = (valstr && *valstr) ? strdup(valstr) : NULL;
}
