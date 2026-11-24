/* compare.c - Dijkstra vs BFS side by side */
#include <stdio.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "dijkstra.h"
#include "bfs.h"
#include "compare.h"

void compareMenu(void) {
    showBuildings();
    int a = askBuilding("\nStart");
    if (a < 0) return;
    int b = askBuilding("Destination");
    if (b < 0) return;

    int dp[MAXN], dl, bp[MAXN], bl;
    int dd = dijkstra(a, b, dp, &dl);
    int bd = bfs(a, b, bp, &bl);
    if (dd < 0 || bd < 0) { printf("\n%sNo route found.%s\n", cRed, cReset); return; }

    printf("\n%sDijkstra%s (minimises DISTANCE)\n  ", cBold, cReset);
    printChain(dp, dl);
    printf("\n  Distance: %d m | Walkways: %d\n", dd, dl - 1);

    printf("\n%sBFS%s (minimises NUMBER OF WALKWAYS)\n  ", cBold, cReset);
    printChain(bp, bl);
    printf("\n  Distance: %d m | Walkways: %d\n", bd, bl - 1);

    int same = (dl == bl);
    for (int i = 0; same && i < dl; i++)
        if (dp[i] != bp[i]) same = 0;

    printf("\n%s", cYellow);
    if (same)
        printf("Both algorithms found the same route.\n");
    else if (dd == bd)
        printf("Different routes, but both are equally long.\n");
    else
        printf("Different routes! BFS uses fewer walkways (%d vs %d) but is %d m longer.\n"
               "BFS ignores walkway lengths, so weighted graphs need Dijkstra.\n",
               bl - 1, dl - 1, bd - dd);
    printf("%s", cReset);
}
