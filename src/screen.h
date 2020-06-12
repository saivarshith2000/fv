#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "file.h"

void clear_screen();
void refresh_screen(int trows, int tcols, int line_count, struct filerow *contents);

#endif
