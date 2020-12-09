#version 150

in vec2 texCoord;
out vec4 outColor;

uniform float exposure;
uniform float dynamicrange       = 10000.0;
uniform float screendynamicrange = 255.0;
uniform float purkinje           = 1.0;
uniform sampler2D tex0;

// technically tone mapping
void main()
{
	vec4 c   = texture2D(tex0, texCoord);
	outColor = c;
	if(dynamicrange > 1.0)
	{
		// vec3 grayscale = vec3(0.299, 0.587, 0.114); // photopic
		// vec3 grayscale = vec3(0.06706982, 0.4851017, 0.44782848); // scotopic
		// (Purkinje effect)
		vec3 grayscale
		    = vec3(0.03856366, 0.35832596, 0.60311038); // Purkinje alternative
		vec3 grey = vec3(dot(grayscale, outColor.rgb));
		// "true" luminance
		vec3 luminance = vec3(dot(vec3(0.2126, 0.7152, 0.0722), outColor.rgb));

		if(purkinje != 0.0)
		{
			outColor.rgb = mix(
			    grey, outColor.rgb,
			    clamp(log(luminance / 0.005) / log(5.0 / 0.005), 0.0, 1.0));
		}
		outColor.rgb *= exposure;
		// b is such that (1/dynamicrange)^b = 1/(2*screendynamicrange) (=>
		// 1/dynamic range value is mapped to half value of a pixel when raised
		// to power b)
		float b = log(2.0 * screendynamicrange) / log(dynamicrange);
		outColor.rgb /= dynamicrange;
		outColor.rgb = pow(outColor.rgb, vec3(b));
	}

	if(outColor.r < 0.5 / 255.0 && outColor.g < 0.5 / 255.0
	   && outColor.b < 0.5 / 255.0)
	{
		return;
	}

	/*float screenminvalue=0.0/255.0;
	outColor.rgb *= 1.0 - screenminvalue;
	outColor.rgb +=screenminvalue;*/
}
