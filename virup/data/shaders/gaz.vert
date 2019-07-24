#version 150 core

in vec3 position;
in float radius;
in float luminosity;

uniform mat4 camera;
uniform float alpha;
uniform mat4 view;
uniform float scale;

out float fragAlpha;

void main()
{
	gl_Position = camera * vec4(position, 1.0);

	float camdist = length(vec3(view * vec4(position, 1.0)));
	fragAlpha     = min(10.0, luminosity * alpha / (radius * radius * camdist * camdist));
	gl_PointSize = 1500.0*scale*radius/camdist;
}
