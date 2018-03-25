[@bs.module "../node_modules/react-select/lib/Select.js"] external select : ReasonReact.reactClass = "default";
[%bs.raw {|require('react-select/dist/react-select.css')|}];

let make = (~disabled=false, ~onChange, ~options, ~value, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=select,
    ~props={
      "disabled": disabled,
      "onChange": onChange,
      "options": options,
      "value": value
    },
    children
  );