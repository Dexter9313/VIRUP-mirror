#version 150 core

in vec2 position;

void main()
{
    gl_Position = vec4(position*2.0-1.0, -1.0, 1.0);
}
