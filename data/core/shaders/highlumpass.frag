#version 150

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex0;

float max3(vec3 v)
{
	return max(max(v.x, v.y), v.z);
}

void main()
{
	outColor = texture2D(tex0, texCoord);
	outColor.rgb *= smoothstep(0.5,1.0, max3(outColor.rgb));
	outColor.rgb = min(vec3(2.0), outColor.rgb);
}
