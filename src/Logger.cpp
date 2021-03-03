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

#include "Logger.hpp"

std::ofstream& Logger::logFile()
{
	static std::ofstream logFile;
	return logFile;
}

void Logger::init()
{
	logFile().open("log.txt", std::ofstream::out | std::ofstream::trunc);
	logFile() << QDateTime::currentDateTime().toString().toStdString()
	          << std::endl;
	qInstallMessageHandler(log);
}

void Logger::log(QtMsgType type, const QMessageLogContext& context,
                 const QString& msg)
{
	QByteArray localMsg   = msg.toLocal8Bit();
	const char* file      = context.file != nullptr ? context.file : "";
	const char* shortFile = file;
	if(strlen(shortFile) > strlen(BUILD_SRC_DIR) + 1)
	{
		shortFile += strlen(BUILD_SRC_DIR) + 1;
	}
	const char* function = context.function != nullptr ? context.function : "";

	std::string messageTypeStr, messageTypeStrColor;
	switch(type)
	{
		case QtDebugMsg:
			messageTypeStr      = "Debug";
			messageTypeStrColor = "Debug";
			break;
		case QtInfoMsg:
			messageTypeStr      = "Info";
			messageTypeStrColor = "Info";
			break;
		case QtWarningMsg:
			messageTypeStr      = "Warning";
			messageTypeStrColor = "\033[1;33mWarning\033[0m";
			break;
		case QtCriticalMsg:
			messageTypeStr      = "Critical";
			messageTypeStrColor = "\033[31mCritical\033[0m";
			break;
		case QtFatalMsg:
			messageTypeStr      = "Fatal";
			messageTypeStrColor = "\033[31mFatal\033[0m";
			break;
	}
	logFile() << messageTypeStr << " (" << shortFile << ":" << context.line
	          << ", " << function << "):" << std::endl
	          << "\t" << localMsg.constData() << std::endl;
	std::cerr << messageTypeStrColor << " (" << shortFile << ":" << context.line
	          << ", " << function << "):" << std::endl
	          << "\t" << localMsg.constData() << std::endl;
}

void Logger::close()
{
	logFile().close();
}
