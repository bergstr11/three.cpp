#ifdef USE_MAP

	vec4 texelColor = texture( map, vUv );

	texelColor = mapTexelToLinear( texelColor );
	diffuseColor *= texelColor;

#endif
