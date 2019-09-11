#version 150

// https://www.geeks3d.com/20140213/glsl-shader-library-fish-eye-and-dome-and-barrel-distortion-post-processing-filters/2/

in vec2 texCoord;

out vec4 outColor;

const float PI = 3.1415926535;

uniform float aspectRatio;
uniform float lenseSize;
uniform vec4 lenseScreenCoord;
uniform float lenseDist;
uniform float radiusLimit;

uniform sampler2D tex0;
uniform sampler2D distortionMap;

vec2 Distort(vec2 p)
{
	vec4 highpval = texture2D(distortionMap, p * 0.5 + 0.5);
	float x = highpval.r + (highpval.b / 255.0);
	float y = highpval.g + (highpval.a / 255.0);
	return vec2(x, y) * 2.0 - 1.0;
}

void main()
{
	vec2 center = lenseScreenCoord.xy * 0.5 + vec2(0.5);
	vec2 xy     = 2.0 * (texCoord - center + vec2(0.5)) - 1.0;
	xy.y /= aspectRatio;
	xy *= lenseDist / lenseSize;

	vec2 uv;
	float d = length(xy);
	if(d <= 0.025)
	{
		outColor.rgb = vec3(0.0);
		outColor.a = 1.0;
		return;
	}
	if(xy.x > -1 && xy.x < 1 && xy.y > -1 && xy.y < 1
	   && lenseScreenCoord.z > -1.0 && lenseScreenCoord.z < 1.0)
	{
		uv = Distort(xy);
		uv /= lenseDist / lenseSize;
		uv.y *= aspectRatio;
		uv = (uv + 1.0) / 2.0 - vec2(0.5) + center;

		if(d > radiusLimit)
		{
			float a = (d - radiusLimit) / (1 - radiusLimit);
			uv = mix(uv, texCoord, clamp(a, 0.0, 1.0));
		}
	}
	else
	{
		uv = texCoord;
	}
	vec4 c = texture2D(tex0, uv);
	outColor = c;
}
