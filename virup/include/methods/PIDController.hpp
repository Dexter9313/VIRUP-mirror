#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <cmath>
#include <cstdint>

class PIDController
{
  public:
	PIDController(){};
	void update(uint64_t dt); // dt in µsec

	float setPoint;
	float const* targetMeasure;
	float* controlVariable;

	float Kp;
	float Ki;
	float Kd;

	float tol = 0;

  private:
	float lasterr = 0;
	float I       = 0;
	bool fixing   = false;
};

#endif // PIDCONTROLLER_H
