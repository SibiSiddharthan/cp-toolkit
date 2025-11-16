#include <cstdint>
#include <vector>
#include <map>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MOD_1097 1000000007
#define MOD_1099 1000000009
#define MOD_FFT  998244353

uint64_t gcd(uint64_t a, uint64_t b)
{
	uint64_t t = 0;

	if (b > a)
	{
		t = a;
		a = b;
		b = t;
	}

	while (b != 0)
	{
		a = a % b;

		t = a;
		a = b;
		b = t;
	}

	return a;
}

vector<uint64_t> powers_n(uint64_t x, uint64_t count)
{
	vector<uint64_t> powers(count + 1, 0);

	powers[0] = 1;

	for (uint64_t i = 1; i <= count; ++i)
	{
		powers[i] = (powers[i - 1] * x) % MOD_1097;
	}

	return powers;
}

vector<vector<uint32_t>> factor_sieve(uint64_t n)
{
	vector<vector<uint32_t>> a(n + 1);

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		uint64_t j = 2;

		a[i].push_back(i);

		while ((i * j) <= n)
		{
			a[i * j].push_back(i);
			j++;
		}
	}

	return a;
}

vector<uint64_t> prime_sieve(uint64_t n)
{
	vector<uint8_t> v(n + 1, 0);
	vector<uint64_t> a;

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		if (v[i] == 0)
		{
			uint64_t j = 2;

			a.push_back(i);

			while ((i * j) <= n)
			{
				v[i * j] = 1;
				j++;
			}
		}
	}

	return a;
}

map<uint64_t, vector<uint64_t>, greater<uint64_t>> prime_factor_sieve(uint64_t n)
{
	vector<uint8_t> v(n + 1, 0);
	map<uint64_t, vector<uint64_t>, greater<uint64_t>> a;

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		if (v[i] == 0)
		{
			uint64_t j = 2;

			a[i].push_back(i);

			while ((i * j) <= n)
			{
				v[i * j] = 1;
				a[i].push_back(i * j);
				j++;
			}
		}
	}

	return a;
}

uint64_t modexp(uint64_t a, uint64_t p, uint64_t m)
{
	uint64_t result = 1;
	uint64_t temp = a;

	if (p & 1)
	{
		result = a;
	}

	p >>= 1;

	while (p != 0)
	{
		temp = (temp * temp) % m;

		if (p & 1)
		{
			result = (result * temp) % m;
		}

		p >>= 1;
	}

	return result;
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

uint64_t ncr(uint64_t n, uint64_t r)
{
	uint64_t result = 1;
	uint64_t numerator = MAX(r, n - r);
	uint64_t denominator = MIN(r, n - r);
	uint64_t divisor = 2;

	if (divisor > denominator)
	{
		divisor = 1;
	}

	while (n != numerator)
	{
		result *= n--;

		if (divisor != 1)
		{
			while (result % divisor == 0)
			{
				result /= divisor;
				divisor += 1;

				if (divisor > denominator)
				{
					divisor = 1;
					break;
				}
			}
		}
	}

	return result;
}

uint64_t modncr(uint64_t n, uint64_t r, uint64_t m)
{
	uint64_t result = 1;
	uint64_t numerator = MAX(r, n - r);
	uint64_t denominator = MIN(r, n - r);
	uint64_t divisor = 2;

	while (n != numerator)
	{
		result = (result * (n--)) % m;
	}

	while (divisor <= denominator)
	{
		result = (result * modinv(divisor++, m)) % m;
	}

	return result;
}

vector<uint64_t> factorial_precompute(uint64_t n, uint64_t m)
{
	vector<uint64_t> factorials(n + 1, 0);

	factorials[0] = 1;
	factorials[1] = 1;

	for (uint64_t i = 2; i <= n; ++i)
	{
		factorials[i] = (factorials[i - 1] * i) % m;
	}

	return factorials;
}

vector<uint64_t> inverses_precompute(uint64_t n, uint64_t m)
{
	vector<uint64_t> inverses(n + 1, 0);

	inverses[0] = 1;
	inverses[1] = 1;

	for (uint64_t i = 2; i <= n; ++i)
	{
		inverses[i] = (inverses[i - 1] * modinv(i, m)) % m;
	}

	return inverses;
}

uint64_t fast_modncr(vector<uint64_t> &factorials, vector<uint64_t> &inverses, uint64_t n, uint64_t r, uint64_t m)
{
	return (factorials[n] * ((inverses[r] * inverses[n - r]) % m)) % m;
}

uint64_t isqrt(uint64_t x)
{
	uint64_t left = 1, right = x;
	uint64_t middle = 0;
	uint64_t estimate = 0;
	uint64_t temp = 0;

	while (left <= right)
	{
		middle = (left + right) / 2;
		temp = middle * middle;

		if (temp == x)
		{
			return middle;
		}
		else if (temp > x)
		{
			if (middle == 0)
			{
				break;
			}

			right = middle - 1;
		}
		else
		{
			estimate = middle;
			left = middle + 1;
		}
	}

	return estimate;
}
