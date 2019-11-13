#version 150 core

uniform vec3 color;
uniform float alpha = 1.0;
out vec4 outColor;

void main()
{
	outColor = vec4(color, alpha);
}
