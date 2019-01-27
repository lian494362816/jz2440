#ifndef __FONT_H__
#define __FONT_H__

int font_init(void);
int font_put_char(int x, int y, char c, int color);
int font_put_string(int x, int y, char *string, int color);


#endif
