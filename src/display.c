#include <stdio.h>
#include "campus.h"
#include "utils.h"
#include "display.h"

void banner(void) {
    printf("%s", cCyan);
    printf("  +--------------------------------------------+\n");
    printf("  |         CAMPUS  NAVIGATION  SYSTEM         |\n");
    printf("  |   Graph + Shortest Path  |  Dijkstra, BFS  |\n");
    printf("  +--------------------------------------------+\n");
    printf("%s", cReset);
}

void showBuildings(void) {
    printf("\n%s--- Buildings ---%s\n", cBold, cReset);
    for (int i = 0; i < n; i++) printf("  %2d. %s\n", i, names[i]);
}

void showWalkways(void) {
    printf("\n%s--- Walkways ---%s\n", cBold, cReset);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (w[i][j]) {
                printf("  %-14s <-> %-14s : %3d m", names[i], names[j], w[i][j]);
                if (stairs[i][j])  printf(" %s[stairs]%s", cYellow, cReset);
                if (blocked[i][j]) printf(" %s[BLOCKED]%s", cRed, cReset);
                printf("\n");
            }
}

void printChain(const int path[], int len) {
    for (int i = 0; i < len; i++)
        printf("%s%s", names[path[i]], i < len - 1 ? " -> " : "");
}

void printDirections(const int path[], int len) {
    printf("\n%sDirections:%s\n", cBold, cReset);
    printf("  Start at %s%s%s\n", cGreen, names[path[0]], cReset);
    for (int i = 0; i < len - 1; i++) {
        printf("  %d. Walk %d m to %s%s%s", i + 1, w[path[i]][path[i + 1]],
               cGreen, names[path[i + 1]], cReset);
        if (stairs[path[i]][path[i + 1]]) printf(" %s(take the stairs)%s", cYellow, cReset);
        printf("\n");
    }
    printf("  You have arrived at %s%s%s\n", cGreen, names[path[len - 1]], cReset);
}

static void boxLine(const char *text) {
    printf("  | %-32s |\n", text);
}

void printSummary(const int path[], int len, int total) {
    char buf[64];
    int stairCount = 0;
    for (int i = 0; i < len - 1; i++)
        if (stairs[path[i]][path[i + 1]]) stairCount++;

    printf("\n%s", cGreen);
    printf("  +----------------------------------+\n");
    boxLine("ROUTE SUMMARY");
    printf("  +----------------------------------+\n");
    snprintf(buf, sizeof buf, "Total distance : %d m", total);
    boxLine(buf);
    snprintf(buf, sizeof buf, "Walking time   : ~%d min", (total + WALK_SPEED - 1) / WALK_SPEED);
    boxLine(buf);
    snprintf(buf, sizeof buf, "Walkways used  : %d", len - 1);
    boxLine(buf);
    snprintf(buf, sizeof buf, "Stairs         : %s", stairCount ? "yes" : "none");
    boxLine(buf);
    printf("  +----------------------------------+\n");
    printf("%s", cReset);
}
