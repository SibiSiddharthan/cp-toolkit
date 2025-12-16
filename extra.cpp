#include <vector>
#include <stack>
#include <queue>

using namespace std;

template <typename T>
using min_priority_queue = priority_queue<T, vector<T>, greater<T>>;

template <typename T>
using max_priority_queue = priority_queue<T, vector<T>, less<T>>;

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

template <typename T>
vector<size_t> nearest_smaller_left(vector<T> &v)
{
	size_t size = v.size();
	vector<size_t> nsv(size, 0);

	stack<pair<uint64_t, uint64_t>> st;

	nsv[size - 1] = size;
	st.push({v[size - 1], size - 1});

	for (uint32_t i = size - 1; i != 0; --i)
	{
		while (st.size() != 0)
		{
			if (v[i - 1] <= st.top().first)
			{
				st.pop();
			}
			else
			{
				nsv[i - 1] = st.top().second;
				st.push({v[i - 1], i - 1});
				break;
			}
		}

		if (st.size() == 0)
		{
			nsv[i - 1] = size;
			st.push({v[i - 1], i - 1});
		}
	}

	return nsv;
}

template <typename T>
vector<T> min_suffix_sum(vector<T> &v, vector<size_t> &nsv)
{
	size_t size = v.size();
	vector<T> suffix_sum(size, 0);

	for (size_t i = size; i != 0; --i)
	{
		suffix_sum[i - 1] = ((nsv[i - 1] != size) ? suffix_sum[nsv[i - 1]] : 0) + (v[i - 1] * ((nsv[i - 1] - i) + 1));
	}

	return suffix_sum
}

template <typename T>
struct range_array
{
	vector<T> array;
	size_t size;

	range_array(size_t size)
	{
		this->array = vector<T>(size, 0);
		this->size = size;
	}

	void add(size_t left, size_t right, T value)
	{
		if (left > this->size)
		{
			left = 0;
		}

		this->array[left] += value;

		if (right + 1 < this->size)
		{
			this->array[right + 1] += ~value + 1;
		}
	}

	void sum()
	{
		T sum = 0;

		for (size_t i = 0; i < this->size; ++i)
		{
			sum += this->array[i];
			this->array[i] = sum;
		}
	}
};

template <typename T>
struct range_grid
{
	vector<vector<T>> grid;
	size_t n, m;

	range_grid(size_t n, size_t m)
	{
		this->grid = vector<vector<T>>(n, vector<T>(m, 0));
		this->n = n;
		this->m = m;
	}

	void add(size_t x1, size_t y1, size_t x2, size_t y2, T value)
	{
		if (x1 > this->n)
		{
			x1 = 0;
		}

		if (y1 > this->m)
		{
			y1 = 0;
		}

		this->grid[x1][y1] += value;

		if (((x2 + 1) < this->n) && ((y2 + 1) < this->m))
		{
			this->grid[x2 + 1][y2 + 1] += value;
		}

		if (x2 + 1 < this->n)
		{
			this->grid[x2 + 1][y1] += ~value + 1;
		}

		if (y2 + 1 < this->m)
		{
			this->grid[x1][y2 + 1] += ~value + 1;
		}
	}

	void sum()
	{
		auto get = [&](size_t x, size_t y) -> T {
			if (x >= this->n || y >= this->m)
			{
				return 0;
			}

			return this->grid[x][y];
		};

		for (size_t i = 0; i < n; ++i)
		{
			for (size_t j = 0; j < m; ++j)
			{
				this->grid[i][j] = (this->grid[i][j] + get(i - 1, j) + get(i, j - 1)) - get(i - 1, j - 1);
			}
		}
	}
};

template <typename T>
struct sweepline
{
	min_priority_queue<pair<T, T>> pq;
	T min_pos = 0, max_pos = 0;

	sweepline(T min, T max)
	{
		this->min_pos = min;
		this->max_pos = max;
	}

	void add_begin(T pos, T value)
	{
		if (pos < this->min_pos || pos > this->max_pos)
		{
			pos = this->min_pos;
		}

		this->pq.push({pos, value});
	}

	void add_end(T pos, T value)
	{
		if (pos > this->max_pos || pos < this->min_pos)
		{
			pos = this->max_pos;
		}

		this->pq.push({pos, (~value) + 1});
	}

	uint32_t sweep(T value)
	{
		T count = 0;

		while (this->pq.size() != 0)
		{
			auto top = this->pq.top();
			count += top.second;

			if (count >= value)
			{
				return 1;
			}

			this->pq.pop();
		}

		return 0;
	}
};
