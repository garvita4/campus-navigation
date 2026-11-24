#ifndef UTILS_H
#define UTILS_H

extern const char *cGreen, *cRed, *cYellow, *cCyan, *cBold, *cReset;
extern int colorsOn;

void setColors(int on);
int  ieq(const char *a, const char *b);
int  icontains(const char *hay, const char *needle);
int  readInt(const char *prompt);
int  askBuilding(const char *prompt);

#endif
