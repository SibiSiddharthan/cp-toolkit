#include "cp.h"
#include "segtree.cpp"
#include "graph.cpp"

template <typename T>
struct seg_add
{
	T identity()
	{
		return 0;
	}

	T join(const T &a, const T &b) const
	{
		return a + b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct seg_mul
{
	T identity()
	{
		return 1;
	}

	T join(const T &a, const T &b) const
	{
		return a * b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct seg_min
{
	T identity()
	{
		return numeric_limits<T>::max();
	}

	T join(const T &a, const T &b) const
	{
		return MIN(a, b);
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct seg_max
{
	T identity()
	{
		return numeric_limits<T>::min();
	}

	T join(const T &a, const T &b) const
	{
		return MAX(a, b);
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <uint64_t M>
struct seg_modadd
{
	uint64_t mod = M;

	uint64_t identity()
	{
		return 0;
	}

	uint64_t join(uint64_t a, uint64_t b) const
	{
		return (a + b) % mod;
	}

	uint64_t assign(uint64_t a, [[maybe_unused]] uint32_t index) const
	{
		return a % mod;
	}
};

template <uint64_t M>
struct seg_modmul
{
	uint64_t mod = M;

	uint64_t identity()
	{
		return 1;
	}

	uint64_t join(uint64_t a, uint64_t b) const
	{
		return (a * b) % mod;
	}

	uint64_t assign(uint64_t a, [[maybe_unused]] uint32_t index) const
	{
		return a % mod;
	}
};

struct seg_modop
{
	uint64_t indentity;
	uint64_t mod;

	seg_modop(uint64_t mod) : mod(mod)
	{
	}
};

struct seg_gcd
{
	uint64_t identity()
	{
		return 0;
	}

	uint64_t join(uint64_t a, uint64_t b) const
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

	uint64_t assign(uint64_t a, [[maybe_unused]] uint32_t index) const
	{
		return a;
	}
};

template <typename T>
struct seg_bitand
{
	T identity()
	{
		return numeric_limits<T>::max();
	}

	T join(const T &a, const T &b) const
	{
		return a & b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct seg_bitor
{
	T identity()
	{
		return 0;
	}

	T operator()(const T &a, const T &b) const
	{
		return a | b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct seg_bitxor
{
	T identity()
	{
		return 0;
	}

	T operator()(const T &a, const T &b) const
	{
		return a ^ b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct range_minmax_node
{
	T value;
	uint32_t min_index;
	uint32_t max_index
};

template <typename T>
struct range_min_op
{
	range_minmax_node<T> identity()
	{
		return {numeric_limits<T>::max(), UINT32_MAX, 0};
	}

	range_minmax_node<T> join(const T &a, const T &b) const
	{
		range_minmax_node<T> result;

		if (a.value < b.value)
		{
			result = a;
		}
		else if (b.value < a.value)
		{
			result = b;
		}
		else
		{
			result.value = a.value;

			result.min_index = MIN(a.min_index, b.min_index);
			result.max_index = MAX(a.max_index, b.max_index);
		}

		return result;
	}

	template <typename U>
	range_minmax_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), index, index};
	}
};

template <typename T>
struct range_max_op
{
	range_minmax_node<T> identity()
	{
		return {numeric_limits<T>::min(), UINT32_MAX, 0};
	}

	range_minmax_node<T> join(const T &a, const T &b) const
	{
		range_minmax_node<T> result;

		if (a.value > b.value)
		{
			result = a;
		}
		else if (b.value > a.value)
		{
			result = b;
		}
		else
		{
			result.value = a.value;

			result.min_index = MIN(a.min_index, b.min_index);
			result.max_index = MAX(a.max_index, b.max_index);
		}

		return result;
	}

	template <typename U>
	range_minmax_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), index, index};
	}
};

template <typename T>
using range_min = simple_segment_tree<range_minmax_node<T>, range_min_op<T>>;

template <typename T>
using range_max = simple_segment_tree<range_minmax_node<T>, range_max_op<T>>;

template <typename T>
struct max_subarray_sum_node
{
	T total_sum;  // Total sum of segment
	T best_sum;   // Best sum of segment
	T max_prefix; // Max prefix sum of segment
	T max_suffix; // Max suffix sum of segment
};

template <typename T>
struct max_subarray_sum_op
{
	max_subarray_sum_node<T> identity()
	{
		return {0, 0, 0, 0};
	}

	max_subarray_sum_node<T> join(const max_subarray_sum_node<T> &left, const max_subarray_sum_node<T> &right) const
	{
		max_subarray_sum_node<T> result = {0, 0, 0, 0};

		result.total_sum = left.total_sum + right.total_sum;
		result.max_prefix = MAX(left.max_prefix, left.total_sum + right.max_prefix);
		result.max_suffix = MAX(right.max_suffix, right.total_sum + left.max_suffix);

		result.best_sum = MAX(left.best_sum, right.best_sum);
		result.best_sum = MAX(result.best_sum, left.max_suffix + right.max_prefix);

		return result;
	}

	template <typename U>
	max_subarray_sum_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), MAX(0, static_cast<T>(element)), MAX(0, static_cast<T>(element)), MAX(0, static_cast<T>(element))};
	}
};

struct tree_diameter_node
{
	uint32_t x, y;
};

struct tree_diameter_op
{
	lowest_common_ancestor &lca;

	tree_diameter_op(lowest_common_ancestor &lca) : lca(lca)
	{
	}

	tree_diameter_node identity() const
	{
		return {UINT32_MAX, UINT32_MAX};
	}

	tree_diameter_node join(const tree_diameter_node &left, const tree_diameter_node &right)
	{
		tree_diameter_node result = {UINT32_MAX, UINT32_MAX};
		array<uint32_t, 4> temp = {left.x, left.y, right.x, right.y};
		uint32_t diameter = 0;

		for (uint32_t i = 0; i < 4; ++i)
		{
			if (temp[i] == UINT32_MAX)
			{
				continue;
			}

			for (uint32_t j = i + 1; j < 4; ++j)
			{
				if (temp[j] == UINT32_MAX)
				{
					continue;
				}

				uint32_t dist = lca.distance(temp[i], temp[j]);

				if (dist >= diameter)
				{
					result = {temp[i], temp[j]};
					diameter = dist;
				}
			}
		}

		return result;
	}

	template <typename U>
	tree_diameter_node assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return {static_cast<uint32_t>(element), static_cast<uint32_t>(element)};
	}
};

// For lazy segment trees
struct node
{
};

struct lazy
{
};

struct seg_op
{
	// Identity element of node
	node identity()
	{
		return {};
	}

	// Join children and put the value in parent
	node join(const node &left, const node &right) const
	{
		node result;

		return result;
	}

	// Initialize the array
	// Keep the template to make the concept happy
	template <typename U>
	node assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return {};
	}

	// Apply an update to the node
	nodeT apply(const node &element, const lazy &update, uint32_t begin, uint32_t end)
	{
		node result = element;

		return result;
	}

	// Combine two lazy updates into one
	lazy compose(const lazy &current, const lazy &update)
	{
		lazy result = current;

		return result;
	}

	// Identity element of updates
	lazy reset()
	{
		return {};
	}
};
