#version 150 core

uniform vec3 color;
uniform float alpha;
out vec4 outColor;

void main()
{
	outColor = vec4(color, alpha);
}
