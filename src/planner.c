#include <stdio.h>
#include <string.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "dijkstra.h"
#include "planner.h"

#define MAX_STOPS 5

static int D[MAX_STOPS + 2][MAX_STOPS + 2];
static int ord[MAX_STOPS + 2], best[MAX_STOPS + 2];
static int bestCost, K;

static void permute(int pos, int cost, int usedMask) {
    if (pos == K + 1) {
        int last = D[ord[K]][K + 1];
        if (last < 0) return;
        if (cost + last < bestCost) {
            bestCost = cost + last;
            memcpy(best, ord, sizeof ord);
        }
        return;
    }
    for (int s = 1; s <= K; s++) {
        if (usedMask & (1 << s)) continue;
        int d = D[ord[pos - 1]][s];
        if (d < 0) continue;
        ord[pos] = s;
        permute(pos + 1, cost + d, usedMask | (1 << s));
    }
}

void plannerMenu(void) {
    showBuildings();
    int pts[MAX_STOPS + 2];
    pts[0] = askBuilding("\nStart");
    if (pts[0] < 0) return;
    int end = askBuilding("Final destination");
    if (end < 0) return;

    K = readInt("Number of stops in between (0-5): ");
    if (K < 0 || K > MAX_STOPS) { printf("Please enter 0 to %d.\n", MAX_STOPS); return; }

    for (int i = 1; i <= K; i++) {
        char p[24];
        snprintf(p, sizeof p, "Stop %d", i);
        pts[i] = askBuilding(p);
        if (pts[i] < 0) return;
    }
    pts[K + 1] = end;

    for (int a = 0; a < K + 2; a++)
        for (int b = 0; b < K + 2; b++)
            D[a][b] = (a == b) ? 0 : dijkstra(pts[a], pts[b], NULL, NULL);

    bestCost = INF;
    ord[0] = 0;
    permute(1, 0, 0);

    if (bestCost == INF) { printf("\n%sNo complete route exists.%s\n", cRed, cReset); return; }

    printf("\n%sBest trip plan:%s\n", cBold, cReset);
    int seq[MAX_STOPS + 2];
    for (int i = 0; i <= K; i++) seq[i] = best[i];
    seq[K + 1] = K + 1;

    for (int i = 0; i < K + 1; i++) {
        int path[MAXN], len;
        dijkstra(pts[seq[i]], pts[seq[i + 1]], path, &len);
        printf("  Leg %d (%d m): ", i + 1, D[seq[i]][seq[i + 1]]);
        printChain(path, len);
        printf("\n");
    }
    printf("\n%sTotal distance: %d m | Walking time: ~%d min%s\n", cGreen, bestCost,
           (bestCost + WALK_SPEED - 1) / WALK_SPEED, cReset);
}
