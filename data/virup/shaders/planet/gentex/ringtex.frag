#version 150 core

#include <planet/gentex/simplex4DNoise.glsl>

in vec2 f_position;

uniform vec3 color;
uniform float seed;

out vec4 outColor;

void main()
{
	float alt = f_position.x;

	float colorBands
	    = (noise(vec4(1.0, 1.0, alt, 1.0 + seed), 24, 1, 0.8) + 1) * 0.5;
	float dc = noise(vec4(1.0, 1.0, alt, 2.0 + seed), 24, 1, 0.8);

	outColor = vec4(color + vec3(dc * 0.8), 1.0);
	outColor *= pow(colorBands, 15.0);
	outColor *= 200.0;
	outColor = clamp(outColor, vec4(0.0), vec4(1.0));
}
