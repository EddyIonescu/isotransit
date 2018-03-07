import _ from 'lodash';
import React, { Component } from 'react';
import ReactMapGL, { NavigationControl } from 'react-map-gl';
import DeckGL, { PolygonLayer } from 'deck.gl';

import { MAP_STYLE, MAPBOX_ACCESS_TOKEN } from '../keysConfig.json';
import "./map.css";

class Map extends Component {
  constructor() {
    super();
    this.state = {
      // Viewport settings that is shared between mapbox and deck.gl
      viewport: {
        width: window.innerWidth,
        height: window.innerHeight,
        longitude: -80.5418298,
        latitude: 43.4684405,
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
    let { layers } = this.props;
    // flatten once as it contains multiple isochrones of the same colour
    layers = _.flatten(layers);
    // put each shape under coordinates into a list
    layers = layers.map(layer => ([layer.coordinates]));
    // reverse lat,lng to lng,lat
    // lat,lng - geography/transportation/science - lng,lat - CS
    layers = layers.map(layer => layer.map(polygons => polygons.map(coords => coords.map(coord => ([coord[1], coord[0]])))));
    console.log(layers);
    return (
      <ReactMapGL
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
          layers={layers.length ? [new PolygonLayer({
              id: "polygon-layer",
              data: layers.map(polygon => ({
                polygon: polygon[0][0],
                fillColor: [12, 250, 140],
              })),
              filled: true,
              stroked: false,
            })] : []}
        />
      </ReactMapGL>
    );
  }
}

export default Map;
