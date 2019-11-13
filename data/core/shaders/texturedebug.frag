#version 150

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D debuggedTex;

void main()
{
	vec4 c = texture(debuggedTex, texCoord);
	outColor = vec4(vec3(c.r), 1.0);
}
