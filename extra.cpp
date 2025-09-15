#include <vector>

using namespace std;

template <typename T>
vector<T> forward_prefix_sums(vector<T> &v)
{
	size_t n = v.size();
	T sum = 0;

	vector<T> p(n, 0);

	for (size_t i = 0; i < n; ++i)
	{
		sum += v[i];
		p[i] = sum;
	}

	return p;
}

