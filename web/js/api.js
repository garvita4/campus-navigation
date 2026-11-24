export async function getCampus() {
  const res = await fetch("/api/campus");
  if (!res.ok) throw new Error("Could not load the campus map");
  return res.json();
}

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
