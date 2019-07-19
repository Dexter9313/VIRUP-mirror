#version 150 core

#include <planet/gentex/simplex4DNoise.glsl>

// CRATERS

float noise1(vec3 p)
{
	p = fract(p * vec3(233.42, 865.32, 1234.0));
	p += dot(p, p + 32.33);
	return fract(p.x * p.y * p.z);
}

vec2 noise2(vec3 p)
{
	float n = noise1(p);
	return vec2(n, noise1(p + n));
}

vec3 noise3(vec3 p)
{
	float n = noise1(p);
	return vec3(n, noise2(p + n));
}

// get a random position for cellId id
// position is within the cube [-0.4;0.4]^3
vec3 getPos(vec3 id)
{
	float seed = 3.0;

	vec3 noise = noise3(id / 1000.0);

	float x = sin(noise.x * 100000.0 * seed + 2348.0);
	float y = cos(noise.y * 100000.0 * seed + 2424.0);
	float z = cos(noise.z * 100000.0 * seed + 1243.0);

	return vec3(x, y, z) * min(0.4, noise1(id));
}

// Returns crater height (from -1 to 1) at distance "len" from center
float crater(in float len)
{
	float pi = 3.1415926535;
	float x  = clamp(pow(len, 4.0) * 8.0, pi * 0.5, pi * 3.5 / 2.0);
	float t  = clamp(len, 0.0, 1.0);
	// return ((sin(-x) + 0.5 - 0.5 * cos(t * pi)) / 3.0 + 0.33333333) * 2.0
	// - 1.0;
	return 0.666666666 * sin(-x) - 0.333333333 * cos(t * pi);
}

float craterHeight(in vec3 pos, in float size)
{
	float foo = 540.31;

	vec3 mpos = pos * size + foo;

	vec3 gridUv = fract(mpos) - .5;
	vec3 cellId = floor(mpos);

	vec3 p    = getPos(cellId);
	float len = length(gridUv - p) * 10.0;

	return crater(len);
}

// Generates the full inverse height map (1.0 lowest and 0.0 highest)
float heightmap(in vec3 pos)
{
	float height = 0.0;
	int iter     = 6;

	for(int i = 0; i < iter; ++i)
	{
		height += (craterHeight(pos, 4.0 * sqrt(float(i + 1))))
		          * pow(1.0, -1.0 * float(i));
	}
	height /= float(iter);

	return height * 0.5 + 0.5;
}

in vec3 f_position;

uniform float seed;
uniform vec3 color;
uniform float polarLatitude;

out vec4 outColor;

void main()
{
	float polarLat = polarLatitude;

	if(polarLat < 90.0 && polarLat > 0.0)
		polarLat
		    += (noise(vec4(f_position, 1.0 + seed), 24, 1, 0.8) * 10.0)
		       * sqrt((90.0 - polarLat));

	polarLat *= 3.1415 / 180.0;

	float snowProportion = 0.0;

	if(abs(f_position.z) < 1.0 - cos(polarLat))
		snowProportion = 0.0;
	else
		snowProportion = 1.0;

	snowProportion
	    = 1.0
	      - clamp(15000.0 * (1.0 - cos(polarLat) - abs(f_position.z)) + 0.5,
	              0.0, 1.0);

	outColor = vec4(vec3(heightmap(f_position)) * color, 1.0);

	outColor = mix(outColor, vec4(1.0), snowProportion);
}
