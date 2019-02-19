#version 150 core

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform mat4 camera;

out vec3 f_normal;
out vec2 f_texcoord;

void main()
{
    gl_Position = camera*vec4(position, 1.0);
	f_normal = normal;
	f_texcoord = texcoord;
}
