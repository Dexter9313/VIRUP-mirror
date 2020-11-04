/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

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

#include "profiling.hpp"

#include <QApplication>
#include <QProcess>

void startProfiling()
{
	QProcess p;
	p.start("callgrind_control -i on");
	p.waitForFinished();
}

void stopProfiling(bool close)
{
	QProcess p;
	p.start("callgrind_control -i off");
	p.waitForFinished();
	if(close)
	{
		QApplication::quit();
	}
}
