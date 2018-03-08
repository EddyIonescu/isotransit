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
  | AddIsochrone(polygons);

type state = {
  selectedLocation: location,
  selectedTime: float,
  movingAwayIso: bool,
  selectedTravelType: travelType,
  timeLengthMinutes: int,
  shortTripTimeLengthMinutes: int,
  isochrones: list(isochrone),
  layers: polygons,
};

type transportOption = {
  . "value": travelType,
    "label": string,
};

let selectOptions: array(transportOption) = [|
  {"value": Transit, "label": "GRT/Walking (30 min)"},
  {"value": Driving, "label": "Driving (30 min)"},
  {"value": Ion, "label": "ION LRT + 10-minute Rideshare"},
  {"value": IonTransitOnly, "label": " ION LRT + 20-minute GRT/Walking"}
|];

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
  let params = {
    "waypoint": string_of_float(selectedLocation.lat) ++ ","
                ++ string_of_float(selectedLocation.lng),
    "maxTime": switch (state.selectedTravelType) {
      | Transit => state.timeLengthMinutes
      | Driving => state.timeLengthMinutes
      | Ion => state.shortTripTimeLengthMinutes
      | IonTransitOnly => 20
    },
    "timeUnit": "minute",
    "dateTime": Js.Date.toUTCString(Js.Date.fromFloat(state.selectedTime)),
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
      ++ "ApZR_vPMZyeXNjB8t5TKwBrom8CtFd-dauFRd4cylgNC8Jnx9Ppxmbqfy9q_Anez",
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
  )
};

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {
    selectedLocation: {lat: 43.4684405, lng:  -80.5418298},
    selectedTime: Js.Date.now(),
    timeLengthMinutes: 30,
    shortTripTimeLengthMinutes: 10,
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
    | AddIsochrone(polygons) =>
        ReasonReact.Update({...state, layers: polygons})
    },
  render: (_self) =>
    <div>
      <Location 
        onSuggestSelect=(
          (s) => _self.send(
            SelectedSuggestion({lat: s##location##lat, lng: s##location##lng})
        ))
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
          Js.Promise.resolve(_self.send(AddIsochrone(polygons)));
        })
        |> Js.Promise.catch ((error) => {
          Js.Promise.resolve(Js.log(error));
        });
        ()
      })
      >
        <h2>{ReasonReact.stringToElement("Generate Isochrone")}</h2>
      </button>
      <DateTimePicker
        selectedTime=_self.state.selectedTime
        onChange=(selectedTime => _self.send(UpdateSelectedTime(selectedTime)))
      />
      <SelectType
        onChange=((t) => _self.send(TransportSelection(t##value)))
        options=selectOptions
        value=_self.state.selectedTravelType
      />
      <Map layers=_self.state.layers />
    </div>
};
