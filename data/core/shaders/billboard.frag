#version 150 core

in vec2 texCoord;

out vec4 outColor;
uniform sampler2D tex;
uniform float alpha = 1.0;

void main()
{
	outColor = texture(tex, texCoord);
	outColor.rgb *= outColor.a * alpha;
}
