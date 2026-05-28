#include "cp.h"

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
struct seg_range_minmax_node
{
	T value;
	uint32_t min_index;
	uint32_t max_index
};

template <typename T>
struct seg_range_min_op
{
	seg_range_minmax_node<T> identity()
	{
		return {numeric_limits<T>::max(), UINT32_MAX, 0};
	}

	seg_range_minmax_node<T> join(const T &a, const T &b) const
	{
		seg_range_minmax_node<T> result;

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
	seg_range_minmax_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), index, index};
	}
};

template <typename T>
struct seg_range_max_op
{
	seg_range_minmax_node<T> identity()
	{
		return {numeric_limits<T>::min(), UINT32_MAX, 0};
	}

	seg_range_minmax_node<T> join(const T &a, const T &b) const
	{
		seg_range_minmax_node<T> result;

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
	seg_range_minmax_node<T> assign(const U &element, uint32_t index) const
	{
		return {static_cast<T>(element), index, index};
	}
};

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
