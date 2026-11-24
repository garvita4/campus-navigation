# Campus Navigation System

A campus navigation system written in **C** that models a campus as a weighted, undirected **graph**—with buildings as vertices and walkways as weighted edges (distance in metres). It computes optimal walking routes, compares traversal algorithms, and exposes both a terminal interface and an interactive web map.

- **Live Demo:** https://campus-navigation-1-ia8k.onrender.com/ *(Hosted on Render's free tier; initial load may take ~50 seconds while the server wakes up.)*

## Overview

The project runs in two modes using the same C graph engine:

1. **Terminal Menu (CLI):** A standalone interactive C program with coloured output, step-by-step walking directions, distance matrices, and Graphviz export.
2. **Web Interface:** The C binary runs an embedded POSIX-socket HTTP server (`./campus --serve`) that exposes a JSON API. The browser frontend (`HTML`, `CSS`, and vanilla `JavaScript`) renders the campus map and queries the C backend for all route calculations.

## Features

### Core Graph & Routing
- **Weighted Graph Representation:** Models buildings, 2D map coordinates, walkway distances, and stair flags from an external configuration file.
- **Shortest Path (Dijkstra's Algorithm):** Computes the minimum-distance path between any two buildings with turn-by-turn directions, estimated walking time, and a route summary.
- **Graph Traversals (BFS & DFS):** Finds routes with the fewest walkways via Breadth-First Search and explores reachable campus components via Depth-First Search.

### Additional Capabilities
- **Algorithm Comparison:** Runs Dijkstra and BFS side by side to illustrate the difference between shortest distance and fewest hops.
- **All-Pairs Shortest Paths (Floyd-Warshall):** Generates a complete building-to-building distance matrix.
- **Multi-Stop Trip Planner:** Evaluates permutations with Dijkstra to find the optimal visiting order for up to 5 stops.
- **Accessibility Mode:** Filters out stairways to compute step-free routes.
- **Dynamic Walkway Blocking:** Temporarily blocks or unblocks specific walkways (e.g., construction or maintenance) and recalculates routes immediately.
- **Flexible Building Search:** Matches buildings by numeric ID or partial name (e.g., `lib` matches `Library`).
- **Data-Driven Map Configuration:** Loads nodes and edges from `data/campus.txt` at runtime without recompiling.
- **Graphviz Export:** Exports the campus graph to `campus.dot` for external visualization.
- **Interactive Web Map:** Supports click-to-select start/end buildings, live obstacle toggling, and light/dark themes.

## Project Structure

```text
campus-navigation/
├── Dockerfile              # Container build configuration for deployment
├── Makefile                # Build targets (campus, run, web, clean)
├── README.md               # Project documentation
├── data/
│   └── campus.txt          # Campus buildings, coordinates, and walkway definitions
├── include/                # Modular C header files
│   ├── bfs.h
│   ├── blocking.h
│   ├── campus.h            # Shared graph constants and data structures
│   ├── compare.h
│   ├── dfs.h
│   ├── dijkstra.h
│   ├── display.h
│   ├── export_dot.h
│   ├── floyd.h
│   ├── planner.h
│   ├── server.h
│   └── utils.h
├── src/                    # Modular C source files
│   ├── main.c              # Entry point and interactive CLI menu
│   ├── graph.c             # Graph storage and file parser
│   ├── dijkstra.c          # Shortest-distance routing
│   ├── bfs.c               # Fewest-walkways routing
│   ├── dfs.c               # Depth-first campus traversal
│   ├── floyd.c             # Floyd-Warshall all-pairs distance table
│   ├── compare.c           # Side-by-side Dijkstra vs. BFS comparison
│   ├── planner.c           # Multi-stop itinerary optimizer
│   ├── blocking.c          # Walkway closure management
│   ├── export_dot.c        # Graphviz .dot file generator
│   ├── display.c           # CLI formatting, directions, and summary tables
│   ├── utils.c             # String matching, input validation, and ANSI colours
│   └── server.c            # HTTP server, static file handler, and JSON API
└── web/                    # Browser frontend
    ├── index.html
    ├── css/
    │   └── style.css
    └── js/
        ├── api.js          # Fetch wrapper for /api/campus and /api/route
        ├── map.js          # SVG/Canvas map renderer and route highlighter
        └── app.js          # UI state, controls, and results panel
```

## Build & Run

### Prerequisites
- `gcc` and `make` on **macOS**, **Linux**, or **Windows (WSL)** *(the HTTP server relies on POSIX sockets)*.

### 1. Terminal Mode
Run all commands from the project root directory so relative paths to `data/campus.txt` and `web/` resolve properly:

```bash
make            # Compile the project into ./campus
./campus        # Launch the interactive terminal menu
```

### 2. Web Interface Mode
```bash
make web        # Builds and runs: ./campus --serve
```

Open **http://localhost:8080** in your browser. To specify a custom port:

```bash
./campus --serve 9000
```

To remove build artifacts:

```bash
make clean
```

## Terminal Menu Reference

| Option | Action |
| :--- | :--- |
| `1` | List all campus buildings |
| `2` | List all walkways, distances, and stair indicators |
| `3` | Find the shortest route (Dijkstra) with directions and summary |
| `4` | Compare Dijkstra (shortest distance) vs. BFS (fewest walkways) |
| `5` | Plan an optimal multi-stop route |
| `6` | Explore campus connectivity using DFS |
| `7` | Display the Floyd-Warshall all-pairs distance table |
| `8` | Block or unblock a walkway |
| `9` | Toggle accessibility mode (step-free routes only) |
| `10` | Export the campus graph to `campus.dot` (Graphviz) |
| `11` | Toggle ANSI terminal colours |
| `0` | Exit |

## Customizing the Campus Map

Edit `data/campus.txt` directly—no recompilation is required:

```text
N|Library|125|235            # N | Building Name | X | Y (web map coordinates)
E|Reception|Library|70|0     # E | From | To | Distance (m) | Stairs (1 = yes, 0 = no)
```

- **Capacity Limits:** Up to `20` buildings (`MAXN`) and `60` walkways (`MAXE`), configurable in `include/campus.h`.
- **Note:** Default distances in `data/campus.txt` are sample estimates and can be replaced with measured campus values.

## Web API

The C server exposes two JSON endpoints alongside static assets from `web/`:

| Endpoint | Description |
| :--- | :--- |
| `GET /api/campus` | Returns all buildings (nodes) and walkways (edges) as JSON. |
| `GET /api/route?from=0&to=5&algo=dij&acc=0&blocked=3,7` | Computes and returns the requested route as JSON. |

**Query Parameters for `/api/route`:**
- `from`, `to` — Source and destination building IDs (`0` to `N - 1`)
- `algo` — `dij` (Dijkstra) or `bfs` (Breadth-First Search)
- `acc` — `1` to enforce step-free routes, `0` otherwise
- `blocked` — Comma-separated list of blocked walkway IDs (optional)

**Sample Response:**
```json
{
  "ok": true,
  "nodes": [0, 2, 3, 5],
  "edges": [0, 3, 6],
  "distance": 170,
  "minutes": 3,
  "walkways": 3,
  "stairs": 0
}
```

## Algorithms & Complexity

Let $V$ be the number of buildings and $E$ be the number of walkways.

| Algorithm | Purpose | Time Complexity |
| :--- | :--- | :--- |
| **Dijkstra** (Adjacency Matrix) | Shortest route by walking distance | $O(V^2)$ |
| **BFS** | Route with fewest walkways | $O(V^2)$ |
| **DFS** | Reachability and campus exploration | $O(V^2)$ |
| **Floyd-Warshall** | All-pairs shortest-path distance table | $O(V^3)$ |
| **Permutations + Dijkstra** | Multi-stop trip planner ($k$ stops) | $O(k!)$ |

The graph uses an **adjacency matrix** representation, giving $O(1)$ edge lookups and keeping memory overhead minimal for typical campus sizes. All walkway distances are non-negative, satisfying Dijkstra's optimality condition.

## Future Improvements

- Binary min-heap priority queue with an adjacency list for large-scale campuses ($O((V + E)\log V)$).
- Time-dependent walkway weights to model peak-hour congestion between classes.
- $k$-shortest paths (Yen's algorithm) to display alternative routes alongside the primary path.