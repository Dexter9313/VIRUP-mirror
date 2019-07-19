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

float clampDerivative(in float x, in float a, in float b)
{
	if(x <= a || x >= b)
	{
		return 0.0;
	}
	return 1.0;
}

// Derivative of the crater height function (see terrestrialdiff)
float craterDerivative(in float len)
{
	float pi = 3.1415926535;
	float x  = clamp(pow(len, 4.0) * 8.0, pi * 0.5, pi * 3.5 / 2.0);
	float dxdl
	    = 32.0 * pow(len, 3.0)
	      * clampDerivative(pow(len, 4.0) * 8.0, pi * 0.5, pi * 3.5 / 2.0);
	float t    = clamp(len, 0.0, 1.0);
	float dtdl = clampDerivative(len, 0.0, 1.0);

	// derivative of radius over t
	// return (-32.0*pow(t,3.0)*cos(x) + 1.5708 * sin(pi*t));
	return -0.666666666 * dxdl * cos(x) + pi * dtdl * sin(pi * t) / 3.0;
	// return 0.0;
}

vec3 craterNormal(in vec3 pos, in float size)
{
	float pi  = 3.1415926535;
	float foo = 540.31;

	vec3 mpos = pos * size + foo;

	vec3 gridUv = fract(mpos) - .5;
	vec3 cellId = floor(mpos);

	vec3 p        = getPos(cellId);
	vec3 pworld   = normalize(p + 0.5 + floor(mpos) - foo);
	vec3 posworld = normalize(pos);

	float len = length(gridUv - p) * 10.0;

	// https://en.wikipedia.org/wiki/Azimuth#Calculating_azimuth
	float phi1 = sin(pworld.z);
	float phi2 = sin(posworld.z);
	float L    = atan(pworld.y, pworld.x) - atan(posworld.y, posworld.x);

	float angle = atan(sin(L), (cos(phi1) * tan(phi2) - sin(phi1) * cos(L)));
	angle       = pi / 2.0 - angle;

	// derivative of radius over t
	float drdt = craterDerivative(len) * noise1(cellId);

	vec3 tangent = normalize(vec3(1.0, 0.0, drdt));

	vec3 normal = cross(tangent, vec3(0.0, 1.0, 0.0));

	normal.xy
	    = mat2(vec2(cos(angle), sin(angle)), vec2(-sin(angle), cos(angle)))
	      * normal.xy;

	return normal;
}

vec3 normalmap(in vec3 pos)
{
	vec3 normal = vec3(0.0, 0.0, 1.0);

	int iter = 100;
	for(int i = 0; i < iter; ++i)
	{
		vec3 curNorm = craterNormal(
		    pos, 4.0 * sqrt(float(i + 1))); // * pow(1.0, -1.0*float(i));
		normal = mix(curNorm, normal, curNorm.z * 0.5 + 0.5);
	}
	// normal /= float(iter);

	return normalize(normal);
}

in vec3 f_position;
in mat3 f_tantoworld;

uniform vec3 color;
uniform float seed;
uniform float polarLatitude;

out vec4 outColor;

void main()
{
	float nx = noise(vec4(f_position, 2000.0 + seed), 24, 10.0, 0.9);
	float ny = noise(vec4(f_position, 3000.0 + seed), 24, 10.0, 0.9);
	float nz = noise(vec4(f_position, 4000.0 + seed), 24, 10.0, 0.9);

	vec3 mountainishNormal
	    = normalize(vec3(nx * 0.3, ny * 0.3, nz * 0.5 + 0.5));
	vec3 normal = normalmap(f_position);
	normal.x *= -1.0;

	normal = f_tantoworld * (mountainishNormal * 0.55 + normal * 0.45);

	outColor.xyz = normal*0.5 + vec3(0.5);
	outColor.a = 1.0;
}

