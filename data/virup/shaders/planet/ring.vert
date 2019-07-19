#version 150 core

in vec2 position;

uniform mat4 camera;

uniform mat4 properRotation
    = mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0),
           vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0));

out vec2 f_position;
out mat4 f_invrot;

void main()
{
	f_position = position;

	f_invrot = transpose(properRotation);

	gl_Position = camera * properRotation * vec4(position, 0.0, 1.0);
}
