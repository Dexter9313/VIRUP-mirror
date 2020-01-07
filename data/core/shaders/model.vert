#version 150 core

in vec3 position;
in vec3 tangent;
in vec3 normal;
in vec2 texcoord;

uniform mat4 camera;

uniform mat4 lightspace;
uniform float boundingSphereRadius;
uniform mat4 localTransform;

out vec3 f_position;
out vec3 f_tangent;
out vec3 f_normal;
out vec2 f_texcoord;
out vec4 f_lightrelpos;

#include <shadows.glsl>

void main()
{
	gl_Position = camera * vec4(position, 1.0);

	f_position    = position;
	f_tangent     = tangent;
	f_normal      = normal;
	f_texcoord    = texcoord;
	f_lightrelpos = computeLightSpacePosition(lightspace, (localTransform * vec4(position, 1.0)).xyz, normal,
	                                          boundingSphereRadius);
}
