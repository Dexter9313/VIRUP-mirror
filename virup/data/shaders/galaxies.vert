#version 150 core

in vec3 position;
in float absmag;
in vec3 color;

uniform mat4 camera;
uniform float pixelSolidAngle      = 1.0;
uniform float brightnessMultiplier = 1.0;
uniform vec3 campos;

uniform vec2 atlassize;

out vec4 f_finalcolor;
out float f_pointsize;
out vec2 f_texTile;
out float f_dist;

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
	f_dist = length(position);   // dist to earth in unit
	if(f_dist < 2.0)             // remove galaxies too close to the LG
	{
		f_pointsize  = -1.0;
		gl_PointSize = 1.0;
		f_finalcolor = vec4(0.0);
		return;
	}

	// in unit
	float camdist = length(position - campos);

	// linear size model
	float coeff1 = 0.03; // 3Mpc
	float coeff2 = 0.13; // 175Mpc
	float a      = (coeff2 - coeff1) / (175.0 - 3.0);
	float b      = coeff2 - 175.0 * a;
	float size   = f_dist * a + b;

	float apparentSize = atan(size / camdist);

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

	float m     = apparentSize * apparentSize;
	f_pointsize = apparentSize * 0.5 / sqrt(pixelSolidAngle);
	if(f_pointsize > 1.0)
	{
		col /= m;
		gl_PointSize = apparentSize * 5.0
		               / sqrt(pixelSolidAngle); // 5.0 = empirical and depends
		                                        // on texture content
		f_finalcolor = vec4(col, brightnessMultiplier * illuminance / m);

		int id = gl_VertexID;
		id     = id % int(atlassize.x * atlassize.y);
		f_texTile
		    = vec2(int(id % int(atlassize.x)), int(id / int(atlassize.x)));
	}
	else
	{
		gl_PointSize = 1.0;
		f_finalcolor = vec4(color, luminance);
	}
}
