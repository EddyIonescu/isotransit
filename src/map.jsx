import React, { Component } from 'react';
import ReactMapGL, { NavigationControl, Popup } from 'react-map-gl';
import DeckGL from 'deck.gl';
import propTypes from 'prop-types';

import { MAP_STYLE, MAPBOX_ACCESS_TOKEN } from './config.json';

class Map extends Component {
  constructor() {
    super();
    this.state = {
      // Viewport settings that is shared between mapbox and deck.gl
      viewport: {
        width: (2 * window.innerWidth) / 3,
        height: window.innerHeight,
        longitude: -122.41669,
        latitude: 37.7853,
        zoom: 15,
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
    this.resizeMap();
    window.addEventListener('resize', this.resizeMap.bind(this));
  }

  componentWillUnmount() {
    window.removeEventListener('resize', this.resizeMap.bind(this));
  }

  resizeMap() {
    this.setState({
      viewport: {
        width: window.innerWidth,
        height: window.innerHeight,
        longitude: -122.41669,
        latitude: 37.7853,
        zoom: 15,
        pitch: 0,
        bearing: 0,
      },
    });
  }
  
  render() {
    console.log("rendering");
    const onViewportChange = viewport => this.setState({ viewport });
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
          layers={this.props.layers}
        />
      </ReactMapGL>
    );
  }
}

Map.propTypes = {
  layers: propTypes.arrayOf(propTypes.object),
};

module.exports = Map;
