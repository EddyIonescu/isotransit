[%bs.raw {|require('./app.css')|}];

type location = {lat: float, lng: float};
type travelType = Transit | Driving | Ion | IonTransitOnly;
type isochrone = {
  location: location,
  time: int,
  movingAway: bool,
  travelType: travelType,
  drawing: string,
};

/*
 * Bing Maps Isochrone API result
 * https://msdn.microsoft.com/en-us/library/mt814919.aspx?f=255&MSPPError=-2147217396
 */ 
type polygons = array({
  . "coordinates": array(array(float))
});

type action =
  | SelectedSuggestion(location)
  | TransportSelection(travelType)
  | UpdateSelectedTime(float)
  | UpdateTimeLength(int)
  | UpdateRideshareTimeLength(int)
  | AddIsochrone(polygons);

type state = {
  selectedLocation: location,
  selectedTime: float,
  movingAwayIso: bool,
  selectedTravelType: travelType,
  timeLengthMinutes: int,
  rideshareTimeLengthMinutes: int,
  isochrones: list(isochrone),
  layers: polygons,
};

type transportOption = {
  . "value": travelType,
    "label": string,
};

let selectOptions: array(transportOption) = [|
  {"value": Transit, "label": "Transit"},
  {"value": Driving, "label": "Driving"},
  {"value": Ion, "label": "ION LRT + Rideshare"},
  {"value": IonTransitOnly, "label": " ION LRT + Transit"}
|];

type timeIntervalOption = {
  . "value": int,
    "label": string,
};

let rec makeTimeIntervals = (first: int, last: int, jump: int) : list(int) =>
  (first > last) ? [] : [first, ...makeTimeIntervals(first + jump, last, jump)];

let selectTimeIntervalOptions (first, last, jump): array(timeIntervalOption) = 
  Array.of_list(
    List.map(
      (v) => {"value": v, "label": string_of_int(v) ++ " minutes"},
      makeTimeIntervals(first, last, jump)
    )
  );


/* TODO - pls put into json */
let ionStops = [|
  [| -80.52945144,	43.49813693 |],
  [| -80.54321203,	43.49721126 |],
  [| -80.54515806,	43.48162357 |],
  [| -80.5411648,	43.4732381 |],
  [| -80.53440443,	43.46885718 |],
  [| -80.52298867,	43.46414138 |],
  [| -80.52336904,	43.462144 |],
  [| -80.51836079,	43.45983591 |],
  [| -80.49913146,	43.45317562 |],
  [| -80.48734599,	43.44964771 |],
  [| -80.49370802,	43.4501848 |],
  [| -80.4894232,	43.448626 |],
  [| -80.4753731,	43.44240342 |],
  [| -80.48359184,	43.4462947 |],
  [| -80.47787648,	43.43358158 |],
  [| -80.44180343,	43.42232678 |],
  [| -80.49080288,	43.45192775 |],
  [| -80.46308195,	43.42282486 |],
  [| -80.51205622,	43.45728346 |],
|];

