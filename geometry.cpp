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

template <typename T>
struct line
{
	// ax + by = c
	T a, b, c; 
};

template <typename T>
point<T> intersect(const line<T> &l1, const line<T> &l2)
{
	double dd = (l1.b * l2.a) - (l1.a * l2.b);
	T x = ((l1.b * l2.c) - (l1.c * l2.b)) / dd;
	T y = ((l1.c * l2.a) - (l1.a * l2.c)) / dd;

	return {x, y};
}
