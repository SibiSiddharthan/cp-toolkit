#include <vector>
#include <utility>
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
struct disjoint_sparse_table
{
	vector<vector<T>> table;
	uint32_t levels;
	uint32_t size;

	T _join(T a, T b)
	{
		// Operations

		// add
		return a + b;

		// mul
		// return a * b;

		// min
		// return MIN(a, b);

		// max
		// return MAX(a, b);
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

		if (left >= this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

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
		uint32_t value = 0;
		uint32_t last = 0;

		uint32_t children[2];
	};

	uint32_t bits;
	vector<node> tree;

	xor_trie(uint32_t bits)
	{
		this->bits = bits;
		this->tree.push_back({0, 0, {0, 0}});
	}

	void insert(uint32_t value, uint32_t index)
	{
		node &temp = this->tree[0];

		for (uint32_t i = 0; i < this->bits; ++i)
		{
			uint8_t bit = (value >> (this->bits - (i + 1))) & 1;

			if (temp.children[bit] == 0)
			{
				this->tree.push_back({0, 0, {0, 0}});
				temp.children[bit] = this->tree.size() - 1;
			}

			temp = this->tree[temp.children[bit]];
			temp.last = index;
		}

		temp.value = value;
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
};

struct range_min
{
	struct node
	{
		uint32_t value;
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
				this->tree[index].value = this->tree[(index * 2) + 2].value;
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

	range_min(const vector<uint32_t> &elements)
	{
		this->_build(elements);
	}

	range_min(uint32_t size)
	{
		this->_build(vector<uint32_t>(size, 0));
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

	pair<uint32_t, pair<uint32_t, uint32_t>> _query(uint32_t left, uint32_t right)
	{
		uint32_t value = UINT32_MAX;
		uint32_t min_index = 0;
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

	uint32_t query_value(uint32_t left, uint32_t right)
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

struct range_max
{
	struct node
	{
		uint32_t value;
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
				this->tree[index].value = this->tree[(index * 2) + 2].value;
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

	range_max(const vector<uint32_t> &elements)
	{
		this->_build(elements);
	}

	range_max(uint32_t size)
	{
		this->_build(vector<uint32_t>(size, 0));
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

	pair<uint32_t, pair<uint32_t, uint32_t>> _query(uint32_t left, uint32_t right)
	{
		uint32_t value = 0;
		uint32_t min_index = 0;
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

	uint32_t query_value(uint32_t left, uint32_t right)
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

struct simple_segment_tree
{
	struct node
	{
		uint32_t value;
	};

	vector<node> tree;
	stack<uint32_t> st;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	uint32_t _op(uint32_t a, uint32_t b)
	{
		// Operations

		// add
		return a + b;

		// mul
		// return a * b;

		// min
		// return MIN(a, b);

		// max
		// return MAX(a, b);
	}

	void _join(uint32_t index)
	{
		if (((index * 2) + 2) < (this->offset + this->size))
		{
			this->tree[index].value = this->_op(this->tree[(index * 2) + 1].value, this->tree[(index * 2) + 2].value);
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
			this->tree[i + this->offset].value = elements[i];
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	simple_segment_tree(const vector<uint32_t> &elements)
	{
		this->_build(elements);
	}

	simple_segment_tree(uint32_t size)
	{
		this->_build(vector<uint32_t>(size, 0));
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

	uint32_t query(uint32_t left, uint32_t right)
	{
		uint32_t value = 0;

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
				value = this->_op(value, this->tree[index].value);
				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return value;
	}
};

struct lazy_segment_tree
{
	struct node
	{
	};

	struct update
	{
	};

	vector<node> tree;
	vector<update> lazy;

	stack<uint32_t> st;
	stack<uint32_t> up;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	void _join(uint32_t index)
	{
	}

	void _apply(uint32_t index, const update &element)
	{
		if (index >= this->offset + this->size)
		{
			return;
		}

		// Apply to current node

		if (index < this->offset)
		{
			// Push to children
		}
	}

	void _push(uint32_t index)
	{
		if (index < this->offset)
		{
			this->_apply((index * 2) + 1, this->lazy[index]);
			this->_apply((index * 2) + 2, this->lazy[index]);

			this->lazy[index] = {};
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
		this->lazy = vector<update>(this->offset);

		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset] = {};
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	lazy_segment_tree(const vector<uint32_t> &elements)
	{
		this->_build(elements);
	}

	lazy_segment_tree(uint32_t size)
	{
		this->_build(vector<uint32_t>(size, 0));
	}

	void range_update(uint32_t left, uint32_t right, uint8_t value)
	{
		uint64_t element = (uint64_t)1 << value;

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
			auto index = this->st.top();
			auto [current_left, current_right] = this->_range(index);

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				// For beats
				// Determine when all of the nodes are affected
				// Determine when none of the nodes are affected

				continue;
			}

			// Push the updates lazily
			this->_push(index);
			this->up.push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	uint32_t range_query(uint32_t left, uint32_t right)
	{
		uint32_t result = 0;

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
			auto index = this->st.top();
			auto [current_left, current_right] = this->_range(index);

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{

				continue;
			}

			// Push updates
			this->_push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return result;
	}
};

struct rbtree
{
	using key_type = uint32_t;
	using value_type = uint32_t;
	using priority_type = uint32_t;

	struct node
	{
		// Common fields
		key_type key;
		value_type value;
		uint8_t color;

		// Order statisitics
		uint32_t order;

		// Augmented priority
		priority_type priority;

		node *parent, *left, *right;
	};

	vector<node *> pool;
	vector<node *> free;

	node *root;
	uint32_t count;

	node *alloc_node()
	{
		node *n = nullptr;

		this->count += 1;

		if (this->free.size() != 0)
		{
			n = this->free.back();
			this->free.pop_back();

			return n;
		}

		n = (node *)malloc(sizeof(node));
		memset(n, 0, sizeof(node));

		return n;
	}

	void free_node(node *n)
	{
		if (n == nullptr)
		{
			return;
		}

		this->count -= 1;

		memset(n, 0, sizeof(node));
		this->free.push_back(n);
	}

	void left_rotate(node *n)
	{
		node *t = n->right;

		n->right = t->left;

		if (t->left != nullptr)
		{
			t->left->parent = n;
		}

		t->parent = n->parent;

		if (n->parent == nullptr)
		{
			this->root = t;
		}
		else
		{
			if (n->parent->left == n)
			{
				n->parent->left = t;
			}
			else
			{
				n->parent->right = t;
			}
		}

		t->left = n;
		n->parent = t;
	}

	void right_rotate(node *n)
	{
		node *t = n->left;

		n->left = t->right;

		if (t->right != nullptr)
		{
			t->right->parent = n;
		}

		t->parent = n->parent;

		if (n->parent == nullptr)
		{
			this->root = t;
		}
		else
		{
			if (n->parent->left == n)
			{
				n->parent->left = t;
			}
			else
			{
				n->parent->right = t;
			}
		}

		t->right = n;
		n->parent = t;
	}

	node *insert(key_type key, uint8_t duplicate = 0)
	{
		node *n = this->root;
		node *t = nullptr;

		if (n == nullptr)
		{
			n = this->alloc_node();
			n->key = key;

			this->root = n;

			return n;
		}

		while (n != nullptr)
		{
			t = n;

			if (key < n->key)
			{
				n = n->left;
			}
			else
			{
				if (key == n->key)
				{
					if (duplicate == 0)
					{
						return n;
					}
				}

				n = n->right;
			}
		}

		n = this->alloc_node();

		n->key = key;
		n->parent = t;
		n->color = 1;

		if (n->key < t->key)
		{
			t->left = n;
		}
		else
		{
			t->right = n;
		}

		while (n->parent->color)
		{
			node *p = n->parent;
			node *gp = p->parent;
			node *u = nullptr;

			if (gp == nullptr)
			{
				break;
			}

			if (p == gp->left)
			{
				u = gp->right;

				if (u->color)
				{
					p->color = 0;
					u->color = 0;
					gp->color = 0;

					n = gp;
				}
				else
				{
					if (n == p->right)
					{
						n = p;
						this->left_rotate(n);
					}

					p = n->parent;
					gp = p->parent;

					p->color = 0;
					gp->color = 1;

					this->right_rotate(gp);
				}
			}
			else
			{
				u = gp->left;

				if (u->color)
				{
					p->color = 0;
					u->color = 0;
					gp->color = 0;

					n = gp;
				}
				else
				{
					if (n == p->left)
					{
						n = p;
						this->right_rotate(n);
					}

					p = n->parent;
					gp = p->parent;

					p->color = 0;
					gp->color = 1;

					this->left_rotate(gp);
				}
			}
		}

		this->root->color = 0;

		return n;
	}

	void erase(node *node)
	{
		if (node == nullptr)
		{
			return;
		}
	}

	void update(node *node)
	{
		if (node == nullptr)
		{
			return;
		}
	}

	node *find(key_type key)
	{
		node *n = this->root;

		while (n != nullptr)
		{
			if (key == n->key)
			{
				return n;
			}

			if (key < n->key)
			{
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}

		return nullptr;
	}

	node *get(uint32_t order)
	{
	}

	uint8_t contains(key_type key)
	{
		return this->find(key) != nullptr;
	}

	uint32_t size()
	{
		return this->count;
	}

	node *front()
	{
	}

	node *back()
	{
	}

	node *next(node *n)
	{
		node *t = nullptr;

		if (n == nullptr)
		{
			return this->front();
		}

		if (n->right != nullptr)
		{
			t = n->right;

			while (t->left != nullptr)
			{
				t = t->left;
			}

			return t;
		}

		while (n->parent != nullptr)
		{
			t = n->parent;

			if (t->right == n)
			{
				n = t;
				continue;
			}

			return t;
		}

		return nullptr;
	}

	node *prev(node *n)
	{
		node *t = nullptr;

		if (n == nullptr)
		{
			return this->back();
		}

		if (n->left != nullptr)
		{
			t = n->left;

			while (t->right != nullptr)
			{
				t = t->right;
			}

			return t;
		}

		while (n->parent != nullptr)
		{
			t = n->parent;

			if (t->left == n)
			{
				n = t;
				continue;
			}

			return t;
		}

		return nullptr;
	}

	node *find_lt(key_type key)
	{
	}

	node *find_lte(key_type key)
	{
	}

	node *find_gt(key_type key)
	{
	}

	node *find_gte(key_type key)
	{
	}

	uint32_t count_lt(key_type key)
	{
		node *n = this->find_lt(key);

		if (n == nullptr)
		{
			return 0;
		}

		return n->order + 1;
	}

	uint32_t count_lte(key_type key)
	{
		node *n = this->find_lte(key);

		if (n == nullptr)
		{
			return 0;
		}

		return n->order + 1;
	}

	uint32_t count_gt(key_type key)
	{
		node *n = this->find_gt(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->size() - n->order;
	}

	uint32_t count_gte(key_type key)
	{
		node *n = this->find_gte(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->size() - n->order;
	}
};
