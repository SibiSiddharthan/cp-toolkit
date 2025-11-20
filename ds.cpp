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
vector<vector<T>> build_sparse_table(vector<T> &vec)
{
	vector<vector<uint32_t>> sparse;
	size_t n = vec.size();

	sparse.push_back(vec);

	for (uint32_t k = 2; k < n; k *= 2)
	{
		vector<uint32_t> temp;

		for (uint32_t i = 0; i < n - k + 1; ++i)
		{
			temp.push_back(sparse.back()[i] | sparse.back()[i + (k / 2)]);
		}

		sparse.push_back(temp);
	}

	return sparse;
}

template <typename T>
T sparse_query(vector<vector<T>> &sparse, size_t left, size_t right)
{
	size_t n = right - left + 1;
	size_t l = sparse.size() - 1;
	size_t a = 0;

	while (n > 0)
	{
		while (n < (1 << l))
		{
			--l;
		}

		a |= sparse[l][left];
		left += (1 << l);
		n -= (1 << l);
	}

	return a;
}

template <typename T>
struct fenwick_tree
{
	vector<T> tree;
	size_t size = 0;

	fenwick_tree(const vector<T> &value)
	{
		this->tree = value;
		this->size = value.size();

		for (size_t i = 0; i < size; ++i)
		{
			size_t above = i + ((size_t)1 << __builtin_ctzll(i + 1));

			if (above < size)
			{
				this->tree[above] = this->tree[above] + this->tree[i];
			}
		}
	}

	T psum(size_t index)
	{
		T sum = 0;
		index += 1;

		while (index != 0)
		{
			sum = sum + tree[index - 1];
			index -= (size_t)1 << __builtin_ctzll(index);
		}

		return sum;
	}

	T sum(size_t left, size_t right)
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

	void update(size_t index, T value)
	{
		T old = this->sum(index, index);

		while (index < size)
		{
			this->tree[index] = (this->tree[index] + value) - old;
			index += (size_t)1 << __builtin_ctzll(index + 1);
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
		size_t parent;
		size_t size;

		// Problem Specifics
		uint32_t min;
		uint32_t max;
	};

	vector<properties> components;

	disjoint_set_union(size_t size)
	{
		this->components.resize(size);

		for (size_t i = 0; i < size; ++i)
		{
			// Common
			this->components[i].parent = i;
			this->components[i].size = 1;

			// Specifics
			this->components[i].min = UINT32_MAX;
			this->components[i].max = 0;
		}
	}

	size_t leader(size_t a)
	{
		size_t parent = a;
		vector<size_t> updates;

		while (this->components[parent].parent != parent)
		{
			updates.push_back(parent);
			parent = this->components[parent].parent;
		}

		for (size_t i : updates)
		{
			this->components[i].parent = parent;
		}

		return parent;
	}

	size_t size(size_t a)
	{
		return this->components[this->leader(a)].size;
	}

	void merge(size_t a, size_t b, size_t weight)
	{
		size_t leader_a = this->leader(a);
		size_t leader_b = this->leader(b);

		size_t small_leader = 0, big_leader = 0;

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

		// -------------------------------------------------------------------------------------------------------------
		// Specifics
		// -------------------------------------------------------------------------------------------------------------

		this->components[big_leader].min = MIN(this->components[big_leader].min, this->components[small_leader].min);
		this->components[big_leader].min = MIN(this->components[big_leader].min, weight);

		this->components[big_leader].max = MAX(this->components[big_leader].max, this->components[small_leader].max);
		this->components[big_leader].max = MAX(this->components[big_leader].max, weight);
	}

	bool same(size_t a, size_t b)
	{
		return this->leader(a) == this->leader(b);
	}

	uint64_t value(size_t a)
	{
		size_t leader = this->leader(a);

		return this->components[leader].min + this->components[leader].max;
	}
};

template <typename T>
struct range_minmax
{
	struct minmax
	{
		size_t left;
		size_t right;

		T min_value;
		size_t min_index_smallest;
		size_t min_index_largest;

		T max_value;
		size_t max_index_smallest;
		size_t max_index_largest;
	};

