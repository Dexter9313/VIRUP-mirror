#version 150 core

in vec3 position;

uniform mat4 camera;

out vec3 f_texcoord;

void main()
{
	gl_Position = camera * vec4(position, 1.0);
	f_texcoord  = position;
}
