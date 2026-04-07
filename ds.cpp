#include <vector>
#include <utility>
#include <array>
#include <stack>
#include <queue>
#include <map>
#include <limits>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

namespace ops {

template <typename T>
struct add
{
	T operator()(T a, T b) const
	{
		return a + b;
	}
};

template <typename T>
struct mul
{
	T operator()(T a, T b) const
	{
		return a * b;
	}
};

template <typename T>
struct min
{
	T operator()(T a, T b) const
	{
		return MIN(a, b);
	}
};

template <typename T>
struct max
{
	T operator()(T a, T b) const
	{
		return MAX(a, b);
	}
};

} // namespace ops

template <typename T, template <typename> class O>
	requires same_as<invoke_result_t<decltype(O<T>()), T, T>, T>
struct disjoint_sparse_table
{
	vector<vector<T>> table;
	uint32_t levels;
	uint32_t size;

	T _join(T a, T b)
	{
		return O<T>{}(a, b);
	}

	disjoint_sparse_table()
	{
	}

	disjoint_sparse_table(vector<T> &elements)
	{
		uint32_t size = 1;

		this->levels = 0;
		this->size = elements.size();

		do
		{
			size <<= 1;
			this->levels += 1;
		} while (size < this->size);

		this->table = vector<vector<T>>(this->levels, vector<T>(this->size));

		for (uint32_t i = 0; i < this->levels; ++i)
		{
			uint32_t begin = 0;
			uint32_t end = 0;
			uint32_t middle = 0;

			while (begin < this->size)
			{
				end = (begin + size) - 1;
				middle = (begin + end) / 2;

				// Fixup middle and end if we are exceeding the array
				end = MIN(end, this->size - 1);

				if (middle >= end)
				{
					middle = (begin + end) / 2;
				}

				if (begin == end)
				{
					this->table[i][middle] = elements[middle];
					begin = end + 1;

					continue;
				}

				this->table[i][middle] = elements[middle];

				for (uint32_t j = middle - 1; j >= begin && j < this->size; --j)
				{
					this->table[i][j] = this->_join(this->table[i][j + 1], elements[j]);
				}

				this->table[i][middle + 1] = elements[middle + 1];

				for (uint32_t j = middle + 2; j <= end; ++j)
				{
					this->table[i][j] = this->_join(this->table[i][j - 1], elements[j]);
				}

				begin = end + 1;
			}

			size >>= 1;
		}
	}

	T query(uint32_t left, uint32_t right)
	{
		uint32_t level = 0;
		T result = 0;

		if (left == right)
		{
			return this->table[this->levels - 1][left];
		}

		level = this->levels - (32 - __builtin_clz(left ^ right));
		result = this->_join(this->table[level][left], this->table[level][right]);

		return result;
	}
};

template <typename T>
struct fenwick_tree
{
	vector<T> tree;
	uint32_t size = 0;

	fenwick_tree(const vector<T> &value)
	{
		this->tree = value;
		this->size = value.size();

		for (uint32_t i = 0; i < size; ++i)
		{
			uint32_t above = i + ((uint32_t)1 << __builtin_ctzll(i + 1));

			if (above < size)
			{
				this->tree[above] = this->tree[above] + this->tree[i];
			}
		}
	}

	T psum(uint32_t index)
	{
		T sum = 0;
		index += 1;

		while (index != 0)
		{
			sum = sum + tree[index - 1];
			index -= (uint32_t)1 << __builtin_ctzll(index);
		}

		return sum;
	}

	T sum(uint32_t left, uint32_t right)
	{
		if (left >= this->size || right >= this->size)
		{
			return 0;
		}

		if (left == 0)
		{
			return this->psum(right);
		}
		else
		{
			return this->psum(right) - this->psum(left - 1);
		}
	}

	void update(uint32_t index, T value)
	{
		T old = this->sum(index, index);

		while (index < size)
		{
			this->tree[index] = (this->tree[index] + value) - old;
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}
};

struct xor_trie
{
	struct node
	{
		uint32_t children[2];
	};

