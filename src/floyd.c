/* floyd.c - all-pairs shortest distances (Floyd-Warshall) */
#include <stdio.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "floyd.h"

void floydMenu(void) {
    int d[MAXN][MAXN];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            d[i][j] = (i == j) ? 0 : (usable(i, j) ? w[i][j] : INF);

    /* try every building k as an intermediate stop */
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (d[i][k] != INF && d[k][j] != INF && d[i][k] + d[k][j] < d[i][j])
                    d[i][j] = d[i][k] + d[k][j];

    showBuildings();
    printf("\n%sAll-pairs shortest distances in metres (Floyd-Warshall)%s\n", cBold, cReset);
    printf("     ");
    for (int j = 0; j < n; j++) printf("%s%5d%s", cCyan, j, cReset);
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("  %s%2d%s ", cCyan, i, cReset);
        for (int j = 0; j < n; j++) {
            if (d[i][j] == INF) printf("%s%5s%s", cRed, "-", cReset);
            else                printf("%5d", d[i][j]);
        }
        printf("\n");
    }
    printf("(\"-\" means no route)\n");
}
