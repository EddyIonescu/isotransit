// Generated by BUCKLESCRIPT VERSION 2.2.0, PLEASE EDIT WITH CARE
'use strict';

var ReasonReact = require("reason-react/src/ReasonReact.js");
var SelectJs = require("../node_modules/react-select/lib/Select.js");

((require('react-select/dist/react-select.css')));

function make($staropt$star, onChange, options, value, children) {
  var disabled = $staropt$star ? $staropt$star[0] : /* false */0;
  return ReasonReact.wrapJsForReason(SelectJs.default, {
              disabled: disabled,
              onChange: onChange,
              options: options,
              value: value
            }, children);
}

exports.make = make;
/*  Not a pure module */
