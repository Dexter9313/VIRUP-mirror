#version 150 core

in vec3 f_position;

uniform float temperature;
uniform float time;
uniform float seed;

uniform sampler1D blackbody;
// {min, max}
uniform vec2 blackbodyBoundaries;

out vec4 outColor;

#include <planet/gentex/simplex4DNoise.glsl>

vec4 getTempColorShift(float temperature)
{
	return vec4(temperature * (0.0534 / 255.0) - (43.0 / 255.0),
	            temperature * (0.0628 / 255.0) - (77.0 / 255.0),
	            temperature * (0.0735 / 255.0) - (115.0 / 255.0), 0.0);
}

void main()
{
	float c = (noise(vec4(f_position, mod(time / 100000.0, 10000.0) / 200.0), 4,
	                 40.0, 0.7)
	           + 1.0)
	          * 0.5;

	// star radius
	float unRadius = 700000.0 * seed;
	// Get worldspace position
	vec4 sPosition = vec4(f_position, 1.0) * unRadius;

	// Sunspots
	float s         = 0.3;
	float frequency = 0.00001;
	float t1        = snoise(sPosition * frequency) - s;
	float t2        = snoise((sPosition + unRadius) * frequency) - s;
	float ss        = (max(t1, 0.0) * max(t2, 0.0)) * 2.0;

	// Accumulate total noise
	c = c - ss;

	float t  = temperature;
	outColor = (vec4(c, c, c, 1.0) /*+ getTempColorShift(t)*/)
	           * texture(blackbody,
	                     (t - blackbodyBoundaries.x)
	                         / (blackbodyBoundaries.y - blackbodyBoundaries.x));
}
