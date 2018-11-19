uniform float opacity;

#include <common>
#include <packing>
#include <bsdfs>
#include <lights_pars>
#include <shadowmap_pars_fragment>
#include <shadowmask_pars_fragment>

out vec4 fragColor;

void main() {

	fragColor = vec4( 0.0, 0.0, 0.0, opacity * ( 1.0 - getShadowMask() ) );

}