	vector<minmax> tree;
	stack<size_t> st;

	size_t offset;
	size_t size;

	range_minmax(vector<T> elements)
	{
		size_t tree_size = 1;

		while (tree_size < elements.size())
		{
			tree_size <<= 1;
		}

		tree_size -= 1;

		this->offset = tree_size;
		this->size = elements.size();

		this->tree = vector<minmax>(this->offset + this->size);

		for (size_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset].left = i;
			this->tree[i + this->offset].right = i;

			this->tree[i + this->offset].min_value = elements[i];
			this->tree[i + this->offset].min_index_smallest = i;
			this->tree[i + this->offset].min_index_largest = i;

			this->tree[i + this->offset].max_value = elements[i];
			this->tree[i + this->offset].max_index_smallest = i;
			this->tree[i + this->offset].max_index_largest = i;
		}

		for (size_t i = this->offset; i != 0; --i)
		{
			size_t j = i - 1;

			if (((j * 2) + 2) < (this->offset + this->size))
			{
				this->tree[j].left = this->tree[(j * 2) + 1].left;
				this->tree[j].right = this->tree[(j * 2) + 2].right;

				this->tree[j].min_value = MIN(this->tree[(j * 2) + 1].min_value, this->tree[(j * 2) + 2].min_value);
				this->tree[j].min_index_smallest =
					MIN(this->tree[(j * 2) + 1].min_index_smallest, this->tree[(j * 2) + 2].min_index_smallest);
				this->tree[j].min_index_largest = MAX(this->tree[(j * 2) + 1].min_index_largest, this->tree[(j * 2) + 2].min_index_largest);

				this->tree[j].max_value = MAX(this->tree[(j * 2) + 1].max_value, this->tree[(j * 2) + 2].max_value);
				this->tree[j].max_index_smallest =
					MIN(this->tree[(j * 2) + 1].max_index_smallest, this->tree[(j * 2) + 2].max_index_smallest);
				this->tree[j].max_index_largest = MAX(this->tree[(j * 2) + 1].max_index_largest, this->tree[(j * 2) + 2].max_index_largest);

				continue;
			}

			if (((j * 2) + 1) < (this->offset + this->size))
			{
				this->tree[j].left = this->tree[(j * 2) + 1].left;
				this->tree[j].right = this->tree[(j * 2) + 1].right;

				this->tree[j].min_value = this->tree[(j * 2) + 1].min_value;
				this->tree[j].min_index_smallest = this->tree[(j * 2) + 1].min_index_smallest;
				this->tree[j].min_index_largest = this->tree[(j * 2) + 1].min_index_largest;

				this->tree[j].max_value = this->tree[(j * 2) + 1].max_value;
				this->tree[j].max_index_smallest = this->tree[(j * 2) + 1].max_index_smallest;
				this->tree[j].max_index_largest = this->tree[(j * 2) + 1].max_index_largest;

				continue;
			}

			// Empty
			this->tree[j].left = this->size;
			this->tree[j].right = this->size;
		}
	}

	pair<T, pair<size_t, size_t>> query_min(size_t left, size_t right)
	{
		T min_value = -1;
		size_t min_index_smallest = this->size;
		size_t min_index_largest = 0;

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
			size_t current_left = this->tree[index].left;
			size_t current_right = this->tree[index].right;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				min_value = MIN(min_value, this->tree[index].min_value);
				min_index_smallest = MIN(min_index_smallest, this->tree[index].min_index_smallest);
				min_index_largest = MAX(min_index_largest, this->tree[index].min_index_largest);

				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return {min_value, {min_index_smallest, min_index_largest}};
	}

	T query_min_value(size_t left, size_t right)
	{
		return this->query_min(left, right).first;
	}

	pair<size_t, size_t> query_min_index(size_t left, size_t right)
	{
		return this->query_min(left, right).second;
	}

	pair<T, pair<size_t, size_t>> query_max(size_t left, size_t right)
	{
		T max_value = 0;
		size_t max_index_smallest = this->size;
		size_t max_index_largest = 0;

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
			size_t current_left = this->tree[index].left;
			size_t current_right = this->tree[index].right;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				max_value = MAX(max_value, this->tree[index].max_value);
				max_index_smallest = MIN(max_index_smallest, this->tree[index].max_index_smallest);
				max_index_largest = MAX(max_index_largest, this->tree[index].max_index_largest);

				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return {max_value, {max_index_smallest, max_index_largest}};
	}

