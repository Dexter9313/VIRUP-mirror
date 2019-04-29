#version 150 core

uniform samplerCube tex;

in vec3 f_texcoord;

out vec4 outColor;

void main()
{
	outColor = texture(tex, f_texcoord);
}
