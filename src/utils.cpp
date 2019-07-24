#include "utils.hpp"

QString getAbsoluteDataPath(QString const& relativeDataPath)
{
	if(QFile(QString("data/") + PROJECT_DIRECTORY + '/' + relativeDataPath)
	       .exists())
	{
		return QString("data/") + PROJECT_DIRECTORY + '/' + relativeDataPath;
	}

	QStringList dirs;
	QDirIterator it("data/", QStringList() << "*", QDir::AllDirs);
	while(it.hasNext())
	{
		dirs << it.next();
	}

	dirs.removeAll("data/.");
	dirs.removeAll("data/..");
	dirs.removeAll("data/core");
	dirs.removeAll(QString("data/") + PROJECT_DIRECTORY);

	for(auto& dir : dirs)
	{
		if(QFile(dir + "/" + relativeDataPath).exists())
		{
			return dir + "/" + relativeDataPath;
		}
	}

	if(QFile(QString("data/core/") + relativeDataPath).exists())
	{
		return QString("data/core/") + relativeDataPath;
	}
	return relativeDataPath;
}
