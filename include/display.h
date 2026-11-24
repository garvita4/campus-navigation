/* display.h - printing helpers for the terminal interface */
#ifndef DISPLAY_H
#define DISPLAY_H

void banner(void);
void showBuildings(void);
void showWalkways(void);
void printChain(const int path[], int len);
void printDirections(const int path[], int len);
void printSummary(const int path[], int len, int total);

#endif
