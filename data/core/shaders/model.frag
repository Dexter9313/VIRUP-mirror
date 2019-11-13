#version 150 core

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D ambient;
uniform sampler2D emissive;
uniform sampler2D normals;
uniform sampler2D shininess;
uniform sampler2D opacity;
uniform sampler2D lightmap;

uniform sampler2DShadow shadowmap;

in vec3 f_position;
in vec3 f_tangent;
in vec3 f_normal;
in vec2 f_texcoord;
in vec4 f_lightrelpos;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightAmbiantFactor;
uniform vec3 cameraPosition;

out vec4 outColor;

#include <shadows.glsl>

void main()
{
	mat3 fromtangentspace;
	fromtangentspace[2] = normalize(f_normal);
	if(length(f_tangent) == 0.0)
	{
		fromtangentspace[0] = cross(vec3(0.0, 0.0, 1.0), fromtangentspace[2]);
	}
	else
	{
		fromtangentspace[0] = normalize(f_tangent);
	}

	// Gram-Schmidt re-orthogonalize T with respect to N
	fromtangentspace[0] = normalize(
	    fromtangentspace[0]
	    - dot(fromtangentspace[0], fromtangentspace[2]) * fromtangentspace[2]);

	fromtangentspace[1] = cross(fromtangentspace[2], fromtangentspace[0]);

	vec4 diffuseColor   = texture(diffuse, f_texcoord);
	vec4 specularColor  = texture(specular, f_texcoord);
	vec4 ambientColor   = texture(ambient, f_texcoord);
	vec4 emissiveColor  = texture(emissive, f_texcoord);
	vec4 normalColor    = texture(normals, f_texcoord);
	vec4 shininessColor = texture(shininess, f_texcoord);
	vec4 opacityColor   = texture(opacity, f_texcoord);
	vec4 lightmapColor  = texture(lightmap, f_texcoord);

	vec3 normal = normalize(fromtangentspace * (normalColor.rgb * 2.0 - 1.0));

	// todo use normalmap
	float lightcoeff = max(0.0, dot(normal, -1.0 * lightDirection));

	// shadow map
	float shadow = computeShadow(f_lightrelpos, shadowmap);
	lightcoeff *= shadow;

	// diffuse
	outColor.rgb
	    = max(lightAmbiantFactor, lightcoeff) * diffuseColor.rgb * lightColor;

	// specular
	vec3 viewDir    = normalize(cameraPosition - f_position);
	vec3 reflectDir = reflect(lightDirection, normal);
	float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	outColor.rgb
	    += spec * lightColor * max(0.0, lightcoeff) * specularColor.rgb;
	outColor /= 2.0;

	outColor.rgb += emissiveColor.rgb;

	// transparency
	outColor.a = 1.0;

	// lightmap
	outColor.rgb *= lightmapColor.r;
}
