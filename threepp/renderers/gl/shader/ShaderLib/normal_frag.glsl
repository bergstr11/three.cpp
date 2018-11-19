#define NORMAL

uniform float opacity;

#if defined( FLAT_SHADED ) || defined( USE_BUMPMAP ) || defined( USE_NORMALMAP )

	in vec3 vViewPosition;

#endif

#ifndef FLAT_SHADED

	in vec3 vNormal;

#endif

#include <packing>
#include <uv_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <logdepthbuf_pars_fragment>

out vec4 fragColor;

void main() {

	#include <logdepthbuf_fragment>
	#include <normal_fragment>

	fragColor = vec4( packNormalToRGB( normal ), opacity );

}
