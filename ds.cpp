#include "cp.h"

template <typename O, typename T>
concept commutative_operator = requires(O op, T a, T b) {
	{ op(a, b) } -> std::same_as<T>;
	{ op.inverse(a, b) } -> std::same_as<T>;
};

template <typename O, typename T>
concept binary_operator = requires(O op, T a, T b) {
	{ op(a, b) } -> std::same_as<T>;
};

template <typename O, typename T>
concept must_reduce = requires(O op, T a) {
	{ op.reduce(a) } -> std::same_as<T>;
};

namespace ops {

template <typename T>
struct add
{
	T identity = 0;

	T inverse(const T &a, const T &b) const
	{
		return a - b;
	}

	T operator()(const T &a, const T &b) const
	{
		return a + b;
	}
};

template <typename T>
struct mul
{
	T identity = 1;

	T inverse(const T &a, const T &b) const
	{
		return a / b;
	}

	T operator()(const T &a, const T &b) const
	{
		return a * b;
	}
};

template <typename T>
struct min
{
	T identity = numeric_limits<T>::max();

	T operator()(const T &a, const T &b) const
	{
		return MIN(a, b);
	}
};

template <typename T>
struct max
{
	T identity = numeric_limits<T>::min();

	T operator()(const T &a, const T &b) const
	{
		return MAX(a, b);
	}
};

template <uint64_t M>
struct mod_add
{
	uint64_t identity = 0;
	uint64_t mod = M;

	uint64_t reduce(uint64_t a) const
	{
		return a % mod;
	}

	uint64_t inverse(uint64_t a, uint64_t b) const
	{
		return ((mod + a) - b) % mod;
	}

	uint64_t operator()(uint64_t a, uint64_t b) const
	{
		return (a + b) % mod;
	}
};

template <uint64_t M>
struct mod_mul
{
	uint64_t identity = 1;
	uint64_t mod = M;

	uint64_t modinv(uint64_t a, uint64_t m)
	{
		uint64_t b = m;
		uint64_t q = 0, r = 0;
		uint64_t u = 1, v = 0, t = 0;

		do
		{
			q = b / a;
			r = b % a;

			t = ((v + m) - ((u * q) % m)) % m;

			b = a;
			a = r;

			v = u;
			u = t;

		} while (r > 0);

		return v;
	}

	uint64_t reduce(uint64_t a) const
	{
		return a % mod;
	}

	uint64_t inverse(uint64_t a, uint64_t b) const
	{
		return (a * modinv(b, mod)) % mod;
	}

	uint64_t operator()(uint64_t a, uint64_t b) const
	{
		return (a * b) % mod;
	}
};

struct mod_op
{
	uint64_t indentity;
	uint64_t mod;

	mod_op(uint64_t mod) : mod(mod)
	{
	}
};

struct gcd
{
	uint64_t identity = 0;

	uint64_t operator()(uint64_t a, uint64_t b) const
	{
		uint64_t t = 0;

		while (b != 0)
		{
			t = a % b;
			a = b;
			b = t;
		}

		return a;
	}
};

template <typename T>
struct bit_and
{
	T identity = numeric_limits<T>::max();

	T operator()(const T &a, const T &b) const
	{
		return a & b;
	}
};

template <typename T>
struct bit_or
{
	T identity = 0;

	T operator()(const T &a, const T &b) const
	{
		return a | b;
	}
};

template <typename T>
struct bit_xor
{
	T identity = 0;

	T inverse(const T &a, const T &b) const
	{
		return a ^ b;
	}

	T operator()(const T &a, const T &b) const
	{
		return a ^ b;
	}
};

} // namespace ops

template <typename T, class O>
	requires binary_operator<O, T>
struct prefix_sums
{
	vector<T> prefix;
	O op;

	template <typename... args>
	prefix_sums(uint32_t size, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->prefix = vector<T>(size, 0);
	}

	template <typename... args>
	prefix_sums(const vector<T> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->prefix = elements;
		this->build();
	}

	T &operator[](uint32_t index)
	{
		return this->prefix[index];
	}

	void build()
	{
		T sum = 0;
		uint32_t size = this->prefix.size();

		for (uint32_t i = 0; i < size; ++i)
		{
			if constexpr (must_reduce<O, T>)
			{
				this->prefix[i] =  this->op.reduce(this->prefix[i]);
			}

			sum = this->op(sum, this->prefix[i]);
			this->prefix[i] = sum;
		}
	}

	T sum(uint32_t left, uint32_t right)
	{
		return left != 0 ? this->op.inverse(this->prefix[right], this->prefix[left - 1]) : this->prefix[right];
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
		return this->op(a, b);
	}

	T _reduce(const T &a)
	{
		if constexpr (must_reduce<O, T>)
		{
			return this->op.reduce(a);
		}

		return a;
	}

	disjoint_sparse_table()
	{
	}

	template <typename... args>
	disjoint_sparse_table(const vector<T> &elements, args &&...arg) : op(std::forward<args>(arg)...)
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
					this->table[i][middle] = this->_reduce(elements[middle]);
					begin = end + 1;

					continue;
				}

				this->table[i][middle] = this->_reduce(elements[middle]);

				for (uint32_t j = middle - 1; j >= begin && j < this->size; --j)
				{
					this->table[i][j] = this->_join(this->table[i][j + 1], this->_reduce(elements[j]));
				}

				this->table[i][middle + 1] = this->_reduce(elements[middle + 1]);

				for (uint32_t j = middle + 2; j <= end; ++j)
				{
					this->table[i][j] = this->_join(this->table[i][j - 1], this->_reduce(elements[j]));
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

template <typename T, class O>
	requires commutative_operator<O, T>
struct fenwick_tree
{
	vector<T> tree;
	uint32_t size = 0;

	O op;

	template <typename... args>
	fenwick_tree(const vector<T> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->tree = elements;
		this->size = elements.size();

		if constexpr (must_reduce<O, T>)
		{
			for (uint32_t i = 0; i < this->size; ++i)
			{
				this->tree[i] = this->op.reduce(this->tree[i]);
			}
		}

		for (uint32_t i = 0; i < this->size; ++i)
		{
			uint32_t above = i + ((uint32_t)1 << __builtin_ctzll(i + 1));

			if (above < this->size)
			{
				this->tree[above] = this->op(this->tree[above], this->tree[i]);
			}
		}
	}

	T _sum(uint32_t index)
	{
		T sum = this->op.identity;
		index += 1;

		while (index != 0)
		{
			sum = this->op(sum, tree[index - 1]);
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

		if constexpr (must_reduce<O, T>)
		{
			value = this->op.reduce(value);
		}

		while (index < size)
		{
			this->tree[index] = this->op.inverse(this->op(this->tree[index], value), old);
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}

	void increase(uint32_t index, T value)
	{
		if constexpr (must_reduce<O, T>)
		{
			value = this->op.reduce(value);
		}

		while (index < size)
		{
			this->tree[index] = this->op(this->tree[index], value);
			index += (uint32_t)1 << __builtin_ctzll(index + 1);
		}
	}

	void decrease(uint32_t index, T value)
	{
		if constexpr (must_reduce<O, T>)
		{
			value = this->op.reduce(value);
		}

		while (index < size)
		{
			this->tree[index] = this->op.inverse(this->tree[index], value);
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
