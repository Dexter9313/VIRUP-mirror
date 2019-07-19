#version 150 core

in vec3 f_position;
in mat4 f_invrot;

uniform samplerCube diff;
uniform sampler2D texRing;
#ifdef NORMAL
uniform samplerCube norm;
#endif

uniform float innerRing;
uniform float outerRing;

uniform float atmosphere = 0.0;

uniform vec3 oblateness = vec3(1.0, 1.0, 1.0);
uniform vec3 lightpos;
uniform float lightradius;
uniform vec3 lightcolor;
uniform vec4 neighborsPosRadius[5];
uniform vec3 neighborsOblateness[5];

// only if custom mesh would it be -1, -1
uniform vec2 flipCoords = vec2(1.0, 1.0);

out vec4 outColor;

#include <planet/getNeighborOcclusionFactor.glsl>

void main()
{
	vec3 pos           = normalize(f_position) * oblateness;
	vec3 norm_pos      = normalize(normalize(f_position) / oblateness);
	vec3 posRelToLight = pos - (f_invrot * vec4(lightpos, 1.0)).xyz;

	vec3 lightdir = -1.0 * normalize(posRelToLight);

	// see if flip pos or f_position
	vec4 diffuse
	    = texture(diff, vec3(flipCoords * f_position.xy, f_position.z));
#ifdef NORMAL
	vec3 normal
	    = texture(norm, vec3(flipCoords * f_position.xy, f_position.z)).xyz;
	normal = normalize(normal * 2.0 - 1.0); // from [0;1] to [-1;1]
	normal.xy *= flipCoords;

	// 0 or 1
	// avoids lighting stuff behind the planet
	float coeff_pos = dot(lightdir, norm_pos);
	float coeff     = max(0.0, dot(lightdir, normal));
#else
	float coeff_pos = 1.0;
	float coeff     = max(0.0, dot(lightdir, norm_pos));
#endif

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

	// RINGS SHADOW
	float coeffRings = 1.0;
	if(outerRing > 0.0 && sign(lightdir.z) != sign(pos.z))
	{
		vec3 pointOnRings = pos + lightdir * abs(pos.z / lightdir.z);
		float alt         = length(pointOnRings);
		if(alt >= innerRing && alt <= outerRing)
		{
			float texCoord = (alt - innerRing) / (outerRing - innerRing);
			coeffRings     = 1.0 - texture(texRing, vec2(texCoord, 0.5)).a;
		}
	}
	// END RINGS SHADOW

	outColor = diffuse;
	outColor.rgb *= coeffRings * coeff * globalCoeffNeighbor
	                * min(1.0, max(0.0, coeff_pos + 0.1) * 10.0);

	// Fully empirical eye-balled algorithm, FAR from accurate, no actual
	// scattering computed at all...
	// Fast as heck though
	const float minR = 0.45;
	const float maxR = 1.0;

	const float minG = 0.45;
	const float maxG = 1.0;

	const float minB = 0.45;
	const float maxB = 1.0;

	coeff_pos = dot(lightdir, norm_pos);
	float atmosColorGrad = coeff_pos * 0.5 + 0.5;
	float atmoscoeff     = clamp(atmosphere * (1.0 - coeff_pos), 0.0, 1.0);
	vec3 atmosCol        = vec3(0.0);
	if(atmosColorGrad > minR && atmosColorGrad < maxR)
	{
		atmosCol.r
		    = 0.5
		          * pow(-atmosColorGrad / (maxR - minR) + maxR / (maxR - minR),
		                15.0)
		          * 0.7
		      + 0.3;
	}
	if(atmosColorGrad > minG && atmosColorGrad < maxG)
	{
		atmosCol.g
		    = 0.3
		          * pow(-atmosColorGrad / (maxG - minG) + maxG / (maxG - minG),
		                1.0)
		          * 0.53
		      + 0.47;
	}
	if(atmosColorGrad > minB && atmosColorGrad < maxB)
	{
		atmosCol.b = 1.0 + atmosColorGrad / (maxB - minB)
		             - maxB / (maxB - minB) * 0.8 + 0.2;
	}

	atmosCol *= globalCoeffNeighbor;
	outColor.rgb = mix(outColor.rgb, atmosCol, atmoscoeff);

	outColor.rgb *= lightcolor;
}
