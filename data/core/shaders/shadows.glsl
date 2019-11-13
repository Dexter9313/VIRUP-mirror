vec4 computeLightSpacePosition(mat4 lightspace, vec3 position, vec3 normal,
                               float boundingSphereRadius)
{
	return lightspace
	       * vec4(position + 0.0014 * normal * boundingSphereRadius, 1.0);
}

float computeShadow(vec4 lightspacepos, sampler2DShadow shadowmap)
{
#ifdef SMOOTHSHADOWS
	const int samples    = SMOOTHSHADOWS;
	const float diskSize = 3.0;
	const float factor   = 3.0 * diskSize / samples;
	vec2 texelSize       = factor / textureSize(shadowmap, 0);

	vec3 projCoords = lightspacepos.xyz / lightspacepos.w;

	float shadow = 0.0;
	for(int x = -1 * (samples / 2); x <= (samples / 2); ++x)
	{
		for(int y = -1 * (samples / 2); y <= (samples / 2); ++y)
		{
			shadow += texture(
			    shadowmap,
			    vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z));
		}
	}
	shadow /= samples * samples;

	return shadow;
#else
	return textureProj(shadowmap, lightspacepos);
#endif
}
