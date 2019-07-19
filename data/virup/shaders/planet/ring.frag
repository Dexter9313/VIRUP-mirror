#version 150 core

in vec2 f_position;
in mat4 f_invrot;

uniform sampler2D tex;

uniform float inner;
uniform float outer;
uniform float planetradius;
uniform vec3 planetoblateness = vec3(1.0, 1.0, 1.0);
uniform vec3 lightpos;
uniform float lightradius;
uniform vec3 lightcolor;
uniform vec4 neighborsPosRadius[5];
uniform vec3 neighborsOblateness[5];

out vec4 outColor;

#include <planet/getNeighborOcclusionFactor.glsl>

void main()
{
	// make perfect circles
	if(length(f_position) < inner || length(f_position) > outer)
		discard;

	float alt      = length(f_position);
	float texCoord = (alt - inner) / (outer - inner);

	vec3 pos           = vec3(f_position, 0.0);
	vec3 posRelToLight = pos - (f_invrot * vec4(lightpos, 1.0)).xyz;

	vec3 lightdir = -1.0 * normalize(posRelToLight);

	vec3 closestPoint = dot(lightdir, -1 * pos) * lightdir + pos;

	closestPoint /= planetoblateness;

	float coeff = 1.0;
	if(dot(lightdir, pos) < 0.0)
	{
		coeff -= getNeighborOcclusionFactor(lightradius * length(pos)
		                                        / length(posRelToLight),
		                                    planetradius, length(closestPoint));
	}

	// NEIGHBORS
	float globalCoeffNeighbor = 1.0;
	for(int i = 0; i < 5; ++i)
	{
		vec3 posRelToNeighbor
		    = pos - (f_invrot * vec4(neighborsPosRadius[i].xyz, 1.0)).xyz;

		if(dot(lightdir, posRelToNeighbor) >= 0.0)
		{
			continue;
		}

		float neighborRadius = neighborsPosRadius[i].w;

		vec3 closestPoint = dot(lightdir, -1 * posRelToNeighbor) * lightdir
		                    + posRelToNeighbor;

		closestPoint /= neighborsOblateness[i];

		globalCoeffNeighbor *= (1.0
		                        - getNeighborOcclusionFactor(
		                              lightradius * length(posRelToNeighbor)
		                                  / length(posRelToLight),
		                              neighborRadius, length(closestPoint)));
	}
	// END NEIGHBORS

	outColor = texture(tex, vec2(texCoord, 0.5));
	outColor.rgb *= coeff * globalCoeffNeighbor;

	outColor.rgb *= lightcolor;
}
