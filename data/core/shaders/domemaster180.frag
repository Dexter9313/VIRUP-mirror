#version 150 core

in vec2 texCoord;

out vec4 outColor;

uniform samplerCube tex;

void main()
{
	const float pi = 3.14159265359;

	vec2 pos2D  = 2.0 * texCoord - 1.0;

	if(length(pos2D) > 1.0)
		discard;

	vec2 lonlat = vec2(atan(pos2D.x, -pos2D.y), (1.0 - length(pos2D)) * pi / 2.0);

	/*vec3 pos = vec3(cos(lonlat.x) * cos(lonlat.y),
	                sin(lonlat.x) * cos(lonlat.y), sin(lonlat.y));*/
	vec3 pos = vec3(sin(lonlat.y),
	                sin(lonlat.x) * cos(lonlat.y), -cos(lonlat.x) * cos(lonlat.y));

	vec3 pos2 = pos.yzx;
	pos2.z *= -1;

	outColor = texture(tex, pos2);
}
