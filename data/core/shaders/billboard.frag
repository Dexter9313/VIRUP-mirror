#version 150 core

in vec2 texCoord;

out vec4 outColor;
uniform sampler2D tex;
uniform float alpha = 1.0;
uniform float exposure = 1.0;
uniform float dynamicrange = 1.0;

void main()
{
	outColor = texture(tex, texCoord);
	outColor.rgb *= outColor.a * alpha;
	outColor.rgb *= dynamicrange / exposure;
}
