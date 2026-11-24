// app.js - page logic: controls, results panel, events

import { getCampus, getRoute } from "./api.js";
import { drawMap, paintRoute } from "./map.js";

const $ = s => document.querySelector(s);
const from = $("#from"), to = $("#to"), out = $("#out");
const blocked = new Set();
let campus = null;
let pick = 1;          // which dropdown the next map click fills
let latest = 0;        // ignores out-of-date responses

async function init() {
  try {
    campus = await getCampus();
  } catch {
    out.innerHTML = '<p class="err">Cannot reach the C server.</p>' +
      '<p class="note">Start it with <b>make web</b> and open http://localhost:8080</p>';
    return;
  }

  campus.nodes.forEach(n => {
    from.add(new Option(n.name, n.id));
    to.add(new Option(n.name, n.id));
  });
  from.value = 0;
  to.value = Math.min(5, campus.nodes.length - 1);

  drawMap($("#map"), campus, onNode, onEdge);

  [from, to, $("#algo"), $("#acc")].forEach(el => el.addEventListener("change", refresh));
  $("#reset").addEventListener("click", () => { blocked.clear(); refresh(); });
  refresh();
}

function onNode(id) {
  (pick ? to : from).value = id;
  pick = 1 - pick;
  refresh();
}

function onEdge(id) {
  blocked.has(id) ? blocked.delete(id) : blocked.add(id);
  refresh();
}

async function refresh() {
  const ticket = ++latest;
  const s = +from.value, t = +to.value;
  const route = await getRoute({ from: s, to: t, algo: $("#algo").value, acc: $("#acc").checked, blocked });
  if (ticket !== latest) return;
  paintRoute(route, blocked, s, t);
  showResult(route, s, t);
}

function showResult(r, s, t) {
  if (s === t) {
    out.innerHTML = '<p class="note">Start and destination are the same building.</p>';
    return;
  }
  if (!r.ok) {
    out.innerHTML = '<p class="err">No route found.</p>' +
      '<p class="note">Reopen a blocked walkway or turn off the step-free option.</p>';
    return;
  }

  const name = id => campus.nodes[id].name;
  const steps = r.edges.map((id, k) => {
    const e = campus.edges[id];
    return `<li>Walk ${e.w} m to <b>${name(r.nodes[k + 1])}</b>${e.stairs ? " (take the stairs)" : ""}</li>`;
  }).join("");

  out.innerHTML = `
    <div class="stats">
      <div class="stat"><b>${r.distance} m</b><span>Total distance</span></div>
      <div class="stat"><b>${r.minutes} min</b><span>Walking time</span></div>
      <div class="stat"><b>${r.walkways}</b><span>Walkways used</span></div>
      <div class="stat"><b>${r.stairs ? r.stairs + " with stairs" : "None"}</b><span>Stairs</span></div>
    </div>
    <p class="note">Start at <b>${name(r.nodes[0])}</b></p>
    <ol>${steps}</ol>
    ${$("#algo").value === "bfs" ? '<p class="note">This method minimises the number of walkways, not the distance.</p>' : ""}`;
}

init();
