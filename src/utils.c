/* utils.c - colours, case-insensitive helpers and user input */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "campus.h"
#include "utils.h"

const char *cGreen = "", *cRed = "", *cYellow = "", *cCyan = "", *cBold = "", *cReset = "";
int colorsOn = 1;

void setColors(int on) {
    colorsOn = on;
    cGreen  = on ? "\033[32m" : "";
    cRed    = on ? "\033[31m" : "";
    cYellow = on ? "\033[33m" : "";
    cCyan   = on ? "\033[36m" : "";
    cBold   = on ? "\033[1m"  : "";
    cReset  = on ? "\033[0m"  : "";
}

int ieq(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

int icontains(const char *hay, const char *needle) {
    size_t ln = strlen(needle);
    if (ln == 0) return 0;
    for (; *hay; hay++) {
        size_t i = 0;
        while (i < ln && hay[i] &&
               tolower((unsigned char)hay[i]) == tolower((unsigned char)needle[i]))
            i++;
        if (i == ln) return 1;
    }
    return 0;
}

int readInt(const char *prompt) {
    char b[32];
    printf("%s", prompt);
    if (!fgets(b, sizeof b, stdin)) return -99;      /* EOF */
    return atoi(b);
}

/* Accepts a building number OR a (partial) name. Returns index or -1. */
int askBuilding(const char *prompt) {
    char buf[64];
    printf("%s (name or number): ", prompt);
    if (!fgets(buf, sizeof buf, stdin)) return -1;
    buf[strcspn(buf, "\r\n")] = 0;
    if (!buf[0]) return -1;

    char *end;
    long x = strtol(buf, &end, 10);
    if (*end == 0) {
        if (x >= 0 && x < n) return (int)x;
        printf("%sInvalid building number.%s\n", cRed, cReset);
        return -1;
    }

    int match = -1, count = 0;
    for (int i = 0; i < n; i++) {
        if (ieq(names[i], buf)) return i;
        if (icontains(names[i], buf)) { match = i; count++; }
    }
    if (count == 1) return match;
    if (count == 0) {
        printf("%sNo building matches \"%s\".%s\n", cRed, buf, cReset);
    } else {
        printf("%s\"%s\" is ambiguous. Matches:%s", cYellow, buf, cReset);
        for (int i = 0; i < n; i++)
            if (icontains(names[i], buf)) printf(" [%s]", names[i]);
        printf("\n");
    }
    return -1;
}
