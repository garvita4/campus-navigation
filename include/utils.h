/* utils.h - colours, string helpers, user input */
#ifndef UTILS_H
#define UTILS_H

extern const char *cGreen, *cRed, *cYellow, *cCyan, *cBold, *cReset;
extern int colorsOn;

void setColors(int on);
int  ieq(const char *a, const char *b);                /* case-insensitive equal */
int  icontains(const char *hay, const char *needle);   /* case-insensitive search */
int  readInt(const char *prompt);                      /* returns -99 on EOF */
int  askBuilding(const char *prompt);                  /* number or (partial) name */

#endif
