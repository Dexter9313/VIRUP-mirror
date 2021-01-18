#version 150 core

in vec3 position;
in vec3 color; // in solar luminosity !

uniform mat4 camera;
uniform float alpha;
uniform vec3 campos;
uniform float pixelSolidAngle;


out vec3 f_color;

const float oneOverLog10 = 0.4342944819;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_ClipDistance[1];
};

float log10(in float x)
{
	return log(x) * oneOverLog10;
}

vec3 log10_3(in vec3 x)
{
	return log(x) * oneOverLog10;
}

void main()
{
	vec4 pos           = camera * vec4(position, 1.0);
	gl_Position        = pos;
	gl_ClipDistance[0] = (pos.z / pos.w) - 0.1;

	float camdist = length(position - campos);
	vec3 absmag = 4.83 - 2.5 * log10_3(color); // color is in Solar Luminosity ;
	                                           // sun is 4.83 abs mag
	vec3 apparentmag = absmag + 5.0 * (log10(camdist) + 2.0);
	vec3 irradiance  = pow(vec3(10.0), 0.4 * (-apparentmag - 14.0));
	vec3 luminance   = irradiance / pixelSolidAngle; // lum

	vec3 a = vec3(1.0);

	f_color = a * alpha * luminance;
}
