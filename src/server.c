/* server.c - tiny HTTP server in C.
 *   GET /api/campus                      -> buildings + walkways as JSON
 *   GET /api/route?from=0&to=5&algo=dij&acc=0&blocked=3,7
 *                                        -> route computed by dijkstra.c / bfs.c
 *   GET anything else                    -> static file from web/
 * Uses POSIX sockets (macOS / Linux / WSL). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "campus.h"
#include "dijkstra.h"
#include "bfs.h"
#include "server.h"

#define BUF_SIZE 8192
static const char *WEB_ROOT = "web";

/* ---------- low-level helpers ---------- */
static void sendAll(int fd, const char *buf, size_t len) {
    while (len > 0) {
        ssize_t k = write(fd, buf, len);
        if (k <= 0) return;
        buf += k;
        len -= (size_t)k;
    }
}

static void sendResponse(int fd, int code, const char *status, const char *type,
                         const char *body, size_t len) {
    char head[256];
    int h = snprintf(head, sizeof head,
                     "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n"
                     "Cache-Control: no-store\r\nConnection: close\r\n\r\n",
                     code, status, type, len);
    sendAll(fd, head, (size_t)h);
    sendAll(fd, body, len);
}

static void sendJson(int fd, int code, const char *status, const char *json) {
    sendResponse(fd, code, status, "application/json", json, strlen(json));
}

/* Read ?key=value from a query string. Returns 1 if found. */
static int getParam(const char *query, const char *key, char *out, size_t size) {
    size_t kl = strlen(key);
    const char *p = query;
    while (p && *p) {
        if (strncmp(p, key, kl) == 0 && p[kl] == '=') {
            p += kl + 1;
            size_t i = 0;
            while (*p && *p != '&' && i < size - 1) out[i++] = *p++;
            out[i] = 0;
            return 1;
        }
        p = strchr(p, '&');
        if (p) p++;
    }
    return 0;
}

/* ---------- API: /api/campus ---------- */
static void apiCampus(int fd) {
    static char buf[BUF_SIZE];
    int off = 0;
    off += snprintf(buf + off, sizeof buf - off, "{\"nodes\":[");
    for (int i = 0; i < n; i++)
        off += snprintf(buf + off, sizeof buf - off,
                        "%s{\"id\":%d,\"name\":\"%s\",\"x\":%d,\"y\":%d}",
                        i ? "," : "", i, names[i], posx[i], posy[i]);
    off += snprintf(buf + off, sizeof buf - off, "],\"edges\":[");
    for (int e = 0; e < m; e++)
        off += snprintf(buf + off, sizeof buf - off,
                        "%s{\"id\":%d,\"a\":%d,\"b\":%d,\"w\":%d,\"stairs\":%d}",
                        e ? "," : "", e, edgeA[e], edgeB[e],
                        w[edgeA[e]][edgeB[e]], stairs[edgeA[e]][edgeB[e]]);
    snprintf(buf + off, sizeof buf - off, "]}");
    sendJson(fd, 200, "OK", buf);
}

