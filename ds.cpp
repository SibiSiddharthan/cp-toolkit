#include "cp.h"
#include "ops.cpp"

template <typename T, class O>
	requires commutative_operator<O, T>
struct prefix_sums
{
	vector<T> elements;
	O op;

	prefix_sums()
	{
	}

	template <typename... args>
	prefix_sums(uint32_t size, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->elements = vector<T>(size, this->op.identity());
	}

	template <typename U, typename... args>
	prefix_sums(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		if constexpr (std::same_as<T, U>)
		{
			this->elements = elements;
		}
		else
		{
			this->elements.reserve(elements.size());

			for (const auto &i : elements)
			{
				this->elements.push_back(i);
			}
		}

		this->build();
	}

	T &operator[](uint32_t index)
	{
		return this->elements[index];
	}

	void build()
	{
		T sum = this->op.identity();
		uint32_t size = this->elements.size();

		for (uint32_t i = 0; i < size; ++i)
		{
			sum = this->op.join(sum, this->elements[i]);
			this->elements[i] = sum;
		}
	}

	T sum(uint32_t left, uint32_t right)
	{
		return ((left != 0) ? this->op.inverse(this->elements[right], this->elements[left - 1]) : this->elements[right]);
	}
};

template <typename T, class O>
	requires commutative_operator<O, T>
struct suffix_sums
{
	vector<T> elements;
	O op;

	suffix_sums()
	{
	}

	template <typename... args>
	suffix_sums(uint32_t size, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->elements = vector<T>(size, this->op.identity());
	}

	template <typename U, typename... args>
	suffix_sums(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		if constexpr (std::same_as<T, U>)
		{
			this->elements = elements;
		}
		else
		{
			this->elements.reserve(elements.size());

			for (const auto &i : elements)
			{
				this->elements.push_back(i);
			}
		}

		this->build();
	}

	T &operator[](uint32_t index)
	{
		return this->elements[index];
	}

	void build()
	{
		T sum = this->op.identity();
		uint32_t size = this->elements.size();

		for (uint32_t i = size; i != 0; --i)
		{
			sum = this->op.join(sum, this->elements[i - 1]);
			this->elements[i - 1] = sum;
		}
	}

	T sum(uint32_t left, uint32_t right)
	{
		return (((right + 1) < this->elements.size()) ? this->op.inverse(this->elements[left], this->elements[right + 1])
													  : this->elements[left]);
	}
};

// (1,1) -> (n,n)
template <typename T, class O>
	requires commutative_operator<O, T>
struct prefix_sums_2d
{
	vector<vector<T>> elements;
	uint32_t n, m;

	O op;

	T _get(uint32_t x, uint32_t y)
	{
		if (x >= this->n || y >= this->m)
		{
			return this->op.identity();
		}

		return this->elements[x][y];
	}

	vector<T> &operator[](uint32_t index)
	{
		return this->elements[index];
	}

	prefix_sums_2d()
	{
	}

	template <typename... args>
	prefix_sums_2d(uint32_t n, uint32_t m, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->n = n;
		this->m = m;
		this->elements = vector<vector<T>>(n, vector<T>(m, this->op.identity()));
	}

	template <typename U, typename... args>
	prefix_sums_2d(const vector<vector<U>> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->n = elements.size();
		this->m = elements[0].size();

		if constexpr (std::same_as<T, U>)
		{
			this->elements = elements;
		}
		else
		{
			this->elements.reserve(this->n);

			for (const auto &i : elements)
			{
				vector<T> elems;

				elems.reserve(this->m);

				for (const auto &j : i)
				{
					elems.push_back(j);
				}

				this->elements.push_back(elems);
			}
		}

		this->build();
	}

	void build()
	{
		for (uint32_t i = 0; i < n; ++i)
		{
			for (uint32_t j = 0; j < m; ++j)
			{
				T temp = this->elements[i][j];

				temp = this->op.join(temp, this->_get(i - 1, j));
				temp = this->op.join(temp, this->_get(i, j - 1));
				temp = this->op.inverse(temp, this->_get(i - 1, j - 1));

				this->elements[i][j] = temp;
			}
		}
	}

	T sum(uint32_t top, uint32_t left, uint32_t bottom, uint32_t right)
	{
		T result = this->elements[bottom][right];

		if ((top - 1) < this->n && (left - 1) < this->m)
		{
			result = this->op.join(result, this->elements[top - 1][left - 1]);
		}

		if ((left - 1) < this->m)
		{
			result = this->op.inverse(result, this->elements[bottom][left - 1]);
		}

		if ((top - 1) < this->n)
		{
			result = this->op.inverse(result, this->elements[top - 1][right]);
		}

		return result;
	}
};

// (n,n) -> (1,1)
template <typename T, class O>
	requires commutative_operator<O, T>
struct suffix_sums_2d
{
	vector<vector<T>> elements;
	uint32_t n, m;

	O op;

	T _get(uint32_t x, uint32_t y)
	{
		if (x >= this->n || y >= this->m)
		{
			return this->op.identity();
		}

		return this->elements[x][y];
	}

