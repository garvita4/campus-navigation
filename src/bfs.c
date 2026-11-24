#include "campus.h"
#include "bfs.h"

int bfs(int src, int dst, int path[], int *len) {
    int parent[MAXN], vis[MAXN] = {0}, q[MAXN], f = 0, r = 0;
    for (int i = 0; i < n; i++) parent[i] = -1;
    vis[src] = 1;
    q[r++] = src;

    while (f < r) {
        int u = q[f++];
        for (int v = 0; v < n; v++)
            if (usable(u, v) && !vis[v]) {
                vis[v] = 1;
                parent[v] = u;
                q[r++] = v;
            }
    }
    if (!vis[dst]) return -1;

    int tmp[MAXN], c = 0, total = 0;
    for (int v = dst; v != -1; v = parent[v]) tmp[c++] = v;
    *len = c;
    for (int i = 0; i < c; i++) path[i] = tmp[c - 1 - i];
    for (int i = 0; i < c - 1; i++) total += w[path[i]][path[i + 1]];
    return total;
}
