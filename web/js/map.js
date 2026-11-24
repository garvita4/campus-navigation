let edgeEls = [];
let nodeEls = [];

export function drawMap(svg, campus, onNode, onEdge) {
  const { nodes, edges } = campus;

  const edgeSvg = edges.map(e => {
    const A = nodes[e.a], B = nodes[e.b];
    const mx = (A.x + B.x) / 2, my = (A.y + B.y) / 2;
    return `<g class="edge" data-i="${e.id}">
      <line class="hit" x1="${A.x}" y1="${A.y}" x2="${B.x}" y2="${B.y}"/>
      <line class="vis ${e.stairs ? "st" : ""}" x1="${A.x}" y1="${A.y}" x2="${B.x}" y2="${B.y}"/>
      <text x="${mx}" y="${my + 4}">${e.w}</text></g>`;
  }).join("");

  const nodeSvg = nodes.map(p =>
    `<g class="node" data-i="${p.id}" tabindex="0" role="button" aria-label="${p.name}">
      <circle cx="${p.x}" cy="${p.y}" r="9"/>
      <text x="${p.x}" y="${p.y + 27}">${p.name}</text></g>`
  ).join("");

  svg.innerHTML = edgeSvg + nodeSvg;
  edgeEls = [...svg.querySelectorAll(".edge")];
  nodeEls = [...svg.querySelectorAll(".node")];

  edgeEls.forEach(el => el.addEventListener("click", () => onEdge(+el.dataset.i)));
  nodeEls.forEach(el => {
    const act = () => onNode(+el.dataset.i);
    el.addEventListener("click", act);
    el.addEventListener("keydown", ev => {
      if (ev.key === "Enter" || ev.key === " ") { ev.preventDefault(); act(); }
    });
  });
}

export function paintRoute(route, blocked, start, end) {
  edgeEls.forEach((el, i) => {
    el.classList.toggle("blk", blocked.has(i));
    el.classList.toggle("on", route.ok && route.edges.includes(i));
  });
  nodeEls.forEach((el, i) => {
    el.classList.toggle("on", route.ok && route.nodes.includes(i));
    el.classList.toggle("end", i === start || i === end);
  });
}
