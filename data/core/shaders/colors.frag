#version 150 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D tex;

uniform float gamma;
uniform float hdr;

const int bayer_pattern[64]
    = int[](0, 32, 8, 40, 2, 34, 10, 42,    /* 8x8 Bayer ordered dithering  */
            48, 16, 56, 24, 50, 18, 58, 26, /* pattern.  Each input pixel   */
            12, 44, 4, 36, 14, 46, 6, 38,   /* is scaled to the 0..63 range */
            60, 28, 52, 20, 62, 30, 54, 22, /* before looking in this table */
            3, 35, 11, 43, 1, 33, 9, 41,    /* to determine the action.     */
            51, 19, 59, 27, 49, 17, 57, 25, 15, 47, 7, 39, 13, 45, 5, 37, 63,
            31, 55, 23, 61, 29, 53, 21);

vec4 dither()
{
	int x = int(mod(round(gl_FragCoord.x / 1.5), 8.0));
	int y = int(mod(round(gl_FragCoord.y / 1.5), 8.0));
	return vec4(bayer_pattern[x + 8 * y] / (32.0 * 256.0) - (1.0 / 128.0));
}

void main()
{
	vec4 result = texture(tex, texCoord);
	// reinhard tone mapping
	if(hdr == 1.0)
	{
		result.rgb = result.rgb / (result.rgb + vec3(1.0));
	}
	result.rgb = pow(result.rgb, vec3(1.0 / gamma));
	// dithering to remove banding
	result += dither();
	outColor = result;
}
