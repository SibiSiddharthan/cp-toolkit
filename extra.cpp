#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

template <typename T>
using min_priority_queue = priority_queue<T, vector<T>, greater<T>>;

template <typename T>
using max_priority_queue = priority_queue<T, vector<T>, less<T>>;

template <typename T>
struct forward_prefix_sums
{
	vector<T> prefix;
	uint32_t size;

	forward_prefix_sums(uint32_t size)
	{
		this->size = size;
		this->prefix = vector<T>(size, 0);
	}

	forward_prefix_sums(const vector<T> &elements)
	{
		this->size = elements.size();
		this->prefix = elements;

		this->construct();
	}

	T &operator[](size_t index)
	{
		return this->prefix[index];
	}

	void construct()
	{
		T sum = 0;

		for (uint32_t i = 0; i < this->size; ++i)
		{
			sum += this->prefix[i];
			this->prefix[i] = sum;
		}
	}

	T sum(uint32_t left, uint32_t right)
	{
		return this->prefix[right] - (left == 0 ? 0 : this->prefix[left - 1]);
	}
};



template <typename T>
vector<uint32_t> nearest_right(const vector<T> &elements)
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
			// Condition
			// nearest smaller value <=
			// nearest smaller or equal <
			// nearest larger value >=
			// nearest larger or equal >
			if (elements[i] <= st.top().first)
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
vector<uint32_t> nearest_left(const vector<T> &elements)
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
			// Condition
			// nearest smaller value <=
			// nearest smaller or equal <
			// nearest larger value >=
			// nearest larger or equal >
			if (elements[i] <= st.top().first)
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

template <typename T>
struct max_subarray_sum
{
	struct node
	{
		T total_sum;  // Total sum of segment
		T best_sum;   // Best sum of segment
		T max_prefix; // Max prefix sum of segment
		T max_suffix; // Max suffix sum of segment
	};

	vector<node> tree;
	stack<uint32_t> st;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	node _op(node &left, node &right)
	{
		node result = {0, 0, 0, 0};

		result.total_sum = left.total_sum + right.total_sum;
		result.max_prefix = MAX(left.max_prefix, left.total_sum + right.max_prefix);
		result.max_suffix = MAX(right.max_suffix, right.total_sum + left.max_suffix);

		result.best_sum = MAX(left.best_sum, right.best_sum);
		result.best_sum = MAX(result.best_sum, left.max_suffix + right.max_prefix);

		return result;
	}

	void _join(uint32_t index)
	{
		if (((index * 2) + 2) < (this->offset + this->size))
		{
			this->tree[index] = this->_op(this->tree[(index * 2) + 1], this->tree[(index * 2) + 2]);
			return;
		}

		if (((index * 2) + 1) < (this->offset + this->size))
		{
			this->tree[index] = this->tree[(index * 2) + 1];
			return;
		}
	}

	pair<uint32_t, uint32_t> _range(uint32_t index)
	{
		uint32_t depth = (32 - __builtin_clz(index + 1)) - 1; // depth of node
		uint32_t count = this->nearest >> depth;              // count of responsibility
		uint32_t segment = (index + 1) & ~(1 << depth);       // index (0 based) of segment at depth
		uint32_t current_left = segment * count;              // left of responsibility
		uint32_t current_right = ((segment + 1) * count) - 1; // right of responsiblity

		return {current_left, current_right};
	}

	void _build(const vector<T> &elements)
	{
		this->nearest = 1 << ((32 - __builtin_clz(elements.size())) - 1);

		if (this->nearest < elements.size())
		{
			this->nearest <<= 1;
		}

		this->offset = this->nearest - 1;
		this->size = elements.size();

		this->tree = vector<node>(this->offset + this->size);

		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset] = {elements[i], MAX(0, elements[i]), MAX(0, elements[i]), MAX(0, elements[i])};
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	max_subarray_sum(const vector<T> &elements)
	{
		this->_build(elements);
	}

	max_subarray_sum(uint32_t size, T value)
	{
		this->_build(vector<int32_t>(size, value));
	}

	void update(uint32_t index, T value)
	{
		uint32_t parent = 0;

		if (index >= this->size)
		{
			return;
		}

		index = this->offset + index;

		this->tree[index] = {value, MAX(0, value), MAX(0, value), MAX(0, value)};

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			this->_join(index);
		}
	}

	node query(uint32_t left, uint32_t right)
	{
		node result = {0, 0, 0, 0};

		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			auto [current_left, current_right] = this->_range(index);

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				result = this->_op(this->tree[index], result);

				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return result;
	}
};
