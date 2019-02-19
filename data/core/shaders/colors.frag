#version 150 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D tex;

uniform float gamma;
uniform float hdr;

void main()
{
    vec4 result = texture(tex, texCoord);
    // reinhard tone mapping
    if(hdr == 1.0)
    {
        result.rgb = result.rgb / (result.rgb + vec3(1.0));
    }
    // gamma correction
    result.rgb = pow(result.rgb, vec3(1.0 / gamma));
    outColor = result;
}
