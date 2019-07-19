#version 150 core

in vec3 position;
in vec4 color;

uniform mat4 camera;

out vec4 fragcolor;

void main()
{
    gl_Position = camera*vec4(position, 1.0);
	fragcolor = color;
}
