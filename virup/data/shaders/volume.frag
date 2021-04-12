#version 150 core

uniform vec3 color;
uniform sampler3D volumetex;

uniform mat4 occltransform;
uniform float useOccl;
uniform sampler3D occltex;

in vec3 f_position;
in vec3 f_campos;

out vec4 outColor;

#include <raymarch.glsl>

float density(vec3 pos, sampler3D volumetex)
{
	vec3 coord = pos.zyx;
	if(min(coord, vec3(1.0)) != coord || max(coord, vec3(0.0, 0.0, 0.0)) != coord)
	{
		return 0.0;
	}
	float res = texture(volumetex, coord).x;
	//res *= step(5.0, res);
	//res -= 5.0;
	return max(0.0, res);
}

vec4 integrate(vec3 from, vec3 to, sampler3D volumetex, sampler3D occltex)
{
	const int SAMPLES = 600;
	vec3 ds = (to - from) / float(SAMPLES);
	float lds = length(ds);
	float lds2 = lds * length(vec3(48.0, 48.0, 12.0)); // /!\ HARDCODED
	vec3 v  = from + ds * 0.5;

	vec4 result = vec4(0.0);
	float NHval = 0.0;
	for(int i = 0; i < SAMPLES; i++)
	{
		float d = lds * density(v, volumetex);
		NHval += lds2 * density(v, occltex);
		result.a += d;
		result.rgb += d * att(NHval);
		v += ds;
	}

	return result;
}

void main()
{
	vec4 i = integrate(f_campos + vec3(0.5), f_position + vec3(0.5), volumetex, occltex);
	//vec4 i = integrate(f_campos, f_position, volumetex, occltex);

	//float fragAlpha   = i.a;
	//fragAlpha = density(vec3(f_position.xy, 0.0) + vec3(0.5), volumetex) / 40.0;

	outColor = vec4(color * i.rgb * 4.0e26, 1.0); // 1e14 hardcoded
	//outColor = vec4(10000.0 * color, 1.0);
}
// 255 0 98
