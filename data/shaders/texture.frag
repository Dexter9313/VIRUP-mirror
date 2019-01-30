#version 150 core

uniform vec3 color;
uniform float alpha;

out vec4 outColor;
uniform sampler2D tex;

void main()
{
	outColor = texture(tex, gl_PointCoord)*vec4(color,alpha);
}