	uint32_t bits;
	vector<node> tree;

	xor_trie(uint32_t bits)
	{
		this->bits = bits;
		this->tree.push_back({{0, 0}});
	}

	void insert(uint32_t value)
	{
		uint32_t index = 0;

		for (uint32_t i = 0; i < this->bits; ++i)
		{
			uint8_t bit = (value >> (this->bits - (i + 1))) & 1;

			if (this->tree[index].children[bit] == 0)
			{
				this->tree.push_back({{0, 0}});
				this->tree[index].children[bit] = this->tree.size() - 1;
			}

			index = this->tree[index].children[bit];
		}
	}

	uint32_t query(uint32_t value)
	{
		uint32_t result = 0;
		uint32_t index = 0;

		for (uint32_t i = 0; i < this->bits; ++i)
		{
			uint8_t bit = (value >> (this->bits - (i + 1))) & 1;

			if (this->tree[index].children[bit ^ 1] != 0)
			{
				index = this->tree[index].children[bit ^ 1];
				result |= 1 << (this->bits - (i + 1));
			}
			else
			{
				index = this->tree[index].children[bit];
			}
		}

		return result;
	}
};

struct disjoint_set_union
{
	struct properties
	{
		// Required
		uint32_t parent;
		uint32_t size;

		// Problem Specifics
	};

	vector<properties> components;

	disjoint_set_union(uint32_t size)
	{
		this->components.resize(size);

		for (uint32_t i = 0; i < size; ++i)
		{
			// Common
			this->components[i].parent = i;
			this->components[i].size = 1;

			// Specifics
		}
	}

	uint32_t leader(uint32_t a)
	{
		uint32_t parent = a;
		vector<uint32_t> updates;

		while (this->components[parent].parent != parent)
		{
			updates.push_back(parent);
			parent = this->components[parent].parent;
		}

		for (uint32_t i : updates)
		{
			this->components[i].parent = parent;
		}

		return parent;
	}

	uint32_t size(uint32_t a)
	{
		return this->components[this->leader(a)].size;
	}

	bool same(uint32_t a, uint32_t b)
	{
		return this->leader(a) == this->leader(b);
	}

	void merge(uint32_t a, uint32_t b)
	{
		// Common

		uint32_t leader_a = this->leader(a);
		uint32_t leader_b = this->leader(b);

		uint32_t small_leader = 0, big_leader = 0;

		if (leader_a == leader_b)
		{
			return;
		}

		if (this->components[leader_a].size >= this->components[leader_b].size)
		{
			big_leader = leader_a;
			small_leader = leader_b;
		}
		else
		{
			big_leader = leader_b;
			small_leader = leader_a;
		}

		this->components[small_leader].parent = big_leader;
		this->components[big_leader].size += this->components[small_leader].size;

		// Specifics
	}

	uint64_t value(uint32_t a)
	{
		uint32_t leader = this->leader(a);

		// Specifics

		return 0;
	}

	auto all()
	{
		map<uint32_t, vector<uint32_t>> result;

		for (uint32_t i = 0; i < this->components.size(); ++i)
		{
			result[this->leader(i)].push_back(i);
		}

		return result;
	}
};

template <typename T>
struct range_min
{
	struct node
	{
		T value;
		uint32_t min_index;
		uint32_t max_index;
	};

	vector<node> tree;
	stack<uint32_t> st;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	pair<uint32_t, uint32_t> _range(uint32_t index)
	{
		uint32_t depth = (32 - __builtin_clz(index + 1)) - 1; // depth of node
		uint32_t count = this->nearest >> depth;              // count of responsibility
		uint32_t segment = (index + 1) & ~(1 << depth);       // index (0 based) of segment at depth
		uint32_t current_left = segment * count;              // left of responsibility
		uint32_t current_right = ((segment + 1) * count) - 1; // right of responsiblity

		return {current_left, current_right};
	}

