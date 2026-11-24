#ifndef DIJKSTRA_H
#define DIJKSTRA_H
/* Shortest distance from src to dst. Returns -1 if unreachable. path/len optional. */
int  dijkstra(int src, int dst, int path[], int *len);
void shortestRouteMenu(void);
#endif
