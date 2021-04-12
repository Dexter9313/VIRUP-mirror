
vec3 stickToUnitBox(vec3 pos, vec3 dir)
{
	if(pos.x < 0.0)
	{
		pos -= dir * pos.x / dir.x;
	}
	if(pos.y < 0.0)
	{
		pos -= dir * pos.y / dir.y;
	}
	if(pos.z < 0.0)
	{
		pos -= dir * pos.z / dir.z;
	}
	if(pos.x > 1.0)
	{
		pos += dir * ((1.0 - pos.x) / dir.x);
	}
	if(pos.y > 1.0)
	{
		pos += dir * ((1.0 - pos.y) / dir.y);
	}
	if(pos.z > 1.0)
	{
		pos += dir * ((1.0 - pos.z) / dir.z);
	}
	return pos;
}

/*
float densityLoc(vec3 coord, sampler3D dusttex)
{
	//if(min(coord, vec3(1.0)) != coord
	//   || max(coord, vec3(0.0, 0.0, 0.0)) != coord)
	//{
	//	return 0.0;
	//}
	// return 1.0 - step(0.1, length(coord - vec3(0.5, 0.5, 0.5)));
	return texture(dusttex, coord).x;
}

float density(vec3 pos, sampler3D dusttex, mat4 dusttransform)
{
	vec3 coord = (dusttransform * vec4(pos, 1.0)).zyx;
	return densityLoc(coord, dusttex);
}
*/

vec3 att(float NHval)
{
	// g / cm^3 -> atom / cm^3
	NHval /= 1.6726219e-24;
	// atom / cm^3 * (3.086e+21 * kpc) => atom /
	// cm^3 * cm => atom / cm^2
	// 1e-22 constants are in cm^2 / atom => dimensionless (mag)
	vec3 result = 3.086e+21 * vec3(4.3e-22, 5.3e-22, 8.5e-22) * NHval;
	// apply mag reduction to illuminance
	result = pow(vec3(10.0), -1.0 * result / 2.5);
	return result;
}


vec3 attenuation(vec3 from, vec3 to, sampler3D dusttex, mat4 dusttransform)
{
	const int SAMPLES = 40;
	vec3 pA = (dusttransform * vec4(from, 1.0)).zyx;
	vec3 pB = (dusttransform * vec4(to, 1.0)).zyx;
	float l = length(pB-pA);
	pA = stickToUnitBox(pA, (pB-pA) / l);
	vec3 ds           = (pB - pA) / float(SAMPLES);
	vec3 v            = pA + ds * 0.5;

	float NH = 0.0;
	for(int i = 0; i < SAMPLES; i++)
	{
		v += ds;
		NH += texture(dusttex, v).x; // atom / cm^3
	}
	NH *= length(to - from) * length(ds) / l;
	return att(NH);
}

/*
vec3 attenuationDiff(vec3 from, vec3 to, sampler3D dusttex, mat4 dusttransform)
{
	const int SAMPLES_MAX = 40;
	const int SAMPLES_MIN = 20;
	const float MAX_DF = 0.001;

	vec3 pA = (dusttransform * vec4(from, 1.0)).zyx;
	vec3 pB = (dusttransform * vec4(to, 1.0)).zyx;
	float l = length(pB-pA);
	pA = stickToUnitBox(pA, (pB-pA) / l);

	vec3 dsMin     = (pB - pA) / float(SAMPLES_MAX);
	vec3 dsMax     = (pB - pA) / float(SAMPLES_MIN);
	vec3 currentds = dsMin;
	vec3 v         = pA + currentds * 0.5;
	float prev = 0.0;
	float L = length(0.5*currentds);
	float totalL = length(pB - pA);

	float NH = 0.0;
	while(L < totalL)
	{
		float tmp = texture(dusttex, v).x; // atom / cm^3
		NH += tmp * length(currentds);
		currentds = mix(dsMax, dsMin, vec3(clamp(abs(length(tmp - prev)) / MAX_DF, 0.0, 1.0)));
		prev = tmp;
		v += currentds;
		L += length(currentds);
	}

	NH *= 3.086e+21* length(to - from) / l; // atom / cm^3 * (3.086e+21 * kpc) => atom /
	                              // cm^3 * cm => atom / cm^2
	vec3 result = vec3(4.3e-22, 5.3e-22, 8.5e-22)
	              * NH; // constants are in cm^2 / atom => dimensionless (mag)
	result = pow(vec3(10.0),
	             -1.0 * result / 2.5); // apply mag reduction to illuminance

	return result;
}
*/

