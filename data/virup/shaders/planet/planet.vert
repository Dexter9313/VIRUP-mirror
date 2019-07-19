#version 150 core

in vec3 position;

uniform mat4 camera;

uniform mat4 properRotation
    = mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0),
           vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0));

uniform vec3 oblateness = vec3(1.0, 1.0, 1.0);

out vec3 f_position;
out mat4 f_invrot;
out mat3 f_tantoworld;

void main()
{
	f_position = position;

	f_invrot = transpose(properRotation);

	vec3 transNorm  = normalize(position);
	vec3 transTan   = cross(vec3(0.0, 0.0, 1.0), transNorm);
	vec3 transBiTan = cross(transNorm, transTan);

	f_tantoworld = mat3(transTan, transBiTan, transNorm);

	gl_Position = camera * properRotation * vec4(position * oblateness, 1.0);
}
