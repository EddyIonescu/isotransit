[@bs.module "../node_modules/react-select/lib/Select.js"] external select : ReasonReact.reactClass = "default";
[%bs.raw {|require('react-select/dist/react-select.css')|}];

let make = (~onChange, ~options, ~value, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=select,
    ~props={"onChange": onChange, "options": options, "value": value},
    children
  );