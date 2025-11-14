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
