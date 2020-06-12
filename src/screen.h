#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "fv_file.h"

void clear_screen();
void refresh_screen(int trows, int tcols, int line_count, int voffset, struct filerow **contents);

#endif /* _SCREEN_H_ */
