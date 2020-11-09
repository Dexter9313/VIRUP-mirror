#version 150 core

in vec3 position;
in float absmag;
in vec3 color;

uniform mat4 camera;
uniform float pixelSolidAngle      = 1.0;
uniform float brightnessMultiplier = 1.0;
uniform vec3 campos;

uniform float camexp;
uniform float camdynrange;

out vec4 f_finalcolor;
out float f_pointsize;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_ClipDistance[1];
};

const float oneOverLog10 = 0.4342944819;

float log10(in float x)
{
	return log(x) * oneOverLog10;
}
float max3(vec3 v)
{
	return max(max(v.x, v.y), v.z);
}

void main()
{
	vec4 pos    = camera * vec4(position, 1.0);
	gl_Position = pos;
	gl_ClipDistance[0]
	    = (pos.z / pos.w) - 0.1; // clip galaxies too close to face in VR

	// in unit
	float camdist     = length(position - campos);
	float apparentmag = absmag + 5.0 * (log10(camdist) - 1.0);
	// lux
	// Derivation from http://stjarnhimlen.se/comp/radfaq.html#7 :
	// 1 mv=0 star outside Earth's atmosphere  = 2.54E-6 lux
	// and illuminance is an exponential of base 10^0.4 (= 100^(1/5)) of
	// magnitude by definition of magnitude (multiply by 100 each five
	// magnitudes)
	// solve 2.54E-6 = pow(10.0, 0.4*b) => b ~= -14.0
	float illuminance = pow(10.0, 0.4 * (-apparentmag - 14.0));
	// lux/sr
	float luminance = brightnessMultiplier * illuminance / pixelSolidAngle;

	vec3 col = color * luminance;

	// m := factor of how much star is over-exposed
	float m     = max3(col) * camexp / camdynrange;
	f_pointsize = sqrt(m);
	if(m > 1.0)
	{
		col /= m;
		gl_PointSize = min(
		    32.0,
		    sqrt(m) * 33.55263157894737
		        / 2.0); // part empirical part measured on the star texture
		f_finalcolor = vec4(col, 1.0);
	}
	else
	{
		gl_PointSize = 1.0;
		f_finalcolor = vec4(color, luminance);
	}
}
