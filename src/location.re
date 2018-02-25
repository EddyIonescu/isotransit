[@bs.module "react-geosuggest"] external location : ReasonReact.reactClass = "default";
[%bs.raw {|require('react-geosuggest/module/geosuggest.css')|}];

let make = (~onSuggestSelect, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=location,
    ~props={"onSuggestSelect": onSuggestSelect},
    children
  );