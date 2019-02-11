#version 150 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D tex;

uniform float gamma;

void main()
{
	vec4 result = texture(tex, texCoord);
	result.rgb = pow(result.rgb, vec3(1.0 / gamma));
	outColor = result;
}
