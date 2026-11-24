#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "dijkstra.h"
#include "compare.h"
#include "planner.h"
#include "dfs.h"
#include "floyd.h"
#include "blocking.h"
#include "export_dot.h"
#include "server.h"

static void printMenu(void) {
    printf("\n%s[Accessibility: %s | Colours: %s]%s\n"
           "  1. Show buildings\n"
           "  2. Show walkways\n"
           "  3. Shortest route (Dijkstra)\n"
           "  4. Compare Dijkstra vs BFS\n"
           "  5. Multi-stop trip planner\n"
           "  6. Explore campus (DFS)\n"
           "  7. Distance table (Floyd-Warshall)\n"
           "  8. Block / unblock a walkway\n"
           "  9. Toggle accessibility mode (avoid stairs)\n"
           " 10. Export map to Graphviz\n"
           " 11. Toggle colours\n"
           "  0. Exit\n",
           cCyan, accessibleMode ? "ON" : "OFF", colorsOn ? "ON" : "OFF", cReset);
}

int main(int argc, char *argv[]) {
    const char *file = "data/campus.txt";
    int serve = 0, port = 8080;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--serve") == 0) {
            serve = 1;
            if (i + 1 < argc && isdigit((unsigned char)argv[i + 1][0])) port = atoi(argv[++i]);
        } else {
            file = argv[i];
        }
    }

    setColors(1);
    if (!loadCampus(file)) {
        printf("Could not load campus map from '%s'.\n"
               "Run the program from the project folder (where data/ lives).\n", file);
        return 1;
    }

    if (serve) return startServer(port);

    banner();
    printf("  Loaded %d buildings and %d walkways from %s\n", n, m, file);

    int choice;
    do {
        printMenu();
        choice = readInt("Enter choice: ");
        switch (choice) {
            case 1:  showBuildings(); break;
            case 2:  showWalkways(); break;
            case 3:  shortestRouteMenu(); break;
            case 4:  compareMenu(); break;
            case 5:  plannerMenu(); break;
            case 6:  dfsMenu(); break;
            case 7:  floydMenu(); break;
            case 8:  blockMenu(); break;
            case 9:
                accessibleMode = !accessibleMode;
                printf("Accessibility mode %s.\n", accessibleMode ? "ON" : "OFF");
                break;
            case 10: exportDot(); break;
            case 11:
                setColors(!colorsOn);
                printf("Colours %s.\n", colorsOn ? "ON" : "OFF");
                break;
            case 0: case -99: printf("Goodbye!\n"); choice = 0; break;
            default: printf("%sInvalid choice.%s\n", cRed, cReset);
        }
    } while (choice != 0);

    return 0;
}
