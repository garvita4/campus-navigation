# Campus Navigation System

A campus navigation project written in **C**. The campus is modelled as a **graph**: buildings are vertices and walkways are weighted edges (distance in metres). The program finds the shortest route between any two buildings and shows the path with its total distance.

It runs in two ways:

- **Terminal menu** - everything in plain C.
- **Web interface** - the C program runs a small HTTP server and calculates every route. The browser (HTML, CSS and a little JavaScript) only draws the map and asks C for the answer.

## Features

**Core (problem statement)**
- Campus modelled as a weighted, undirected graph
- Shortest route between any two buildings with the full path and total distance (Dijkstra)
- Graph traversals: BFS (fewest walkways) and DFS (explore the campus)

**Extra features**
- Compare Dijkstra and BFS side by side (shows why edge weights matter)
- All-pairs distance table (Floyd-Warshall)
- Multi-stop trip planner: best order to visit up to 5 places
- Accessibility mode: step-free routes that avoid stairs
- Block / unblock walkways (construction, flooding) and re-route instantly
- Turn-by-turn directions, walking time and a route summary box
- Search buildings by number or partial name (`lib` finds Library)
- Campus map loaded from a text file, so no recompiling to change it
- Coloured terminal output
- Graphviz export of the campus map
- Interactive web map with click-to-select buildings, light and dark theme

## Project structure

```
campus-navigation/
├── README.md
├── Makefile
├── data/
│   └── campus.txt        campus map: buildings, positions, walkways
├── include/              header files (one per module)
│   ├── campus.h          shared graph data
│   ├── utils.h  display.h  dijkstra.h  bfs.h  compare.h
│   ├── dfs.h  floyd.h  planner.h  blocking.h  export_dot.h  server.h
├── src/                  one source file per feature
│   ├── main.c            entry point and terminal menu
│   ├── graph.c           graph storage and file loading
│   ├── utils.c           colours, string helpers, input
│   ├── display.c         banner, directions, summary box
│   ├── dijkstra.c        shortest route (main requirement)
│   ├── bfs.c             fewest-walkways route
│   ├── compare.c         Dijkstra vs BFS
│   ├── dfs.c             campus exploration
│   ├── floyd.c           all-pairs distance table
│   ├── planner.c         multi-stop trip planner
│   ├── blocking.c        block / unblock walkways
│   ├── export_dot.c      Graphviz export
│   └── server.c          HTTP server and JSON API
└── web/                  browser interface
    ├── index.html
    ├── css/style.css
    └── js/
        ├── api.js        asks the C server for data
        ├── map.js        draws the map and highlights routes
        └── app.js        controls and results panel
```

## How to build and run

Requirements: `gcc` and `make` on macOS, Linux or Windows WSL. (The web server uses POSIX sockets, so plain Windows without WSL is not supported.)

```bash
make            # build the program
./campus        # terminal menu
```

For the web interface:

```bash
make web        # same as: ./campus --serve
```

Then open **http://localhost:8080**. To use another port: `./campus --serve 9000`.

Run the program from the project folder, because it reads `data/campus.txt` and `web/` using relative paths. Clean up with `make clean`.

## Terminal menu

| Option | What it does |
|---|---|
| 1, 2 | List buildings / walkways |
| 3 | Shortest route with directions and summary |
| 4 | Compare Dijkstra and BFS |
| 5 | Multi-stop trip planner |
| 6 | DFS exploration |
| 7 | Floyd-Warshall distance table |
| 8 | Block / unblock a walkway |
| 9 | Accessibility mode on / off |
| 10 | Export `campus.dot` for Graphviz |
| 11 | Colours on / off |

You can type a building number or a name, for example `lib` or `gate 1`.

## Using your own map

Edit `data/campus.txt`. No recompiling is needed.

```
N|Library|125|235            building name | x | y  (x,y place it on the web map)
E|Reception|Library|70|0     from | to | distance in metres | stairs (1 = yes, 0 = no)
```

The distances in the provided file are **estimates**. Replace them with real measurements. The limits are 20 buildings and 60 walkways (`MAXN` and `MAXE` in `include/campus.h`).

## Web API (served by the C program)

| Request | Returns |
|---|---|
| `GET /api/campus` | all buildings and walkways as JSON |
| `GET /api/route?from=0&to=5&algo=dij&acc=0&blocked=3,7` | the route as JSON |

Parameters: `algo` is `dij` or `bfs`, `acc=1` means step-free only, and `blocked` is a comma-separated list of walkway ids.

Example response:
```json
{"ok":true,"nodes":[0,2,3,5],"edges":[0,3,6],"distance":170,"minutes":3,"walkways":3,"stairs":0}
```

## Algorithms and complexity

V = number of buildings, E = number of walkways.

| Algorithm | Used for | Time |
|---|---|---|
| Dijkstra (adjacency matrix) | shortest route by distance | O(V²) |
| BFS | fewest walkways | O(V²) |
| DFS | exploration | O(V²) |
| Floyd-Warshall | all-pairs distances | O(V³) |
| Permutations + Dijkstra | multi-stop planner (k stops) | O(k!) |

The graph is stored as an adjacency matrix. This keeps the code simple, and checking whether two buildings are directly connected takes O(1) time. Dijkstra requires non-negative weights, which is always true for walking distances.

## Possible improvements

- Min-heap Dijkstra with an adjacency list for very large campuses
- Rush-hour walkway weights that change with the time of day
- Top 3 alternative routes
