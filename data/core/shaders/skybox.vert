#version 150 core

in vec3 position;

uniform mat4 camera;

out vec3 f_texcoord;

void main()
{
    gl_Position = camera*vec4(position, 1.0);
	vec3 texcoord = vec3(position.x, position.z, -1.0 * position.y);
	texcoord.xz = -1.0*vec2(texcoord.z, texcoord.x);
	f_texcoord = texcoord;
}