/* ---------- API: /api/route ---------- */
static void apiRoute(int fd, const char *query) {
    char v[256];
    if (!getParam(query, "from", v, sizeof v)) { sendJson(fd, 400, "Bad Request", "{\"error\":\"missing from\"}"); return; }
    int s = atoi(v);
    if (!getParam(query, "to", v, sizeof v))   { sendJson(fd, 400, "Bad Request", "{\"error\":\"missing to\"}"); return; }
    int t = atoi(v);
    if (s < 0 || s >= n || t < 0 || t >= n) { sendJson(fd, 400, "Bad Request", "{\"error\":\"invalid building\"}"); return; }

    int useBfs = getParam(query, "algo", v, sizeof v) && strcmp(v, "bfs") == 0;
    accessibleMode = getParam(query, "acc", v, sizeof v) && v[0] == '1';

    clearBlocked();
    if (getParam(query, "blocked", v, sizeof v))
        for (char *tok = strtok(v, ","); tok; tok = strtok(NULL, ","))
            blockEdge(atoi(tok));

    int path[MAXN], len = 0;
    int dist = useBfs ? bfs(s, t, path, &len) : dijkstra(s, t, path, &len);
    if (dist < 0) { sendJson(fd, 200, "OK", "{\"ok\":false}"); return; }

    static char buf[BUF_SIZE];
    int off = 0, stairCount = 0;
    off += snprintf(buf + off, sizeof buf - off, "{\"ok\":true,\"nodes\":[");
    for (int i = 0; i < len; i++)
        off += snprintf(buf + off, sizeof buf - off, "%s%d", i ? "," : "", path[i]);
    off += snprintf(buf + off, sizeof buf - off, "],\"edges\":[");
    for (int i = 0; i < len - 1; i++) {
        if (stairs[path[i]][path[i + 1]]) stairCount++;
        off += snprintf(buf + off, sizeof buf - off, "%s%d", i ? "," : "",
                        eid[path[i]][path[i + 1]] - 1);
    }
    snprintf(buf + off, sizeof buf - off,
             "],\"distance\":%d,\"minutes\":%d,\"walkways\":%d,\"stairs\":%d}",
             dist, (dist + WALK_SPEED - 1) / WALK_SPEED, len - 1, stairCount);
    sendJson(fd, 200, "OK", buf);
}

/* ---------- static files from web/ ---------- */
static const char *mimeType(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (!strcmp(ext, ".html")) return "text/html; charset=utf-8";
    if (!strcmp(ext, ".css"))  return "text/css; charset=utf-8";
    if (!strcmp(ext, ".js"))   return "text/javascript; charset=utf-8";
    if (!strcmp(ext, ".json")) return "application/json";
    if (!strcmp(ext, ".svg"))  return "image/svg+xml";
    if (!strcmp(ext, ".png"))  return "image/png";
    return "application/octet-stream";
}

static void serveFile(int fd, const char *urlPath) {
    if (strstr(urlPath, "..")) {
        sendResponse(fd, 403, "Forbidden", "text/plain", "Forbidden", 9);
        return;
    }
    char full[300];
    snprintf(full, sizeof full, "%s%s%s", WEB_ROOT, urlPath,
             strcmp(urlPath, "/") == 0 ? "index.html" : "");

    FILE *f = fopen(full, "rb");
    if (!f) { sendResponse(fd, 404, "Not Found", "text/plain", "404 Not Found", 13); return; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *data = malloc((size_t)size + 1);
    if (!data) { fclose(f); return; }
    size_t got = fread(data, 1, (size_t)size, f);
    fclose(f);
    sendResponse(fd, 200, "OK", mimeType(full), data, got);
    free(data);
}

/* ---------- request handling ---------- */
static void handleClient(int fd) {
    char req[BUF_SIZE];
    ssize_t r = read(fd, req, sizeof req - 1);
    if (r <= 0) return;
    req[r] = 0;

    char method[8], target[512];
    if (sscanf(req, "%7s %511s", method, target) != 2) return;
    if (strcmp(method, "GET") != 0) {
        sendResponse(fd, 405, "Method Not Allowed", "text/plain", "405", 3);
        return;
    }

    char *q = strchr(target, '?');
    const char *query = "";
    if (q) { *q = 0; query = q + 1; }

    if (strcmp(target, "/api/campus") == 0)     apiCampus(fd);
    else if (strcmp(target, "/api/route") == 0) apiRoute(fd, query);
    else                                        serveFile(fd, target);
}

int startServer(int port) {
    signal(SIGPIPE, SIG_IGN);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   /* this computer only */
    addr.sin_port = htons((unsigned short)port);

    if (bind(srv, (struct sockaddr *)&addr, sizeof addr) < 0) { perror("bind"); return 1; }
    if (listen(srv, 16) < 0) { perror("listen"); return 1; }

    printf("Campus server running.\nOpen http://localhost:%d in your browser (Ctrl+C to stop)\n", port);
    fflush(stdout);

    for (;;) {
        int client = accept(srv, NULL, NULL);
        if (client < 0) continue;
        handleClient(client);
        close(client);
    }
}
