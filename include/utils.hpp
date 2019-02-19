#ifndef UTILS_H
#define UTILS_H

#include <QFile>
#include <QString>
#include <vector>

enum class Side
{
	NONE,
	LEFT,
	RIGHT
};

template <typename T>
inline void append(std::vector<T>& v1, std::vector<T> const& v2);

template <typename T>
void append(std::vector<T>& v1, std::vector<T> const& v2)
{
	v1.insert(v1.end(), v2.begin(), v2.end());
}

/* if data/projectdir/relativeDataPath exists,
 *   returns data/projectdir/relativeDataPath
 * else if data/core/relativeDataPath exists,
 *   returns data/core/relativeDataPath
 * else,
 *   returns relativeDataPath
*/
QString getAbsoluteDataPath(QString const& relativeDataPath);

#endif // UTILS_H
