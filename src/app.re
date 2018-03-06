[%bs.raw {|require('./app.css')|}];

type location = {lat: float, lng: float};
type travelType = Transit | Driving | AV;
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
  isochrones: list(isochrone),
  layers: polygons,
};

type transportOption = {
  . "value": travelType,
    "label": string,
};

let selectOptions: array(transportOption) = [|
  {"value": Transit, "label": "Transit (30 minutes)"},
  {"value": Driving, "label": "Driving (30 minutes)"},
  {"value": AV, "label": "Driving (max 10 minutes per trip) + ION LRT/BRT"}
|];

let ionStops = [
  [-80.52945144,  43.49813693],
  [-80.54321203,  43.49721126],
  [-80.54515806,	43.48162357],
  [-80.5411648,	  43.4732381],
  [-80.53440443,	43.46885718],
  [-80.52298867,	43.46414138],
  [-80.52336904,	43.462144],
  [-80.51836079,	43.45983591],
  [-80.49913146,	43.45317562],
  [-80.48734599,	43.44964771],
  [-80.49370802,	43.4501848],
  [-80.4894232,	  43.448626],
  [-80.4753731,	  43.44240342],
  [-80.48359184,	43.4462947],
  [-80.47787648,	43.43358158],
  [-80.39313812,	43.41029453],
  [-80.32804673,	43.40722426],
  [-80.32107051,	43.38546487],
  [-80.31882873,	43.37392952],
  [-80.38889975,	43.413086],
  [-80.36222258,	43.4004675],
  [-80.3209874,	  43.38636103],
  [-80.31819008,	43.37287123],
  [-80.32301446,	43.39279373],
  [-80.32774659,	43.40761002],
  [-80.44180343,	43.42232678],
  [-80.31368034,	43.357354],
  [-80.49080288,	43.45192775],
  [-80.46308195,	43.42282486],
  [-80.51205622,	43.45728346],
];

let getIsochrone = (
  selectedLocation: location,
  selectedTravelType: travelType,
  selectedTime: float,
  timeLengthMinutes: int
) => {
  let params = {
    "waypoint": string_of_float(selectedLocation.lat) ++ ","
                ++ string_of_float(selectedLocation.lng),
    "maxTime": timeLengthMinutes,
    "timeUnit": "minute",
    "dateTime": Js.Date.toUTCString(Js.Date.fromFloat(selectedTime)),
    "travelMode": switch (selectedTravelType) {
        | Transit => "transit"
        | Driving => "driving"
      },
    "optimize": switch (selectedTravelType) {
      | Transit => "time"
      | Driving => "timeWithTraffic"
    }
  };
  Js.Promise.(
    Axios.postData(
      "https://dev.virtualearth.net/REST/v1/Routes/Isochrones?key="
      ++ "Avj-tmXDH_oOb5wmY1yfBkJNSG4hBUVMraHAP5upxDqBLj6ni747Lagw40_4SsVZ",
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
        Js.Promise.(
        getIsochrone(
          _self.state.selectedLocation,
          _self.state.selectedTravelType,
          _self.state.selectedTime,
          _self.state.timeLengthMinutes
        )
          |> then_ ((polygons) => {
            Js.log(polygons);
            Js.Promise.resolve(_self.send(AddIsochrone(polygons)));
          })
          |> catch ((error) => {
            Js.Promise.resolve(Js.log(error));
          })
        );
        ()
      })
      >
        <h2>{ReasonReact.stringToElement("Get Basic Isochrone")}</h2>
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
