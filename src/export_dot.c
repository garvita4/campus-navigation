/* export_dot.c - export the campus graph for Graphviz */
#include <stdio.h>
#include "campus.h"
#include "export_dot.h"

void exportDot(void) {
    FILE *f = fopen("campus.dot", "w");
    if (!f) { printf("Could not write campus.dot\n"); return; }

    fprintf(f, "graph Campus {\n  layout=neato; overlap=false; splines=true;\n"
               "  node [shape=box, style=\"rounded,filled\", fillcolor=lightyellow];\n");
    for (int i = 0; i < n; i++)
        fprintf(f, "  \"%s\";\n", names[i]);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (w[i][j]) {
                fprintf(f, "  \"%s\" -- \"%s\" [label=\"%d m\"", names[i], names[j], w[i][j]);
                if (blocked[i][j])      fprintf(f, ", color=red, style=dashed");
                else if (stairs[i][j])  fprintf(f, ", color=orange, style=dotted");
                fprintf(f, "];\n");
            }
    fprintf(f, "}\n");
    fclose(f);
    printf("Saved campus.dot. To draw it, run:\n  dot -Tpng campus.dot -o campus.png\n"
           "(red dashed = blocked, orange dotted = stairs)\n");
}
