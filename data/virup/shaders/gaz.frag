#version 150 core

uniform vec3 color;
uniform sampler2D tex;

in float fragAlpha;

out vec4 outColor;

void main()
{
	//outColor = texture(tex, gl_PointCoord) * vec4(color*fragAlpha, fragAlpha);

	outColor = max(0.0, (pow(0.5, 6) - pow(distance(gl_PointCoord, vec2(0.5, 0.5)), 6))) * vec4(color, fragAlpha);
}
