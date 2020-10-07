#version 330 core

// https://learnopengl.com/Advanced-Lighting/Bloom

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex0;

uniform float horizontal;
uniform float weight[5]
    = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 tex_offset = 1.0 / textureSize(tex0, 0); // gets size of single texel
	vec3 result     = texture(tex0, texCoord).rgb
	              * weight[0]; // current fragment's contribution
	if(horizontal != 0.0)
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(tex0, texCoord + vec2(tex_offset.x * i, 0.0)).rgb
			          * weight[i];
			result += texture(tex0, texCoord - vec2(tex_offset.x * i, 0.0)).rgb
			          * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(tex0, texCoord + vec2(0.0, tex_offset.y * i)).rgb
			          * weight[i];
			result += texture(tex0, texCoord - vec2(0.0, tex_offset.y * i)).rgb
			          * weight[i];
		}
	}
	outColor = vec4(result, 1.0);
}
