[@bs.module "./react_components/map.jsx"] external map : ReasonReact.reactClass = "default";

let make = (~selectedLocation, ~layers, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=map,
    ~props={"selectedLocation": selectedLocation, "layers": layers},
    children
  );