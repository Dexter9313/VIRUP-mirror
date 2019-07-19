#version 150 core

in vec3 f_position;
in mat3 f_tantoworld;

uniform sampler2D norm;

uniform float PI = 3.1415;

out vec4 outColor;

void main()
{
	vec2 texCoord = vec2(atan(f_position.y, f_position.x) / (2.0 * PI),
	                     1.0 - (asin(f_position.z) / PI) - 0.5);

	if(texCoord.x < 0.0)
	{
		texCoord.x += 1.0;
	}

	vec3 normal = texture(norm, texCoord).xyz;
	normal      = normalize(normal * 2.0 - 1.0); // from [0;1] to [-1;1]
	normal.y *= -1.0;
	normal   = f_tantoworld * normal;
	outColor = vec4(normal * 0.5 + 0.5, 1.0);
}
