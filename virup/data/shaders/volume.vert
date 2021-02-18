#version 150 core

in vec3 position;

uniform mat4 camera;
uniform vec3 campos;

out vec3 f_position;
out vec3 f_campos;

/*out gl_PerVertex
{
    vec4 gl_Position;
    float gl_ClipDistance[1];
};*/

void main()
{
	vec4 pos    = camera * vec4(position, 1.0);
	gl_Position = vec4(pos.xy, 0.0, pos.w);

	// gl_ClipDistance[0] = (pos.z / pos.w) - 0.1;

	f_position = position;
	f_campos   = campos;
}
