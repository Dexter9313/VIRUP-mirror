#version 150 core

in vec3 position;

uniform mat4 camera;
uniform float alpha;
uniform vec3 campos;
uniform float pixelSolidAngle;

out float fragAlpha;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_ClipDistance[1];
};

void main()
{
	vec4 pos           = camera * vec4(position, 1.0);
	gl_Position        = pos;
	gl_ClipDistance[0] = (pos.z / pos.w) - 0.1;

	float camdist = length(position - campos);
	fragAlpha     = alpha / (camdist * camdist); // illum
	fragAlpha /= pixelSolidAngle;                // lum
}