	vector<T> &operator[](uint32_t index)
	{
		return this->elements[index];
	}

	suffix_sums_2d()
	{
	}

	template <typename... args>
	suffix_sums_2d(uint32_t n, uint32_t m, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->n = n;
		this->m = m;
		this->elements = vector<vector<T>>(n, vector<T>(m, this->op.identity()));
	}

	template <typename U, typename... args>
	suffix_sums_2d(const vector<vector<U>> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->n = elements.size();
		this->m = elements[0].size();

		if constexpr (std::same_as<T, U>)
		{
			this->elements = elements;
		}
		else
		{
			this->elements.reserve(this->n);

			for (const auto &i : elements)
			{
				vector<T> elems;

				elems.reserve(this->m);

				for (const auto &j : i)
				{
					elems.push_back(j);
				}

				this->elements.push_back(elems);
			}
		}

		this->build();
	}

	void build()
	{
		for (uint32_t i = n; i != 0; --i)
		{
			for (uint32_t j = m; j != 0; --j)
			{
				T temp = this->elements[i - 1][j - 1];

				temp = this->op.join(temp, this->_get(i, j - 1));
				temp = this->op.join(temp, this->_get(i - 1, j));
				temp = this->op.inverse(temp, this->_get(i, j));

				this->elements[i - 1][j - 1] = temp;
			}
		}
	}

	T sum(uint32_t top, uint32_t left, uint32_t bottom, uint32_t right)
	{
		T result = this->elements[top][left];

		if ((bottom + 1) < this->n && (right + 1) < this->m)
		{
			result = this->op.join(result, this->elements[bottom + 1][right + 1]);
		}

		if ((right + 1) < this->m)
		{
			result = this->op.inverse(result, this->elements[top][right + 1]);
		}

		if ((bottom + 1) < this->n)
		{
			result = this->op.inverse(result, this->elements[bottom + 1][left]);
		}

		return result;
	}
};

template <typename T, class O>
	requires commutative_operator<O, T>
struct fenwick_tree
{
	vector<T> tree;
	O op;

	fenwick_tree()
	{
	}

	template <typename U, typename... args>
	fenwick_tree(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		uint32_t size = elements.size();

		if constexpr (std::same_as<T, U>)
		{
			this->tree = elements;
		}
		else
		{
			this->tree.reserve(elements.size());

			for (const auto &i : elements)
			{
				this->tree.push_back(i);
			}
		}

		for (uint32_t i = 0; i < size; ++i)
		{
			uint32_t above = i + ((uint32_t)1 << __builtin_ctzll(i + 1));

			if (above < size)
			{
				this->tree[above] = this->op(this->tree[above], this->tree[i]);
			}
		}
	}

	T _sum(uint32_t index)
	{
		T sum = this->op.identity();
		index += 1;

		while (index != 0)
		{
			sum = this->op.join(sum, tree[index - 1]);
			index -= (uint32_t)1 << __builtin_ctzll(index);
		}

		return sum;
	}

	T sum(uint32_t left, uint32_t right)
	{
		return left == 0 ? this->_sum(right) : this->op.inverse(this->_sum(right), this->_sum(left - 1));
	}

