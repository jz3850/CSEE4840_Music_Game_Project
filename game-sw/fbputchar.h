#ifndef _FBPUTCHAR_H
#define _FBPUTCHAR_H

void fbputchar(int x, int y, char c, int color_index);
void fbputs(int x, int y, const char *s, int color_index);

#endif
