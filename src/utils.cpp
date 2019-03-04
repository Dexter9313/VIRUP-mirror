#include "utils.hpp"

QString getAbsoluteDataPath(QString const& relativeDataPath)
{
	if(QFile(QString("data/") + PROJECT_DIRECTORY + '/' + relativeDataPath)
	       .exists())
	{
		return QString("data/") + PROJECT_DIRECTORY + '/' + relativeDataPath;
	}
	if(QFile(QString("data/core/") + relativeDataPath).exists())
	{
		return QString("data/core/") + relativeDataPath;
	}
	return relativeDataPath;
}
