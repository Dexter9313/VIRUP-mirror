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
#include "../../include/physics/SimulationTime.hpp"

SimulationTime::SimulationTime(UniversalTime startingUt)
    : currentUt(std::move(startingUt))
{
}

SimulationTime::SimulationTime(QDateTime const& startingDateTime)
    : currentUt(dateTimeToUT(startingDateTime))
{
}

void SimulationTime::resetAtEpoch()
{
	lastTime   = stdclock::now();
	currentUt  = 0.0;
	currentFPS = 0.f;
}

void SimulationTime::update()
{
	stdclock::time_point now(stdclock::now());
	double simInterval
	    = std::chrono::duration_cast<std::chrono::duration<double>>(now
	                                                                - lastTime)
	          .count();
	if(!lockedRealTime)
	{
		currentUt += simInterval * timeCoeff;
	}
	else
	{
		currentUt = dateTimeToUT(QDateTime().currentDateTime(), false);
	}
	lastTime = now;

	double frameInterval
	    = std::chrono::duration_cast<std::chrono::duration<double>>(
	          now - lastFrameTime)
	          .count();
	if(targetFPS == 0 || frameInterval >= 1.0 / targetFPS)
	{
		currentFPS    = 1.f / frameInterval;
		drawFrame     = true;
		lastFrameTime = now;
	}
}

bool SimulationTime::drawableFrame()
{
	if(drawFrame)
	{
		drawFrame     = false;
		lastFrameTime = stdclock::now();
		return true;
	}
	return false;
}

UniversalTime SimulationTime::dateTimeToUT(QDateTime const& dateTime, bool utc)
{
	UniversalTime result(dateTime.date().toJulianDay() - 2451545.0);
	result *= 24 * 3600;
	QTime t = utc ? dateTime.time() : dateTime.toUTC().time();
	result += t.hour() * 3600.0;
	result += t.minute() * 60.0;
	result += t.second();
	result += t.msec() / 1000.0;

	return result;
}

std::string SimulationTime::UTToStr(UniversalTime uT)
{
	uT /= 24.0 * 3600.0;
	auto j2000d(static_cast<int64_t>(uT));
	if(uT < 0)
	{
		--j2000d;
	}
	uT -= j2000d;

	UniversalTime timeOfDayInMSecs(uT * 24.0 * 3600.0 * 1000.0);

	QDateTime dt(
	    QDate::fromJulianDay(j2000d + 2451545),
	    QTime::fromMSecsSinceStartOfDay(static_cast<int>(timeOfDayInMSecs)),
	    Qt::UTC);

	QString seconds
	    = QString("%1").arg(dt.toUTC().time().second(), 2, 10, QChar('0'));

	return (dt.toUTC().toString(Qt::SystemLocaleShortDate) + ":" + seconds)
	    .toStdString();
}