	T query_max_value(size_t left, size_t right)
	{
		return this->query_max(left, right).first;
	}

	pair<size_t, size_t> query_max_index(size_t left, size_t right)
	{
		return this->query_max(left, right).second;
	}

	void point_update(size_t index, T value)
	{
		size_t parent = 0;

		if (index >= this->size)
		{
			return;
		}

		index = this->offset + index;

		this->tree[index].min_value = value;
		this->tree[index].max_value = value;

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			if (((index * 2) + 2) < (this->offset + this->size))
			{
				this->tree[index].min_value = MIN(this->tree[(index * 2) + 1].min_value, this->tree[(index * 2) + 2].min_value);
				this->tree[index].min_index_smallest =
					MIN(this->tree[(index * 2) + 1].min_index_smallest, this->tree[(index * 2) + 2].min_index_smallest);
				this->tree[index].min_index_largest =
					MAX(this->tree[(index * 2) + 1].min_index_largest, this->tree[(index * 2) + 2].min_index_largest);

				this->tree[index].max_value = MAX(this->tree[(index * 2) + 1].max_value, this->tree[(index * 2) + 2].max_value);
				this->tree[index].max_index_smallest =
					MIN(this->tree[(index * 2) + 1].max_index_smallest, this->tree[(index * 2) + 2].max_index_smallest);
				this->tree[index].max_index_largest =
					MAX(this->tree[(index * 2) + 1].max_index_largest, this->tree[(index * 2) + 2].max_index_largest);

				continue;
			}

			if (((index * 2) + 1) < (this->offset + this->size))
			{
				this->tree[index].min_value = this->tree[(index * 2) + 1].min_value;
				this->tree[index].min_index_smallest = this->tree[(index * 2) + 1].min_index_smallest;
				this->tree[index].min_index_largest = this->tree[(index * 2) + 1].min_index_largest;

				this->tree[index].max_value = this->tree[(index * 2) + 1].max_value;
				this->tree[index].max_index_smallest = this->tree[(index * 2) + 1].max_index_smallest;
				this->tree[index].max_index_largest = this->tree[(index * 2) + 1].max_index_largest;

				continue;
			}
		}
	}
};

template <typename T>
struct segment_tree
{
	struct node
	{
		// Required
		size_t left;
		size_t right;

		// Specifics
		uint64_t or_all;
		uint64_t and_all;
		uint32_t max;
		uint32_t count;
	};

	vector<node> tree;
	stack<size_t> st;
	stack<size_t> up;

	vector<pair<uint64_t, uint64_t>> lazy;

	size_t offset;
	size_t size;

	void _join(size_t index)
	{
		if (((index * 2) + 2) < (this->offset + this->size))
		{
			this->tree[index].or_all = this->tree[(index * 2) + 1].or_all | this->tree[(index * 2) + 2].or_all;
			this->tree[index].and_all = this->tree[(index * 2) + 1].and_all & this->tree[(index * 2) + 2].and_all;

			if (this->tree[(index * 2) + 1].max == this->tree[(index * 2) + 2].max)
			{
				this->tree[index].max = this->tree[(index * 2) + 1].max;
				this->tree[index].count = this->tree[(index * 2) + 1].count + this->tree[(index * 2) + 2].count;
			}
			else if (this->tree[(index * 2) + 1].max > this->tree[(index * 2) + 2].max)
			{
				this->tree[index].max = this->tree[(index * 2) + 1].max;
				this->tree[index].count = this->tree[(index * 2) + 1].count;
			}
			else
			{
				this->tree[index].max = this->tree[(index * 2) + 2].max;
				this->tree[index].count = this->tree[(index * 2) + 2].count;
			}

			return;
		}

		if (((index * 2) + 1) < (this->offset + this->size))
		{
			this->tree[index].or_all = this->tree[(index * 2) + 1].or_all;
			this->tree[index].and_all = this->tree[(index * 2) + 1].and_all;
			this->tree[index].max = this->tree[(index * 2) + 1].max;
			this->tree[index].count = this->tree[(index * 2) + 1].count;

			return;
		}
	}

