#version 150 core

in vec4 f_finalcolor;
in float f_pointsize;
in vec2 f_texTile;
in float f_dist;

uniform sampler2D tex;
uniform vec2 atlassize;
uniform float colormix = 0.0;

out vec4 outColor;

vec3 hsl2rgb( in vec3 c )
{
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

    return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}

void main()
{
	if(f_pointsize < 0.0)
		discard;

	vec4 texcol = vec4(1.0);
	float colmix = colormix;
	if(f_pointsize > 1.0)
	{
		vec2 texCoords = f_texTile + gl_PointCoord;
		texcol = texture(tex, texCoords / atlassize);

		texcol.a *= f_finalcolor.a * 5.0; // empirical
		colmix = 0.0;

		// enhance contrast
		texcol.rgb = clamp(1.5*(texcol.rgb - 0.5) + 0.5, vec3(0.0), vec3(1.0));
	}

	outColor = texcol;
	if(f_pointsize <= 1.0)
	{
		outColor *= f_finalcolor;
	}
	outColor.rgb = mix(outColor.rgb, hsl2rgb(vec3(max(0.0, 0.75 - (1.1*f_dist / 8836.0)), 1.0, 0.5)), colormix);
	outColor.rgb *= outColor.a; // multiply color by luminance
	outColor.a = 1.0;
}
