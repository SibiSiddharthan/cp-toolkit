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
