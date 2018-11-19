uniform samplerCube tCube;
uniform float tFlip;
uniform float opacity;

in vec3 vWorldPosition;
out vec4 fragColor;

void main() {

	fragColor = texture( tCube, vec3( tFlip * vWorldPosition.x, vWorldPosition.yz ) );
	fragColor.a *= opacity;

}
