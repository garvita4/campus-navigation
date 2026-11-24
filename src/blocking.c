#include <stdio.h>
#include "campus.h"
#include "utils.h"
#include "display.h"
#include "blocking.h"

void blockMenu(void) {
    showWalkways();
    int a = askBuilding("\nWalkway from");
    if (a < 0) return;
    int b = askBuilding("Walkway to");
    if (b < 0) return;
    if (!w[a][b]) {
        printf("%sNo direct walkway between those buildings.%s\n", cRed, cReset);
        return;
    }

    blocked[a][b] = blocked[b][a] = !blocked[a][b];
    printf("Walkway %s <-> %s is now %s%s%s.\n", names[a], names[b],
           blocked[a][b] ? cRed : cGreen, blocked[a][b] ? "BLOCKED" : "OPEN", cReset);
}
