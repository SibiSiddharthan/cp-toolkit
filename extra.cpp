#include "cp.h"

// Condition
// nearest smaller value    : <= (less_equal<>())
// nearest smaller or equal : <  (less<>())
// nearest larger value     : >= (greater_equal<>())
// nearest larger or equal  : >  (greater<>())

template <typename T, typename C>
vector<uint32_t> nearest_right(const vector<T> &elements, C comp)
{
	uint32_t size = elements.size();

	vector<uint32_t> nearest(size, 0);
	stack<pair<T, uint32_t>> st;

	nearest.back() = size;
	st.push({elements[size - 1], size - 1});

	for (uint32_t i = size - 2; i < size; --i)
	{
		while (st.size() != 0)
		{
			if (comp(elements[i], st.top().first))
			{
				st.pop();
				continue;
			}

			nearest[i] = st.top().second;
			st.push({elements[i], i});

			break;
		}

		if (st.size() == 0)
		{
			nearest[i] = size;
			st.push({elements[i], i});
		}
	}

	return nearest;
}

template <typename T, typename C>
vector<uint32_t> nearest_left(const vector<T> &elements, C comp)
{
	uint32_t size = elements.size();

	vector<uint32_t> nearest(size, 0);
	stack<pair<T, uint32_t>> st;

	nearest[0] = size;
	st.push({elements[0], 0});

	for (uint32_t i = 1; i < size; ++i)
	{
		while (st.size() != 0)
		{
			if (comp(elements[i], st.top().first))
			{
				st.pop();
				continue;
			}

			nearest[i] = st.top().second;
			st.push({elements[i], i});

			break;
		}

		if (st.size() == 0)
		{
			nearest[i] = size;
			st.push({elements[i], i});
		}
	}

	return nearest;
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

uint32_t mex_of_sorted_array(vector<uint32_t> a)
{
	uint32_t mex = 0;
	uint32_t n = a.size();

	for (uint32_t i = 0; i < n; ++i)
	{
		if (mex >= a[i])
		{
			if (mex == a[i])
			{
				mex += 1;
			}

			continue;
		}

		break;
	}

	return mex;
}

uint32_t next_mex_of_sorted_array(vector<uint32_t> a)
{
	uint32_t mex = 0;
	uint32_t skip = 0;
	uint32_t n = a.size();

	for (uint32_t i = 0; i < n; ++i)
	{
		if (mex >= a[i])
		{
			if (mex == a[i])
			{
				mex += 1;
			}

			continue;
		}

		if (skip == 0)
		{
			mex += 1;
			skip = 1;

			i -= 1;

			continue;
		}

		break;
	}

	if (skip == 0)
	{
		mex += 1;
	}

	return mex;
}

uint64_t compute_xor(uint64_t x)
{
	uint64_t result = 0;

	// Traverse bits from low to high
	for (uint32_t bit = 0; bit < 64; ++bit)
	{
		uint64_t high = x >> (bit + 1);
		uint64_t count = 0;

		// High value excluding current bit with all combinations of lower values (2^bit)
		if (bit != 63)
		{
			count += high * ((uint64_t)1 << bit);
		}

		// If bit is set, remaining will be lower bits + 1
		if (x & ((uint64_t)1 << bit))
		{
			count += x - (x & ~(((uint64_t)1 << bit) - 1));
			count += 1;
		}

		// Bit is set if count is odd
		result |= (count & 1) << bit;
	}

	return result;
}

uint64_t compute_xor_fast(uint64_t x)
{
	switch (x & 3)
	{
	case 0:
		return x;
	case 1:
		return 1;
	case 2:
		return x + 1;
	case 3:
		return 0;
	}

	return 0;
}

template <typename T>
vector<vector<T>> spiral_layers(const vector<vector<T>> &grid)
{
	vector<vector<T>> layers;

	uint32_t rows = grid.size();
	uint32_t columns = grid.front().size();

	uint32_t row_begin = 0, row_end = rows - 1;
	uint32_t column_begin = 0, column_end = columns - 1;

	while (rows > 1 && columns > 1)
	{
		vector<T> layer;
		layer.reserve(2 * (rows + columns - 2));

		// top (1 ... n)
		for (uint32_t i = column_begin; i <= column_end; ++i)
		{
			layer.push_back(grid[row_begin][i]);
		}

		// right (1 ... n)
		for (uint32_t i = row_begin + 1; i <= row_end - 1; ++i)
		{
			layer.push_back(grid[i][column_end]);
		}

		// bottom (n ... 1)
		for (uint32_t i = column_end; i >= column_begin; --i)
		{
			layer.push_back(grid[row_end][i]);

			if (i == 0)
			{
				break;
			}
		}

		// left (n ... 1)
		for (uint32_t i = row_end - 1; i >= row_begin + 1; --i)
		{
			layer.push_back(grid[i][column_begin]);
		}

		layers.push_back(layer);

		rows -= 2;
		columns -= 2;

		row_begin += 1;
		row_end -= 1;

		column_begin += 1;
		column_end -= 1;
	}

	if (rows != 0 && columns != 0)
	{
		vector<T> layer;

		if (rows == 1)
		{
			for (uint32_t i = column_begin; i <= column_end; ++i)
			{
				layer.push_back(grid[row_begin][i]);
			}
		}
		else
		{
			for (uint32_t i = row_begin; i <= row_end; ++i)
			{
				layer.push_back(grid[i][column_begin]);
			}
		}

		layers.push_back(layer);
	}

	return layers;
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
			this->array[right + 1] -= value;
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
			this->grid[x2 + 1][y1] -= value;
		}

		if (y2 + 1 < this->m)
		{
			this->grid[x1][y2 + 1] -= value;
		}
	}

	void sum()
	{
		auto get = [&](size_t x, size_t y) -> T
		{
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

struct mo
{
	struct query
	{
		uint32_t left, right;
		uint32_t query_id, block_id;
	};

	uint32_t range;
	uint32_t count;
	uint32_t block;
	vector<query> queries;

	uint64_t _isqrt(uint64_t x)
	{
		uint64_t left = 1, right = x;
		uint64_t middle = 0;
		uint64_t estimate = 0;
		uint64_t temp = 0;

		while (left <= right)
		{
			middle = (left + right) / 2;
			temp = middle * middle;

			if (temp == x)
			{
				return middle;
			}
			else if (temp > x)
			{
				estimate = middle;

				if (middle == 0)
				{
					break;
				}

				right = middle - 1;
			}
			else
			{
				left = middle + 1;
			}
		}

		return estimate;
	}

	mo(uint32_t range, uint32_t count)
	{
		this->range = range;
		this->count = count;
		this->block = this->_isqrt(range);
		this->queries = vector<query>(count);

		for (uint32_t i = 0; i < count; ++i)
		{
			cin >> this->queries[i].left >> this->queries[i].right;

			this->queries[i].left--;
			this->queries[i].right--;
			this->queries[i].query_id = i;
			this->queries[i].block_id = this->queries[i].left / this->block;
		}

		sort(this->queries.begin(), this->queries.end(),
			 [](const query &a, const query &b)
			 {
				 if (a.block_id == b.block_id)
				 {
					 return a.right < b.right;
				 }

				 return a.block_id < b.block_id;
			 });
	}

	vector<uint32_t> process(const vector<uint32_t> &elements)
	{
		// Problem Specifics

		// Common
		vector<uint32_t> ans(this->count, 0);

		uint32_t current_left = 0;
		uint32_t current_right = 0;

		// At the end of each query [l,r] current will be [l,r+1]
		for (auto &q : this->queries)
		{
			uint32_t left = q.left;
			uint32_t right = q.right;

			while (current_left > left)
			{
				current_left--;

				// Add
			}

			while (current_right < right + 1)
			{
				// Add

				current_right++;
			}

			while (current_left < left)
			{
				// Remove

				current_left++;
			}

			while (current_right > right + 1)
			{
				current_right--;

				// Remove
			}

			// Set the answer
			ans[q.query_id] = 0;
		}

		return ans;
	}
};

void disjoint_sparse_divide(uint32_t n, uint32_t q)
{
	vector<array<uint32_t, 5>> qp(q);
	map<array<uint32_t, 2>, uint32_t> ranges_map;

	ranges_map[{0, 0}] = 0;

	for (uint32_t i = 0; i < q; ++i)
	{
		if (qp[i][0] == qp[i][1])
		{
			qp[i][4] = 0;
			continue;
		}

		uint32_t left = 0, right = n - 1;
		uint32_t middle = 0;

		while (left < right)
		{
			middle = (left + right) / 2;

			if (qp[i][0] <= middle && qp[i][1] >= middle + 1)
			{
				if (!ranges_map.contains({left, right}))
				{
					uint32_t s = ranges_map.size();
					ranges_map[{left, right}] = s;
				}

				qp[i][4] = ranges_map[{left, right}];

				break;
			}

			if (qp[i][0] > middle)
			{
				left = middle + 1;
			}
			else
			{
				right = middle;
			}
		}
	}
}
