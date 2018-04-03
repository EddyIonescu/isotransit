import _ from 'lodash';
import React, { Component } from 'react';
import ReactMapGL, { NavigationControl } from 'react-map-gl';
import DeckGL, { PolygonLayer, IconLayer } from 'deck.gl';

import { MAP_STYLE, MAPBOX_ACCESS_TOKEN } from '../keysConfig.json';
import "./map.css";
import ionStops from '../ionStops.json';

const atlasIcon = require('../res/icon-atlas.png');

class Map extends Component {
  constructor() {
    super();
    this.state = {
      // Viewport settings that is shared between mapbox and deck.gl
      viewport: {
        width: window.innerWidth,
        height: window.innerHeight,
        zoom: 12,
        pitch: 0,
        bearing: 0,
      },
      settings: {
        dragPan: true,
        minZoom: 0,
        maxZoom: 20,
        minPitch: 0,
        maxPitch: 85,
      },
    };
  }

  componentWillMount() {
    this.selectedRoutes = new Set();
    this.resizeMap();
    window.addEventListener('resize', this.resizeMap.bind(this));
  }

  componentWillUnmount() {
    window.removeEventListener('resize', this.resizeMap.bind(this));
  }
  
  resizeMap() {
    this.setState({
      viewport: Object.assign(this.state.viewport, {
        width: window.innerWidth,
        height: window.innerHeight,
      }),
    });
  }

  render() {
    let { selectedLocation, isochrones } = this.props;
    const onViewportChange = viewport => {
      selectedLocation.lat = viewport.latitude;
      selectedLocation.lng = viewport.longitude;
      this.setState({ viewport });
    };
    const { viewport } = this.state;
    console.log(isochrones);
    isochrones = isochrones.map(iso => ({
      layers: _.flatten(iso[4])
              .map(layer => ([layer.coordinates]))
              .map(layer => 
                layer.map(polygons => 
                  polygons.map(coords => 
                    coords.map(coord => 
                      ([coord[1], coord[0]])))))
    })).filter(iso => !!iso.layers); // should be drawing field - TODO - fix!
    /*
    // flatten once as it contains multiple isochrones of the same colour
    layers = _.flatten(layers);
    // put each shape under coordinates into a list
    layers = layers.map(layer => ([layer.coordinates]));
    // reverse lat,lng to lng,lat
    // lat,lng - geography/transportation/science - lng,lat - CS
    layers = layers.map(layer => layer.map(polygons => polygons.map(coords => coords.map(coord => ([coord[1], coord[0]])))));
    console.log(layers);
    */

    const colors = [
      [38, 204, 107, 150], // green - 5 minutes
      [38, 195, 204, 120], // turquoise - 10 minutes
      [66, 185, 244, 100], // sky blue - 15 minutes
      [38, 107, 204, 90], // blue - 20 minutes
      [137, 66, 244, 80], // violet - 25 minutes
    ];
    colors.reverse();

    const ICON_MAPPING = {
      marker: {
        x: 0, y: 0, width: 128, height: 128, mask: true,
      },
    };

    console.log(ionStops);
    const ionStopsLayer = new IconLayer({
      id: 'ion-stops-layer',
      data: ionStops.map(ionStop => ({
        position: [ionStop[0], ionStop[1]], 
        icon: 'marker',
        size: 72,
        color: [20, 50, 250],
      })),
      iconAtlas: atlasIcon,
      iconMapping: ICON_MAPPING,
    });
    console.log(ionStopsLayer);
    console.log(selectedLocation);
    return (
      <ReactMapGL
        {...viewport}
        longitude={selectedLocation.lng}
        latitude={selectedLocation.lat}
        mapStyle={MAP_STYLE}
        mapboxApiAccessToken={MAPBOX_ACCESS_TOKEN}
        onViewportChange={onViewportChange}
      >
        <DeckGL
          longitude={selectedLocation.lng}
          latitude={selectedLocation.lat}
          {...viewport}
          layers={[...(isochrones.length ? isochrones.map((iso, idx) =>
            new PolygonLayer({
              id: `polygon-layer${idx}`,
              data: iso.layers.map(polygon => ({
                polygon: polygon[0][0],
                fillColor: colors[idx % colors.length]
              })),
              filled: true,
              stroked: false,
            })) : []), ionStopsLayer]}
        />
      </ReactMapGL>
    );
  }
}

export default Map;
