import 'react-widgets/dist/css/react-widgets.css';
import Moment from 'moment/moment';
import momentLocalizer from 'react-widgets-moment';
import { DateTimePicker } from 'react-widgets';
import React from 'react';

// wrapper for react-widgets datetime-picker
// (needed as to init localization)

// needed to render DateTime component
// https://jquense.github.io/react-widgets/localization/
Moment.locale('en');
momentLocalizer();

/*
 * selectedTime: Number
 * onChange: Number -> Unit
 */ 
export default ({ selectedTime, onChange }) => (
  <DateTimePicker
    value={new Date(selectedTime)}
    onChange={newTime => onChange(Number(newTime))}
  />
);