	void _join(uint32_t index)
	{
		if (((index * 2) + 2) < (this->offset + this->size))
		{
			if (this->tree[(index * 2) + 1].value < this->tree[(index * 2) + 2].value)
			{
				this->tree[index] = this->tree[(index * 2) + 1];
			}
			else if (this->tree[(index * 2) + 2].value < this->tree[(index * 2) + 1].value)
			{
				this->tree[index] = this->tree[(index * 2) + 2];
			}
			else
			{
				this->tree[index].value = this->tree[(index * 2) + 1].value;

				this->tree[index].min_index = MIN(this->tree[(index * 2) + 1].min_index, this->tree[(index * 2) + 2].min_index);
				this->tree[index].max_index = MAX(this->tree[(index * 2) + 1].max_index, this->tree[(index * 2) + 2].max_index);
			}

			return;
		}

		if (((index * 2) + 1) < (this->offset + this->size))
		{
			this->tree[index] = this->tree[(index * 2) + 1];

			return;
		}
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
			this->tree[i + this->offset] = {elements[i], i, i};
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	range_min(const vector<T> &elements)
	{
		this->_build(elements);
	}

	range_min(uint32_t size)
	{
		this->_build(vector<T>(size, numeric_limits<T>::max()));
	}

	void update(uint32_t index, uint32_t value)
	{
		uint32_t parent = 0;

		if (index >= this->size)
		{
			return;
		}

		index = this->offset + index;

		this->tree[index].value = value;

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			this->_join(index);
		}
	}

	pair<T, pair<uint32_t, uint32_t>> _query(uint32_t left, uint32_t right)
	{
		T value = numeric_limits<T>::max();
		uint32_t min_index = UINT32_MAX;
		uint32_t max_index = 0;

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
				if (this->tree[index].value < value)
				{
					value = this->tree[index].value;
					min_index = this->tree[index].min_index;
					max_index = this->tree[index].max_index;
				}
				else if (this->tree[index].value == value)
				{
					min_index = MIN(min_index, this->tree[index].min_index);
					max_index = MAX(max_index, this->tree[index].max_index);
				}

				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return {value, {min_index, max_index}};
	}

	T query_value(uint32_t left, uint32_t right)
	{
		return this->_query(left, right).first;
	}

	uint32_t query_min_index(uint32_t left, uint32_t right)
	{
		return this->_query(left, right).second.first;
	}

	uint32_t query_max_index(uint32_t left, uint32_t right)
	{
		return this->_query(left, right).second.second;
	}
};

template <typename T>
struct range_max
{
	struct node
	{
		T value;
		uint32_t min_index;
		uint32_t max_index;
	};

	vector<node> tree;
	stack<uint32_t> st;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	pair<uint32_t, uint32_t> _range(uint32_t index)
	{
		uint32_t depth = (32 - __builtin_clz(index + 1)) - 1; // depth of node
		uint32_t count = this->nearest >> depth;              // count of responsibility
		uint32_t segment = (index + 1) & ~(1 << depth);       // index (0 based) of segment at depth
		uint32_t current_left = segment * count;              // left of responsibility
		uint32_t current_right = ((segment + 1) * count) - 1; // right of responsiblity

		return {current_left, current_right};
	}

	void _join(uint32_t index)
	{
		if (((index * 2) + 2) < (this->offset + this->size))
		{
			if (this->tree[(index * 2) + 1].value > this->tree[(index * 2) + 2].value)
			{
				this->tree[index] = this->tree[(index * 2) + 1];
			}
			else if (this->tree[(index * 2) + 2].value > this->tree[(index * 2) + 1].value)
			{
				this->tree[index] = this->tree[(index * 2) + 2];
			}
			else
			{
				this->tree[index].value = this->tree[(index * 2) + 1].value;

				this->tree[index].min_index = MIN(this->tree[(index * 2) + 1].min_index, this->tree[(index * 2) + 2].min_index);
				this->tree[index].max_index = MAX(this->tree[(index * 2) + 1].max_index, this->tree[(index * 2) + 2].max_index);
			}

			return;
		}

		if (((index * 2) + 1) < (this->offset + this->size))
		{
			this->tree[index] = this->tree[(index * 2) + 1];

			return;
		}
	}

