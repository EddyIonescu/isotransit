// Generated by BUCKLESCRIPT VERSION 2.2.0, PLEASE EDIT WITH CARE
'use strict';

var App = require("./app.js");
var ReactDOMRe = require("reason-react/src/ReactDOMRe.js");
var ReasonReact = require("reason-react/src/ReasonReact.js");
var RegisterServiceWorker = require("./registerServiceWorker");

((require('./index.css')));

ReactDOMRe.renderToElementWithId(ReasonReact.element(/* None */0, /* None */0, App.make("Welcome to React and Reason", /* array */[])), "root");

RegisterServiceWorker.default();

/*  Not a pure module */
