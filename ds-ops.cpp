#include "cp.h"

template <typename T>
struct op_add
{
	T identity()
	{
		return 0;
	}

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
struct op_mul
{
	T identity()
	{
		return 1;
	}

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
struct op_min
{
	T identity()
	{
		return numeric_limits<T>::max();
	}

	T operator()(const T &a, const T &b) const
	{
		return MIN(a, b);
	}
};

template <typename T>
struct op_max
{
	T identity()
	{
		return numeric_limits<T>::min();
	}

	T operator()(const T &a, const T &b) const
	{
		return MAX(a, b);
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
struct op_modmul
{
	uint64_t mod = M;

	uint64_t identity()
	{
		return 1;
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
struct op_bitand
{
	T identity()
	{
		return numeric_limits<T>::max();
	}

	T operator()(const T &a, const T &b) const
	{
		return a & b;
	}
};

template <typename T>
struct op_bitor
{
	T identity()
	{
		return 0;
	}

	T operator()(const T &a, const T &b) const
	{
		return a | b;
	}
};

template <typename T>
struct op_bitxor
{
	T identity()
	{
		return 0;
	}

	T inverse(const T &a, const T &b) const
	{
		return a ^ b;
	}

	T operator()(const T &a, const T &b) const
	{
		return a ^ b;
	}
};
