[@bs.module "./map"] external map : ReasonReact.reactClass = "default";

let make = (~layers, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=map,
    ~props={"layers": layers},
    children
  );
