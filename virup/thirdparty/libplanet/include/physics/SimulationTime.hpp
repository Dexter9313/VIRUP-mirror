/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef SIMULATIONTIME_HPP
#define SIMULATIONTIME_HPP

#include <QDateTime>
#include <QSettings>
#include <chrono>

#include "UniversalTime.hpp"

typedef std::chrono::steady_clock stdclock;

/*! \ingroup phys
 * Holds and updates the physical time of the simulation called "simulation
 * time".
 *
 * Simulation time flow is not always the same as real time, it can advance
 * faster or slower depending on the "time coefficient". It doesn't even start
 * synchronized with real time. Epoch time can virtually be set to any time as
 * long as physical values (such as ephemeris)
 * depending on it are aware of its real value.
 *
 * A time coefficient of 1 means the simulation will run at real time rate. A
 * time coefficient greater than 1 will make the simulation run faster than
 * real time and a time coefficient smaller than 1 will make the simulation run
 * slower than real time. Time coefficient is proportional
 * (2 means "twice as fast as real time"). A time coefficient of 0 freezes
 * the simulation. (Although graphics may still be rendered and controls may
 * still
 * work over camera for example as the frame counter doesn't depend on the time
 * coefficient.) Finally, a negative time coefficient would be equivalent to its
 * positive counterpart but would make the simulation go backward in time
 * (this might or might not be desirable but as much as this class is concerned,
 * it would work).
 *
 * Simulation time also keeps track on when to draw frames for a given target
 * Frames Per Second value. This allows simulating physics at a higher
 * time resolution than display time resolution.
 *
 * It's the master clock for both \ref graph and \ref phys.
 *
 * It uses and can return a UniversalTime (see \ref UniversalTime.hpp)
 * which is a time type wide
 * enough to contain high values of time in seconds while still maintaining
 * good enough accuracy. It is designed to hold the equivalent of
 * approximately 30 billion years at nanosecond precision -- actually its
 * precision
 * can be higher and depends on the compiler but this is the garanteed minimum.
 * (Our universe is only 13.8 billion years old so there's plenty of time
 * to play with.)
 *
 * A UniversalTime is a number of seconds since epoch. It is a floating-point
 * value.
 *
 * See \ref phys group description for conventions and notations.
 */
class SimulationTime
{
  public:
	/*! Constructor
	 * Sets the initial UniversalTime.
	 *
	 * Default time coefficient is 1 and default target FPS is 60.
	 */
	SimulationTime(UniversalTime startingUt = 0.f);

	SimulationTime(QDateTime const& startingDateTime);

	bool getLockedRealTime() const { return lockedRealTime; };
	/*! Returns the UniversalTime where the simulation was at last #update()
	 * call.
	 */
	UniversalTime getCurrentUt() const { return currentUt; };
	/*! Sets the time coefficient.
	 *
	 * \param timeCoeff time coefficient to be set
	 */
	void setTimeCoeff(float timeCoeff)
	{
		if(!lockedRealTime)
		{
			this->timeCoeff = timeCoeff;
		}
	};
	/*! Returns the currently set time coefficient.
	 */
	float getTimeCoeff() const { return timeCoeff; };
	/*! How much Frames Per Seconds are actually being drawn
	 * (assuming the engine draws frames immediatly on #drawableFrame()
	 * returning true).
	 */
	float getCurrentFPS() const { return currentFPS; };
	/*! Sets the target FPS of the simulation's display
	 *
	 * A target FPS of 0 means there is no limit and #drawableFrame()
	 * will always return true.
	 *
	 * \param targetFPS how much Frames Per Seconds have to be drawn.
	 */
	void setTargetFPS(float targetFPS) { this->targetFPS = targetFPS; };
	/*! Returns the currently set target FPS of the simulation's display
	 */
	float getTargetFPS() const { return targetFPS; };
	/*! Resets the UniversalTime to epoch (0.0 seconds).
	 */
	void resetAtEpoch();

	/*! Updates the current UniversalTime the simulation is at based
	 * on real time measure and time coefficient.
	 *
	 * Also computes the result of #drawableFrame() .
	 */
	void update();

	/*! Returns true if and only if enough time elapsed to draw a new frame
	 * and assumes a frame is drawn by the engine when true is returned
	 * (starts measuring time for next frame).
	 *
	 * This call will change the state of this object, assuming calling
	 * this method means that we want to draw a frame and will do it on
	 * it returning true. The only next time it will return true will
	 * be when another frame will have to be drawn, so make sure only one
	 * piece of code directly relies on it.
	 */
	bool drawableFrame();

	static UniversalTime dateTimeToUT(QDateTime const& dateTime,
	                                  bool utc = true);
	static std::string UTToStr(UniversalTime ut);

  private:
	stdclock::time_point lastTime = stdclock::now();
	UniversalTime currentUt;
	float timeCoeff = 1.f;

	const bool lockedRealTime
	    = QSettings().value("simulation/lockedrealtime").toBool();

	stdclock::time_point lastFrameTime = stdclock::now();
	float currentFPS                   = 0.f;
	float targetFPS                    = 60.f;
	bool drawFrame                     = false;
};

#endif // SIMULATIONTIME_HPP