	void set(uint32_t index, T value)
	{
		T old = this->sum(index, index);

		while (index < this->tree.size())
		{
			this->tree[index] = this->op.inverse(this->op.join(this->tree[index], value), old);
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}

	void increase(uint32_t index, T value)
	{
		while (index < this->tree.size())
		{
			this->tree[index] = this->op.join(this->tree[index], value);
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}

	void decrease(uint32_t index, T value)
	{
		while (index < this->tree.size())
		{
			this->tree[index] = this->op.inverse(this->tree[index], value);
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}
};

template <typename T, class O>
	requires binary_operator<O, T>
struct disjoint_sparse_table
{
	vector<vector<T>> table;
	uint32_t levels;
	uint32_t size;

	O op;

	T _join(const T &a, const T &b)
	{
		return this->op.join(a, b);
	}

	disjoint_sparse_table()
	{
	}

	template <typename U, typename... args>
	disjoint_sparse_table(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		uint32_t size = 1;

		this->size = elements.size();
		this->levels = (32 - (__builtin_clz(elements.size()) + 1)) + (__builtin_popcount(elements.size()) != 1);
		this->table = vector<vector<T>>(this->levels, vector<T>(this->size));

		size <<= this->levels;

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
		uint32_t level = this->levels - (32 - __builtin_clz(left ^ right));

		if (left == right)
		{
			return this->table[this->levels - 1][left];
		}

		return this->_join(this->table[level][left], this->table[level][right]);
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

	auto search(uint32_t index, uint64_t value)
	{
		uint32_t size = this->table.size();

		for (uint32_t bit = size - 1; bit < size; --bit)
		{
			if (this->table[bit][index][0] <= value)
			{
				value -= this->table[bit][index][0];
				index = this->table[bit][index][1];
			}
		}

		return make_pair(index, value);
	}
};

struct cartesian_tree
{
	uint32_t root;
	vector<vector<uint32_t>> tree;

	vector<uint32_t> &operator[](uint32_t index)
	{
		return this->tree[index];
	}

	cartesian_tree(vector<uint32_t> &elements)
	{
		uint32_t size = elements.size();

		vector<uint32_t> nearest(size, UINT32_MAX);
		stack<uint32_t> st;

		for (uint32_t i = 0; i < size; ++i)
		{
			uint32_t prev = UINT32_MAX;

			while (st.size() != 0 && elements[i] < elements[st.top()])
			{
				prev = st.top();
				st.pop();
			}

			if (prev != UINT32_MAX)
			{
				nearest[prev] = i;
			}

			if (st.size() != 0)
			{
				nearest[i] = st.top();
			}

			st.push(i);
		}

		this->tree.resize(size);

		for (uint32_t i = 0; i < size; ++i)
		{
			if (nearest[i] == UINT32_MAX)
			{
				this->root = i;
				nearest[i] = i;
				continue;
			}

			this->tree[nearest[i]].push_back(i);
		}
	}

	vector<uint32_t> walk()
	{
		vector<uint32_t> order;
		queue<uint32_t> qu;

		qu.push(this->root);

		while (qu.size() != 0)
		{
			uint32_t top = qu.front();

			order.push_back(top);
			qu.pop();

			for (uint32_t i : this->tree[top])
			{
				qu.push(i);
			}
		}

		return order;
	}
};

struct merge_sort_tree
{
	using range_t = uint32_t;

	struct node
	{
		range_t begin, end;   // responsibility
		uint32_t left, right; // children

		node()
		{
			begin = 0, end = 0, left = 0, right = 0;
		}
	};

	struct vertex
	{
		vector<uint64_t> array;
		prefix_sums<uint64_t, op_add<uint64_t>> ps;

		vertex(uint32_t begin, uint32_t end)
		{
			this->array = vector<uint64_t>((end - begin) + 1);
		}

		void build()
		{
			this->ps = prefix_sums<uint64_t, op_add<uint64_t>>(this->array);
		}

		size_t size() const
		{
			return this->array.size();
		}

		uint64_t &operator[](uint32_t index)
		{
			return this->array[index];
		}

		uint64_t query(uint64_t value)
		{
			uint64_t index = bsa_gt(this->array, value);
			uint64_t count = this->array.size() - index;

			if (index == this->array.size())
			{
				return 0;
			}

			return this->ps.sum(index, this->array.size() - 1) - (count * value);
		}
	};

	vector<node> info;
	vector<vertex> tree;

	stack<uint32_t> st;
	stack<uint32_t> up;

	void _join(uint32_t index)
	{
		uint32_t left = this->info[index].left;
		uint32_t right = this->info[index].right;
		uint32_t pos_left = 0, pos_right = 0, pos_index = 0;

		while (pos_left < this->tree[left].size() && pos_right < this->tree[right].size())
		{
			if (this->tree[left][pos_left] <= this->tree[right][pos_right])
			{
				this->tree[index][pos_index++] = this->tree[left][pos_left++];
			}
			else
			{
				this->tree[index][pos_index++] = this->tree[right][pos_right++];
			}
		}

		while (pos_left < this->tree[left].size())
		{
			this->tree[index][pos_index++] = this->tree[left][pos_left++];
		}

		while (pos_right < this->tree[right].size())
		{
			this->tree[index][pos_index++] = this->tree[right][pos_right++];
		}

		this->tree[index].build();
	}

	void _create(uint32_t index)
	{
		node left = {}, right = {};

		// left
		left.begin = this->info[index].begin;
		left.end = (this->info[index].begin + this->info[index].end) / 2;

		this->info[index].left = this->info.size();
		this->info.push_back(left);
		this->tree.push_back({left.begin, left.end});

		// right
		right.begin = left.end + 1;
		right.end = this->info[index].end;

		this->info[index].right = this->info.size();
		this->info.push_back(right);
		this->tree.push_back({right.begin, right.end});
	}

	template <typename U>
	void _build(const vector<U> &elements)
	{
		node root = {};

		root.begin = 0;
		root.end = elements.size() - 1;
		root.left = 0;
		root.right = 0;

		this->info.push_back(root);
		this->tree.push_back({root.begin, root.end});
		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();

			this->st.pop();

			if (this->tree[index].size() == 1)
			{
				this->tree[index][0] = elements[this->info[index].begin];
				this->tree[index].build();
				continue;
			}

			this->_create(index);
			this->up.push(index);

			this->st.push(this->info[index].left);
			this->st.push(this->info[index].right);
		}

		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	template <typename U>
	merge_sort_tree(const vector<U> &elements)
	{
		this->_build(elements);
	}

	uint64_t query(range_t left, range_t right, uint64_t value)
	{
		uint64_t result = 0;

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			range_t current_left = this->info[index].begin;
			range_t current_right = this->info[index].end;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				result += this->tree[index].query(value);
				continue;
			}

			this->st.push(this->info[index].left);
			this->st.push(this->info[index].right);
		}

		return result;
	}
};
