#version 150

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D highlumtex;

float max3(vec3 v)
{
	return max(max(v.x, v.y), v.z);
}

void main()
{
	// not bloom for now, only conserves hue
	outColor = texture2D(tex0, texCoord);
	outColor.rgb += texture2D(highlumtex, texCoord).rgb;
	float m  = max3(outColor.rgb);
	if(m > 1.0)
	{
		outColor.rgb /= m;
	}
}
