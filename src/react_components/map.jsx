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
    const onViewportChange = viewport => {
      this.setState({ viewport });
    };
    const { viewport } = this.state;
    let { selectedLocation, layers } = this.props;
    // flatten once as it contains multiple isochrones of the same colour
    layers = _.flatten(layers);
    // put each shape under coordinates into a list
    layers = layers.map(layer => ([layer.coordinates]));
    // reverse lat,lng to lng,lat
    // lat,lng - geography/transportation/science - lng,lat - CS
    layers = layers.map(layer => layer.map(polygons => polygons.map(coords => coords.map(coord => ([coord[1], coord[0]])))));
    console.log(layers);

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
        longitude={selectedLocation.lng}
        latitude={selectedLocation.lat}
        {...viewport}
        mapStyle={MAP_STYLE}
        mapboxApiAccessToken={MAPBOX_ACCESS_TOKEN}
        onViewportChange={onViewportChange}
      >
        <div className="navigation-control">
          <NavigationControl onViewportChange={onViewportChange} />
        </div>
        <DeckGL
          {...viewport}
          layers={[...(layers.length ? [new PolygonLayer({
              id: "polygon-layer",
              data: layers.map(polygon => ({
                polygon: polygon[0][0],
                fillColor: [12, 200, 190],
              })),
              filled: true,
              stroked: false,
            })] : []), ionStopsLayer]}
        />
      </ReactMapGL>
    );
  }
}

export default Map;
