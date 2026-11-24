#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "campus.h"
#include "utils.h"

char names[MAXN][32];
int  n = 0, m = 0;
int  posx[MAXN], posy[MAXN];
int  w[MAXN][MAXN], stairs[MAXN][MAXN], blocked[MAXN][MAXN], eid[MAXN][MAXN];
int  edgeA[MAXE], edgeB[MAXE];
int  accessibleMode = 0;

int findExact(const char *name) {
    for (int i = 0; i < n; i++)
        if (ieq(names[i], name)) return i;
    return -1;
}

int usable(int u, int v) {
    return w[u][v] && !blocked[u][v] && !(accessibleMode && stairs[u][v]);
}

void clearBlocked(void) {
    memset(blocked, 0, sizeof blocked);
}

void blockEdge(int id) {
    if (id < 0 || id >= m) return;
    blocked[edgeA[id]][edgeB[id]] = blocked[edgeB[id]][edgeA[id]] = 1;
}

int loadCampus(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) return 0;

    char line[160];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '#' || line[0] == 0) continue;

        char *tok = strtok(line, "|");
        if (!tok) continue;

        if (strcmp(tok, "N") == 0) {
            char *nm = strtok(NULL, "|");
            char *xs = strtok(NULL, "|");
            char *ys = strtok(NULL, "|");
            if (nm && n < MAXN) {
                strncpy(names[n], nm, sizeof names[n] - 1);
                posx[n] = xs ? atoi(xs) : 0;
                posy[n] = ys ? atoi(ys) : 0;
                n++;
            }
        } else if (strcmp(tok, "E") == 0) {
            char *a = strtok(NULL, "|");
            char *b = strtok(NULL, "|");
            char *d = strtok(NULL, "|");
            char *s = strtok(NULL, "|");
            int u = a ? findExact(a) : -1;
            int v = b ? findExact(b) : -1;
            if (u < 0 || v < 0 || !d || m >= MAXE) {
                printf("Warning: skipped invalid walkway line.\n");
                continue;
            }
            w[u][v] = w[v][u] = atoi(d);
            stairs[u][v] = stairs[v][u] = s ? atoi(s) : 0;
            edgeA[m] = u;
            edgeB[m] = v;
            eid[u][v] = eid[v][u] = m + 1;
            m++;
        }
    }
    fclose(f);
    return n > 0;
}
