// api.js - talks to the C server (see src/server.c)

export async function getCampus() {
  const res = await fetch("/api/campus");
  if (!res.ok) throw new Error("Could not load the campus map");
  return res.json();
}

// The C program calculates the route; JavaScript only asks for it.
export async function getRoute({ from, to, algo, acc, blocked }) {
  const url =
    "/api/route?from=" +
    from +
    "&to=" +
    to +
    "&algo=" +
    algo +
    "&acc=" +
    (acc ? 1 : 0) +
    "&blocked=" +
    [...blocked].join(",");
  const res = await fetch(url);
  return res.json();
}
