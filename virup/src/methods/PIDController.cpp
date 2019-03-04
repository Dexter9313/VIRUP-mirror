#include "methods/PIDController.hpp"

void PIDController::update(uint64_t dt)
{
	if(dt == 0)
	{
		return;
	}

	float err(setPoint - *targetMeasure);

	if(fabsf(err) < tol && !fixing)
	{
		return;
	}
	else if(fabsf(err) < tol / 2.0f)
	{
		fixing = false;
		return;
	}
	fixing = true;

	I += err * (dt / 1000000.0f);
	if(I > 1e4)
	{
		I = 0;
	}
	if(I < -1e4)
	{
		I = 0;
	}
	*controlVariable += Kp * err + Ki * I + Kd * (err - lasterr) / dt;
	if(*controlVariable > 1)
	{
		*controlVariable = 1;
	}
	if(*controlVariable < 0)
	{
		*controlVariable = 0;
	}
	lasterr = err;
}
