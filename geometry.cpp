#include <cstdint>
#include <cmath>

#define ABS_SUB(x, y) ((x) < (y) ? ((y) - (x)) : ((x) - (y)))

template <typename T>
struct point
{
	T x, y;
};

template <typename T>
T manhattan_distance(point<T> *p1, point<T> *p2)
{
	return ABS_SUB(p1->x, p2->x) + ABS_SUB(p1->y, p2->y);
}

template <typename T>
T euclid_distance_sq(point<T> *p1, point<T> *p2)
{
	return (ABS_SUB(p1->x, p2->x) * ABS_SUB(p1->x, p2->x)) + (ABS_SUB(p1->y, p2->y) * ABS_SUB(p1->y, p2->y));
}

template <typename T>
T euclid_distance(point<T> *p1, point<T> *p2)
{
	return sqrt(((p1->x - p2->x) * (p1->x - p2->x)) + ((p1->y - p2->y) * (p1->y - p2->y)));
}
