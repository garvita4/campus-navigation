/* campus.h - shared graph data (buildings = vertices, walkways = edges) */
#ifndef CAMPUS_H
#define CAMPUS_H

#include <limits.h>

#define MAXN 20          /* max buildings */
#define MAXE 60          /* max walkways  */
#define INF INT_MAX
#define WALK_SPEED 80    /* metres per minute */

extern char names[MAXN][32];
extern int  n;                       /* number of buildings */
extern int  m;                       /* number of walkways  */
extern int  posx[MAXN], posy[MAXN];  /* map coordinates (for the web UI) */
extern int  w[MAXN][MAXN];           /* distance in metres, 0 = no walkway */
extern int  stairs[MAXN][MAXN];      /* 1 = walkway has stairs */
extern int  blocked[MAXN][MAXN];     /* 1 = walkway closed */
extern int  eid[MAXN][MAXN];         /* walkway id + 1 (0 = none) */
extern int  edgeA[MAXE], edgeB[MAXE];
extern int  accessibleMode;          /* 1 = avoid stairs */

int  loadCampus(const char *file);
int  findExact(const char *name);
int  usable(int u, int v);           /* can this walkway be used right now? */
void clearBlocked(void);
void blockEdge(int id);

#endif
