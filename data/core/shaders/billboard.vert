#version 150 core

in vec2 position;

uniform mat4 camera;

out vec2 texCoord;

void main()
{
	texCoord    = position + vec2(0.5, 0.5);
	gl_Position = camera * vec4(position, 0.0, 1.0);
}
