#version 150 core

uniform vec3 color;
uniform sampler2D tex;

in float fragAlpha;

out vec4 outColor;

float kernel(float x)
{
	x = abs(x);
	if(x < 0.5)
	{
		return 1 - 6 * x * x + 6 * x * x * x;
	}
	else if(x < 1.0)
	{
		return 2 * (1 - x) * (1 - x) * (1 - x);
	}

	discard;
}

void main()
{
	//outColor = texture(tex, gl_PointCoord) * vec4(color*fragAlpha, fragAlpha);

	// float dist = 2*distance(gl_PointCoord, vec2(0.5, 0.5));
	// outColor = kernel(dist) * vec4(color, fragAlpha);

	outColor = max(0.0, (pow(0.5, 6) - pow(distance(gl_PointCoord, vec2(0.5, 0.5)), 6))) * vec4(color, fragAlpha);
}
