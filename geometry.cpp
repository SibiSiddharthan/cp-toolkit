#include "cp.h"

template <typename T>
struct point
{
	T x, y;
};

template <typename T>
T manhattan_distance(point<T> *p1, point<T> *p2)
{
	return DIFF(p1->x, p2->x) + DIFF(p1->y, p2->y);
}

template <typename T>
T euclid_distance_sq(point<T> *p1, point<T> *p2)
{
	return (DIFF(p1->x, p2->x) * DIFF(p1->x, p2->x)) + (DIFF(p1->y, p2->y) * DIFF(p1->y, p2->y));
}

template <typename T>
T euclid_distance(point<T> *p1, point<T> *p2)
{
	return sqrt(((p1->x - p2->x) * (p1->x - p2->x)) + ((p1->y - p2->y) * (p1->y - p2->y)));
}
