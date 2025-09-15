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

template <typename T>
vector<T> backward_prefix_sums(vector<T> &v)
{
	size_t n = v.size();
	T sum = 0;

	vector<T> p(n, 0);

	for (size_t i = n; i != 0; --i)
	{
		sum += v[i - 1];
		p[i - 1] = sum;
	}

	return p;
}

template <typename T>
T range_sum(vector<T> &prefix_sums, size_t left, size_t right)
{
	return prefix_sums[right] - (left == 0 ? 0 : prefix_sums[left - 1]);
}