let getIsochrone = (state: state, specifiedLocation: option(location)) => {
  let selectedLocation = switch(specifiedLocation) {
    | None => state.selectedLocation
    | Some(loc) => loc
  };
  let offset = switch (state.selectedTravelType) {
  | Transit | IonTransitOnly => -18000000 /* Bing Maps Transit is buggy */
  | _ => 0
  };
  let params = {
    "waypoint": string_of_float(selectedLocation.lat) ++ ","
                ++ string_of_float(selectedLocation.lng),
    "maxTime": switch (state.selectedTravelType) {
      | Transit => state.timeLengthMinutes
      | Driving => state.timeLengthMinutes
      | Ion => state.rideshareTimeLengthMinutes
      | IonTransitOnly => state.timeLengthMinutes
    },
    "timeUnit": "minute",
    "dateTime": Js.Date.toUTCString(Js.Date.fromFloat(state.selectedTime +. float_of_int(offset))),
    "travelMode": switch (state.selectedTravelType) {
        | Transit => "transit"
        | Driving => "driving"
        | Ion => "driving"
        | IonTransitOnly => "transit"
      },
    "optimize": switch (state.selectedTravelType) {
      | Transit => "time"
      | Driving => "timeWithTraffic"
      | Ion => "timeWithTraffic"
      | IonTransitOnly => "time"
    }
  };
  Js.Promise.(
    Axios.postData(
      "https://dev.virtualearth.net/REST/v1/Routes/Isochrones?key="
      ++ "AtVkKGv4bwzxrZD4WGdKDwPH3We4IJfA_TrzRWJbRMgrHxtT0nSqPFL4JN9TFEID",
      /* TODO - put dev-key into keysConfig file */
      {params}
    )
    |> then_ ((response) => {
         Js.Promise.resolve(
           (List.nth(
             (List.nth(response##data##resourceSets, 0))##resources, 0)
           )##polygons
         );
    })
    |> catch ((error) => {
      Js.Promise.resolve(None)
    })
  )
};

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {
    selectedLocation: {lat: 43.4684405, lng:  -80.5418298},
    selectedTime: Js.Date.now(),
    timeLengthMinutes: 20,
    rideshareTimeLengthMinutes: 10,
    movingAwayIso: true,
    selectedTravelType: Transit,
    isochrones: [],
    layers: [||],
  },
  reducer: (action, state) =>
    switch (action) {
    | SelectedSuggestion(selectedLocation) =>
        ReasonReact.Update({...state, selectedLocation})
    | TransportSelection(selectedTravelType) =>
        ReasonReact.Update({...state, selectedTravelType})
    | UpdateSelectedTime(selectedTime) =>
        ReasonReact.Update({...state, selectedTime})
    | UpdateTimeLength(timeLengthMinutes) =>
        ReasonReact.Update({...state, timeLengthMinutes})
    | UpdateRideshareTimeLength(rideshareTimeLengthMinutes) =>
        ReasonReact.Update({...state, rideshareTimeLengthMinutes})
    | AddIsochrone(polygons) =>
        ReasonReact.Update({...state, layers: Array.append(state.layers, polygons)})
    },
  render: (_self) =>
    <div className="app-container">
      <div className="options-control">
        <div className="search-location">
          <Location 
            onSuggestSelect=(
              (s) => _self.send(
                SelectedSuggestion({lat: s##location##lat, lng: s##location##lng})
            ))
          />
        </div>
        <h3>{ReasonReact.stringToElement("Departure Time")}</h3>
        <DateTimePicker
          selectedTime=_self.state.selectedTime
          onChange=(selectedTime => _self.send(UpdateSelectedTime(selectedTime)))
        />
        <h3>{ReasonReact.stringToElement("Transportation Mode")}</h3>
        <SelectType
          onChange=((t) => _self.send(TransportSelection(t##value)))
          options=selectOptions
          value=_self.state.selectedTravelType
        />
        <h3>{ReasonReact.stringToElement("Total Trip Duration")}</h3>
        <SelectType
          onChange=((t) => _self.send(UpdateTimeLength(t##value)))
          options=selectTimeIntervalOptions(5, 60, 5)
          value=_self.state.timeLengthMinutes
        />
        <h3>{ReasonReact.stringToElement("Max Duration of Ridesharing")}</h3>
        <SelectType
          disabled=(switch(_self.state.selectedTravelType) {
          | Ion => false
          | _ => true
          })
          onChange=((t) => _self.send(UpdateRideshareTimeLength(t##value)))
          options=selectTimeIntervalOptions(1, 15, 1)
          value=_self.state.rideshareTimeLengthMinutes
        />
        <button onClick=((_) => {
          Js.Promise.all(
            (switch(_self.state.selectedTravelType) {
              | Ion | IonTransitOnly => 
                  Array.map((ionStop) => Js.Promise.(getIsochrone(_self.state,
                          Some({lat: ionStop[1], lng: ionStop[0]})
                        )), ionStops)
              | Transit | Driving => [| Js.Promise.(getIsochrone(_self.state, None)) |];
            })
          )
          |> Js.Promise.then_ ((polygons) => {
            Js.log(polygons);
            Js.Promise.resolve(_self.send(AddIsochrone(
              Js.Array.reduce((acc, polygon) => switch(polygon) {
              | None => acc;
              | Some(p) => Js.Array.push(p, acc); acc
              }, [||], polygons)
            )));
          })
          |> Js.Promise.catch ((error) => {
            Js.Promise.resolve(Js.log(error));
          });
          ()
        })
        >
          <h2>{ReasonReact.stringToElement("Generate Isochrone")}</h2>
        </button>
      </div>
      <div className="map">
        <Map 
          selectedLocation=({
            "lat": _self.state.selectedLocation.lat,
            "lng": _self.state.selectedLocation.lng
          }) 
          layers=_self.state.layers
        />
      </div>
    </div>
};
