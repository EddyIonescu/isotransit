[%bs.raw {|require('./app.css')|}];

type location = {lat: float, lng: float};
type travelType = Transit | Driving;
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
  {"value": Transit, "label": "Transit"},
  {"value": Driving, "label": "Driving"},
|];

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
