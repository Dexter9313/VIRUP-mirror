#version 150 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D tex;

uniform float lum;

void main()
{
    vec4 result = texture(tex, texCoord);
    float color = result.r*0.3 + result.g*0.59 + result.b*0.11;
    outColor = vec4(vec3(lum*color), 1.0);
}
