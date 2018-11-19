uniform sampler2D tEquirect;

in vec3 vWorldPosition;

#include <common>

out vec4 fragColor;

void main() {

	vec3 direction = normalize( vWorldPosition );

	vec2 sampleUV;

	sampleUV.y = asin( clamp( direction.y, - 1.0, 1.0 ) ) * RECIPROCAL_PI + 0.5;

	sampleUV.x = atan( direction.z, direction.x ) * RECIPROCAL_PI2 + 0.5;

	fragColor = texture( tEquirect, sampleUV );

}
