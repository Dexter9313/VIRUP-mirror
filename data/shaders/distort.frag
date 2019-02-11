#version 150

// https://www.geeks3d.com/20140213/glsl-shader-library-fish-eye-and-dome-and-barrel-distortion-post-processing-filters/2/

in vec2 texCoord;

out vec4 outColor;

const float PI = 3.1415926535;


uniform float BarrelPower;

uniform sampler2D tex0;

vec2 Distort(vec2 p)
{
    float theta  = atan(p.y, p.x);
    float radius = length(p);
    radius = pow(radius, BarrelPower);
    p.x = radius * cos(theta);
    p.y = radius * sin(theta);
    return 0.5 * (p + 1.0);
}

void main()
{
    vec2 xy = 2.0 * texCoord - 1.0;
    vec2 uv;
    float d = length(xy);
    if (d < 1.0)
    {
        uv = Distort(xy);
    }
    else
    {
        uv = texCoord;
    }
    vec4 c = texture2D(tex0, uv);
    outColor = c;
}