	void _build(const vector<uint32_t> &elements)
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
			this->tree[i + this->offset] = {elements[i], i, i};
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	range_max(const vector<T> &elements)
	{
		this->_build(elements);
	}

	range_max(uint32_t size)
	{
		this->_build(vector<T>(size, numeric_limits<T>::min()));
	}

	void update(uint32_t index, uint32_t value)
	{
		uint32_t parent = 0;

		if (index >= this->size)
		{
			return;
		}

		index = this->offset + index;

		this->tree[index].value = value;

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			this->_join(index);
		}
	}

	pair<T, pair<uint32_t, uint32_t>> _query(uint32_t left, uint32_t right)
	{
		T value = numeric_limits<T>::min();
		uint32_t min_index = UINT32_MAX;
		uint32_t max_index = 0;

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
				if (this->tree[index].value > value)
				{
					value = this->tree[index].value;
					min_index = this->tree[index].min_index;
					max_index = this->tree[index].max_index;
				}
				else if (this->tree[index].value == value)
				{
					min_index = MIN(min_index, this->tree[index].min_index);
					max_index = MAX(max_index, this->tree[index].max_index);
				}

				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return {value, {min_index, max_index}};
	}

	T query_value(uint32_t left, uint32_t right)
	{
		return this->_query(left, right).first;
	}

	uint32_t query_min_index(uint32_t left, uint32_t right)
	{
		return this->_query(left, right).second.first;
	}

	uint32_t query_max_index(uint32_t left, uint32_t right)
	{
		return this->_query(left, right).second.second;
	}
};

template <typename T>
struct rectangle_query
{
	vector<T> tree;
	uint32_t size = 0;

	rectangle_query(uint32_t size)
	{
		this->size = size;
		this->tree = vector<T>(this->size, 0);
	}

	void _update(uint32_t index, T value)
	{
		while (index < size)
		{
			this->tree[index] += value;
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}

	void add(uint32_t begin, uint32_t end, T value)
	{
		if (begin >= this->size)
		{
			begin = 0;
		}

		this->_update(begin, value);

		if (end + 1 < this->size)
		{
			this->_update((end + 1), ~value + 1);
		}
	}

	T query(uint32_t index)
	{
		T sum = 0;
		index += 1;

		while (index != 0)
		{
			sum = sum + tree[index - 1];
			index -= (uint32_t)1 << __builtin_ctzll(index);
		}

		return sum;
	}
};

struct binary_jumping
{
	vector<vector<array<uint64_t, 2>>> table;

	binary_jumping(const vector<uint32_t> &parents, const vector<uint64_t> &value, uint32_t depth)
	{
		uint32_t size = parents.size();
		this->table = vector<vector<array<uint64_t, 2>>>(depth, vector<array<uint64_t, 2>>(size));

		for (uint32_t j = 0; j < size; ++j)
		{
			this->table[0][j] = {value[j], parents[j]};
		}

		for (uint32_t i = 1; i < depth; ++i)
		{
			for (uint32_t j = 0; j < size; ++j)
			{
				uint64_t v = this->table[i - 1][j][0];
				uint64_t p = this->table[i - 1][j][1];

				this->table[i][j] = {v + this->table[i - 1][p][0], this->table[i - 1][p][1]};
			}
		}
	}

	uint64_t query(uint32_t index, uint32_t depth)
	{
		uint64_t result = 0;
		uint32_t bit = 0;

		for (uint32_t bit = 0; bit < 30; ++bit)
		{
			if (depth & (1 << bit))
			{
				result += this->table[bit][index][0];
				index = this->table[bit][index][1];
			}
		}

		return result;
	}
};
