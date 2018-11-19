#ifdef USE_DISPLACEMENTMAP

	transformed += normalize( objectNormal ) * ( texture( displacementMap, uv ).x * displacementScale + displacementBias );

#endif
