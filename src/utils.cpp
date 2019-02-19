#include "utils.hpp"

QString getAbsoluteDataPath(QString const& relativeDataPath)
{
	if(QFile(QString("data/") + PROJECT_DIRECTORY + '/' + relativeDataPath).exists())
		return QString("data/") + PROJECT_DIRECTORY + '/' + relativeDataPath;
	else if(QFile(QString("data/core/") + relativeDataPath).exists())
		return QString("data/core/") + relativeDataPath;
	else
		return relativeDataPath;

}
