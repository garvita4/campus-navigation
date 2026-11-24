#ifndef CAMPUS_H
#define CAMPUS_H

#include <limits.h>

#define MAXN 20
#define MAXE 60
#define INF INT_MAX
#define WALK_SPEED 80

extern char names[MAXN][32];
extern int  n;
extern int  m;
extern int  posx[MAXN], posy[MAXN];
extern int  w[MAXN][MAXN];
extern int  stairs[MAXN][MAXN];
extern int  blocked[MAXN][MAXN];
extern int  eid[MAXN][MAXN];
extern int  edgeA[MAXE], edgeB[MAXE];
extern int  accessibleMode;

int  loadCampus(const char *file);
int  findExact(const char *name);
int  usable(int u, int v);
void clearBlocked(void);
void blockEdge(int id);

#endif
