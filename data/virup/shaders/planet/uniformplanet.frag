#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform vec3 oblateness = vec3(1.0, 1.0, 1.0);
uniform vec3 color;
uniform vec3 lightpos;
uniform float lightradius;
uniform vec3 lightcolor;
uniform vec4 neighborsPosRadius[5];
uniform vec3 neighborsOblateness[5];

out vec4 outColor;

#include <planet/getNeighborOcclusionFactor.glsl>

void main()
{
	vec3 pos           = normalize(f_position) * oblateness;
	vec3 norm          = normalize(normalize(f_position) / oblateness);
	vec3 posRelToLight = pos - (f_invrot * vec4(lightpos, 1.0)).xyz;

	vec3 lightdir = -1.0 * normalize(posRelToLight);

	float coeff = max(0.0, dot(lightdir, norm));

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

	outColor = vec4(color, 1.0);
	outColor.rgb *= coeff * globalCoeffNeighbor;

	outColor.rgb *= lightcolor;
}
