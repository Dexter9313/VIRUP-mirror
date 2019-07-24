#version 150 core

in vec3 position;

uniform mat4 camera;
uniform float alpha;
uniform mat4 view;

out float fragAlpha;

void main()
{
    gl_Position = camera*vec4(position, 1.0);

    float camdist = length(vec3(view*vec4(position, 1.0)));
    fragAlpha = alpha / (camdist*camdist);
}
