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

template <typename T>
vector<vector<T>> forward_prefix_sums_2d(vector<vector<T>> &v, size_t n, size_t m)
{
	vector<vector<T>> p(n, vector<T>(m, 0));

	auto get = [n, m](vector<vector<T>> &a, size_t x, size_t y) -> T {
		if (x >= n || y >= m)
		{
			return 0;
		}

		return a[x][y];
	};

	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < m; ++j)
		{
			p[i][j] = (v[i][j] + get(p, i - 1, j) + get(p, i, j - 1)) - get(p, i - 1, j - 1);
		}
	}

	return p;
}

template <typename T>
T forward_range_sum_2d(vector<vector<T>> &v, size_t n, size_t m, size_t top, size_t left, size_t bottom, size_t right)
{
	T result = v[bottom][right];

	if ((top - 1) < n && (left - 1) < m)
	{
		result += v[top - 1][left - 1];
	}

	if ((left - 1) < m)
	{
		result -= v[bottom][left - 1];
	}

	if ((top - 1) < n)
	{
		result -= v[top - 1][right];
	}

	return result;
}

template <typename T>
vector<vector<T>> backward_prefix_sums_2d(vector<vector<T>> &v, size_t n, size_t m)
{
	vector<vector<T>> p(n, vector<T>(m, 0));

	auto get = [n, m](vector<vector<T>> &a, size_t x, size_t y) -> T {
		if (x >= n || y >= m)
		{
			return 0;
		}

		return a[x][y];
	};

	for (size_t i = n; i != 0; --i)
	{
		for (size_t j = m; j != 0; --j)
		{
			p[i - 1][j - 1] = (v[i - 1][j - 1] + get(p, i, j - 1) + get(p, i - 1, j)) - get(p, i, j);
		}
	}

	return p;
}

template <typename T>
T backward_range_sum_2d(vector<vector<T>> &v, size_t n, size_t m, size_t top, size_t left, size_t bottom, size_t right)
{
	T result = 0;

	if (top > bottom || left > right)
	{
		return 0;
	}

	result += v[top][left];

	if ((bottom + 1) < n && (right + 1) < m)
	{
		result += v[bottom + 1][right + 1];
	}

	if ((right + 1) < m)
	{
		result -= v[top][right + 1];
	}

	if ((bottom + 1) < n)
	{
		result -= v[bottom + 1][left];
	}

	return result;
}
