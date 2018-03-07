IsoTransit

IsoTransit is a map that allows you to make time-based isochrones to see how far you can get by driving or public transit.

## Driving
- powered by Bing Maps Isochrone API using real-time traffic data

## Transit
- powered by Bing Maps Isochrone API likely using GTFS data
- future plan is to use Valhalla with GTFS due to increased flexibility and performance (Bing Maps doesn't allow for customizing walking speed, biking, and errors when the waits for buses are too long)

## Short Drive + Rapid Transit
- while trains and subways are great, getting to them is often inconvenient. Sometimes it's so inconvenient (ie. slow feeder buses or lack of parking) that other alternatives are taken. Thanks to Uber Pool/Lyft Line, short rides are now a lot cheaper, opening up new ways of getting around.
- the isochrone can only be limited by how long a drive can be. There can be unlimited drives as long as they start or end at a transit station.

## Getting Started

IsoTransit uses ReasonReact, a framework made by Facebook that using Bucklescript (OCaml-to-JS) compiles ReasonML (OCaml but more Javascripty) into ReactJS. The resulting ReactJS code is clean, optimized, pure, and of course, type-safe.

MapBox (Mapbox.GL) is used for the map. Deck.GL is used to draw the isochrones. Google Maps API is used for the suggestions bar, and Bing Maps Isochrone API is used to generate the isochrone shapes.

