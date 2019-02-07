#ifndef UTILS_H
#define UTILS_H

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

#endif // UTILS_H
