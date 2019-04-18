#version 150 core

uniform samplerCube tex;

in vec3 f_texcoord;

out vec4 outColor;

void main()
{
	vec3 abscoord = abs(f_texcoord);
	vec3 texcoord = f_texcoord;
	if(abscoord.y > abscoord.x && abscoord.y > abscoord.z)
	{
		texcoord.xz = vec2(-1.0*texcoord.z, texcoord.x);
	}
	outColor = texture(tex, texcoord);
}
