/* dijkstra.c - shortest route by distance (the main requirement) */
#include <stdio.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "dijkstra.h"

int dijkstra(int src, int dst, int path[], int *len) {
    int dist[MAXN], parent[MAXN], vis[MAXN];
    for (int i = 0; i < n; i++) { dist[i] = INF; parent[i] = -1; vis[i] = 0; }
    dist[src] = 0;

    for (int c = 0; c < n; c++) {
        /* pick the unvisited building with the smallest distance */
        int u = -1, min = INF;
        for (int i = 0; i < n; i++)
            if (!vis[i] && dist[i] < min) { min = dist[i]; u = i; }
        if (u == -1) break;
        vis[u] = 1;

        /* relax all usable walkways from u */
        for (int v = 0; v < n; v++)
            if (usable(u, v) && !vis[v] && dist[u] + w[u][v] < dist[v]) {
                dist[v] = dist[u] + w[u][v];
                parent[v] = u;
            }
    }
    if (dist[dst] == INF) return -1;

    if (path && len) {                       /* rebuild path from parent[] */
        int tmp[MAXN], c = 0;
        for (int v = dst; v != -1; v = parent[v]) tmp[c++] = v;
        *len = c;
        for (int i = 0; i < c; i++) path[i] = tmp[c - 1 - i];
    }
    return dist[dst];
}

void shortestRouteMenu(void) {
    showBuildings();
    int a = askBuilding("\nStart");
    if (a < 0) return;
    int b = askBuilding("Destination");
    if (b < 0) return;

    int path[MAXN], len;
    int d = dijkstra(a, b, path, &len);
    if (d < 0) {
        printf("\n%sNo route available from %s to %s%s.%s\n", cRed, names[a], names[b],
               accessibleMode ? " (accessibility mode ON)" : "", cReset);
        return;
    }
    printf("\n%sShortest route%s:%s\n  ", cBold, accessibleMode ? " (step-free)" : "", cReset);
    printChain(path, len);
    printf("\n");
    printDirections(path, len);
    printSummary(path, len, d);
}