	void _update(size_t index)
	{
		size_t parent = 0;

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			this->_join(index);
		}
	}

	segment_tree(vector<T> elements)
	{
		size_t tree_size = 1;

		while (tree_size < elements.size())
		{
			tree_size <<= 1;
		}

		tree_size -= 1;

		this->offset = tree_size;
		this->size = elements.size();

		this->tree = vector<node>(this->offset + this->size);
		this->lazy = vector<pair<uint64_t, uint64_t>>(this->offset + this->size, {0, 0});

		for (size_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset].left = i;
			this->tree[i + this->offset].right = i;

			this->tree[i + this->offset].or_all = elements[i];
			this->tree[i + this->offset].and_all = elements[i];
			this->tree[i + this->offset].max = __builtin_popcountll(elements[i]);
			this->tree[i + this->offset].count = 1;
		}

		for (size_t i = this->offset; i != 0; --i)
		{
			size_t index = i - 1;

			this->_join(index);

			if (((index * 2) + 2) < (this->offset + this->size))
			{
				this->tree[index].left = this->tree[(index * 2) + 1].left;
				this->tree[index].right = this->tree[(index * 2) + 2].right;

				continue;
			}

			if (((index * 2) + 1) < (this->offset + this->size))
			{
				this->tree[index].left = this->tree[(index * 2) + 1].left;
				this->tree[index].right = this->tree[(index * 2) + 1].right;

				continue;
			}

			// Empty
			this->tree[index].left = this->size;
			this->tree[index].right = this->size;
		}
	}

	void _apply(size_t index, pair<uint64_t, uint64_t> element)
	{
		if (index >= this->offset + this->size)
		{
			return;
		}

		this->tree[index].max -= __builtin_popcountll(this->tree[index].and_all & element.second);
		this->tree[index].or_all &= ~element.second;
		this->tree[index].and_all &= ~element.second;

		this->tree[index].max += __builtin_popcountll(~this->tree[index].or_all & element.first);
		this->tree[index].or_all |= element.first;
		this->tree[index].and_all |= element.first;

		if (index < this->offset)
		{
			this->lazy[index].first &= ~element.second;
			this->lazy[index].second &= ~element.first;

			this->lazy[index].first |= element.first;
			this->lazy[index].second |= element.second;
		}
	}

	void _push(size_t index)
	{
		this->_apply((index * 2) + 1, this->lazy[index]);
		this->_apply((index * 2) + 2, this->lazy[index]);

		this->lazy[index] = {0, 0};
	}

	void range_insert(size_t left, size_t right, uint8_t value)
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
			size_t current_left = this->tree[index].left;
			size_t current_right = this->tree[index].right;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				if (this->tree[index].and_all & element)
				{
					continue;
				}

				if ((this->tree[index].or_all & element) == 0)
				{
					this->_apply(index, {element, 0});

					continue;
				}
			}

			this->_push(index);
			this->up.push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	void range_remove(size_t left, size_t right, uint8_t value)
	{
		uint64_t element = ((uint64_t)1 << value);

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
			size_t current_left = this->tree[index].left;
			size_t current_right = this->tree[index].right;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				if ((this->tree[index].or_all & element) == 0)
				{
					continue;
				}

				if (this->tree[index].and_all & element)
				{
					this->_apply(index, {0, element});

					continue;
				}
			}

			this->_push(index);
			this->up.push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	pair<uint32_t, uint32_t> query(size_t left, size_t right)
	{
		uint32_t max = 0;
		uint32_t count = 0;

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
			size_t current_left = this->tree[index].left;
			size_t current_right = this->tree[index].right;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				if (this->tree[index].max == max)
				{
					count += this->tree[index].count;
				}
				else if (this->tree[index].max > max)
				{
					max = this->tree[index].max;
					count = this->tree[index].count;
				}

				continue;
			}

			this->_push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return {max, count};
	}
};
