[@bs.module "./react_components/map.jsx"] external map : ReasonReact.reactClass = "default";

let make = (~layers, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=map,
    ~props={"layers": layers},
    children
  );