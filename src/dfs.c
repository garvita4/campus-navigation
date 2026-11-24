/* dfs.c - explore the campus depth-first */
#include <stdio.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "dfs.h"

static void dfsVisit(int u, int vis[]) {
    vis[u] = 1;
    printf("%s", names[u]);
    for (int v = 0; v < n; v++)
        if (usable(u, v) && !vis[v]) {
            printf(" -> ");
            dfsVisit(v, vis);
        }
}

void dfsMenu(void) {
    showBuildings();
    int a = askBuilding("\nStart");
    if (a < 0) return;
    int vis[MAXN] = {0};
    printf("\n%sDFS exploration:%s\n  ", cBold, cReset);
    dfsVisit(a, vis);
    printf("\n");
}
