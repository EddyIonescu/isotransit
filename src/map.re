[@bs.module "./react_components/map.jsx"] external map : ReasonReact.reactClass = "default";

let make = (~selectedLocation, ~isochrones, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=map,
    ~props={"selectedLocation": selectedLocation, "isochrones": isochrones},
    children
  );