#define PI 3.1415

// Disk on disk occlusion proportion (see notes/disk-occlusion.ggb in Geogebra geometry)
float getNeighborOcclusionFactor(float rLight, float rNeighbor, float dist)
{
	// both disks don't meet
	if(dist >= rLight + rNeighbor)
	{
		return 0.0;
	}

	// they meet and light is a point so occlusion == 1.0
	// early return because of divisions by sLight == 0.0
	if(rLight == 0.0)
	{
		return 1.0;
	}

	// surfaces of light disk and neighbor disk
	float sLight    = PI * rLight * rLight;
	float sNeighbor = PI * rNeighbor * rNeighbor;

	// disks intersection surface
	float sX = 0.0;

	// one disk is included in the other
	if(dist <= abs(rLight - rNeighbor))
	{
		sX = min(sLight, sNeighbor);
	}
	else
	{
		float alpha
		    = ((rLight * rLight) - (rNeighbor * rNeighbor) + (dist * dist))
		      / (2.0 * dist);
		float x = sqrt((rLight * rLight) - (alpha * alpha));

		float gammaLight = asin(x / rLight);
		if(alpha < 0.0)
		{
			gammaLight = PI - gammaLight;
		}
		float gammaNeighbor = asin(x / rNeighbor);
		if(alpha > dist)
		{
			gammaNeighbor = PI - gammaNeighbor;
		}

		sX = (rLight * rLight * gammaLight)
		     + (rNeighbor * rNeighbor * gammaNeighbor) - (dist * x);
	}

	return sX / sLight;
}

