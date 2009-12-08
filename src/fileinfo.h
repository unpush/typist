#include <stdio.h>
#include "base.h"

BoolType get_index(void);
int check_lesson_name(const char *);
FILE *open_rc(void);
void read_rc(FILE *);
void write_rc(void);
void update_lesson(const char *);
