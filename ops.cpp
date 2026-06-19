#include "cp.h"

template <typename T>
struct op_add
{
	T identity()
	{
		return 0;
	}

	T join(const T &a, const T &b) const
	{
		return a + b;
	}

	T inverse(const T &a, const T &b) const
	{
		return a - b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct op_mul
{
	T identity()
	{
		return 1;
	}

	T join(const T &a, const T &b) const
	{
		return a * b;
	}

	T inverse(const T &a, const T &b) const
	{
		return a / b;
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

template <typename T>
struct op_min
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
struct op_max
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
struct op_modadd
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

	uint64_t inverse(uint64_t a, uint64_t b) const
	{
		return ((mod + a) - b) % mod;
	}

	uint64_t assign(uint64_t a, [[maybe_unused]] uint32_t index) const
	{
		return a % mod;
	}
};

template <uint64_t M>
struct op_modmul
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

	uint64_t inverse(uint64_t a, uint64_t b) const
	{
		return (a * modinv(b, mod)) % mod;
	}

	uint64_t assign(uint64_t a, [[maybe_unused]] uint32_t index) const
	{
		return a % mod;
	}
};

struct op_modop
{
	uint64_t mod;

	op_modop(uint64_t mod) : mod(mod)
	{
	}

	uint64_t identity()
	{
		return 0;
	}
};

struct op_gcd
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
struct op_bitand
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
struct op_bitor
{
	T identity()
	{
		return 0;
	}

	T join(const T &a, const T &b) const
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
struct op_bitxor
{
	T identity()
	{
		return 0;
	}

	T join(const T &a, const T &b) const
	{
		return a ^ b;
	}

	T inverse(const T &a, const T &b) const
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
	uint32_t count;
	uint32_t min_index;
	uint32_t max_index;
};

template <typename T>
struct range_min_op
{
	range_minmax_node<T> identity()
	{
		return {numeric_limits<T>::max(), 0, UINT32_MAX, 0};
	}

	range_minmax_node<T> join(const range_minmax_node<T> &left, const range_minmax_node<T> &right) const
	{
		range_minmax_node<T> result;

		if (left.value < right.value)
		{
			result = left;
		}
		else if (right.value < left.value)
		{
			result = right;
		}
		else
		{
			result.value = left.value;
			result.count = left.count + right.count;
			result.min_index = MIN(left.min_index, right.min_index);
			result.max_index = MAX(left.max_index, right.max_index);
		}

		return result;
	}

	template <typename U>
	range_minmax_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), 1, index, index};
	}
};

template <typename T>
struct range_max_op
{
	range_minmax_node<T> identity()
	{
		return {numeric_limits<T>::min(), 0, UINT32_MAX, 0};
	}

	range_minmax_node<T> join(const range_minmax_node<T> &left, const range_minmax_node<T> &right) const
	{
		range_minmax_node<T> result;

		if (left.value > right.value)
		{
			result = left;
		}
		else if (right.value > left.value)
		{
			result = right;
		}
		else
		{
			result.value = left.value;
			result.count = left.count + right.count;
			result.min_index = MIN(left.min_index, right.min_index);
			result.max_index = MAX(left.max_index, right.max_index);
		}

		return result;
	}

	template <typename U>
	range_minmax_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), 1, index, index};
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

template <typename T>
struct range_add_node
{
	T value;
};

template <typename T>
struct range_add_op
{
	range_add_node<T> compose(const range_add_node<T> &current, const range_add_node<T> &update)
	{
		return {current.value + update.value};
	}

	range_add_node<T> reset()
	{
		return {0};
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

// For simple segment trees
struct node
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
	node apply(const node &element, const lazy &update, uint32_t begin, uint32_t end)
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
