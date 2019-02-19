#version 150 core

uniform vec3 color;
uniform sampler2D tex;

in vec3 f_normal;
in vec2 f_texcoord;

out vec4 outColor;

void main()
{
	outColor = texture(tex, f_texcoord);
}
