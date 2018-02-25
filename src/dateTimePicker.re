[@bs.module "./react_components/dateTimePicker.jsx"] external picker : ReasonReact.reactClass = "default";

let make = (~selectedTime, ~onChange, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=picker,
    ~props={"selectedTime": selectedTime, "onChange": onChange},
    children
  );
