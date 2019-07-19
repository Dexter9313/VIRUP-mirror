#version 150 core

#include <planet/gentex/simplex4DNoise.glsl>

in vec3 f_position;

uniform vec3 color;
uniform float seed;

uniform float bandsIntensity;
uniform float stormsIntensity;

out vec4 outColor;

void main()
{
	/*float rot = time * (1.0 - abs(f_position.z));
	vec3 rotatedpos = vec3(cos(rot)*f_position.x + sin(rot)*f_position.y,
	cos(rot)*f_position.y - sin(rot) * f_position.x, f_position.z);
*/
	float n = noise(vec4(f_position, 1.0 + seed), 24, 5.0, 0.5) * 0.1
	          * bandsIntensity;

	// storms
	// Get the three threshold samples

	float s  = 0.75;
	float f  = 2.5;
	float t1 = snoise(vec4(f_position, 1.0 + seed) * f) - s;
	float t2 = snoise((vec4(f_position, 1.0 + seed) + 800.0) * f) - s;
	float t3 = snoise((vec4(f_position, 1.0 + seed) + 1600.0) * f) - s;

	// Intersect them and get rid of negatives

	float threshold = max(t1 * t2 * t3, 0.0);

	n += snoise(vec4(f_position, 1.0 + seed) * 10.0) * threshold * 10.0
	     * stormsIntensity;

	float alt = f_position.z + n;

	float colorBands = (noise(vec4(1.0, 1.0, alt, 1.0 + seed), 24, 1, 0.6) + 1)
	                       * 0.5 * bandsIntensity
	                   + 1 - bandsIntensity;

	float dr = noise(vec4(1.0, 1.0, alt, 2.0 + seed), 24, 1, 0.6);
	float dg = noise(vec4(1.0, 1.0, alt, 3.0 + seed), 24, 1, 0.6);
	float db = noise(vec4(1.0, 1.0, alt, 4.0 + seed), 24, 1, 0.6);

	/*float coeff = max(0.0, dot(normalize(lightpos), normalize(f_position)));
	float c     = coeff * colorBands;*/

	outColor = vec4(
	    color + (vec3(dr, dg, db) * 0.3 * bandsIntensity * bandsIntensity),
	    1.0);
	outColor.rgb *= colorBands;

	// outColor = vec4((f_position + vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
}
