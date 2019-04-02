#version 150 core

in vec3 position;
in vec4 color;

uniform mat4 camera;

out vec4 vertColor;

void main()
{
	vertColor = color;
	gl_Position = camera*vec4(position, 1.0);
}
