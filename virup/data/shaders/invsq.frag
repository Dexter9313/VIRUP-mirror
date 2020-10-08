#version 150 core

uniform vec3 color;

in float fragAlpha;

out vec4 outColor;

void main()
{
	outColor = vec4(color * fragAlpha, 1.0);
}
