#include <cstdint>
#include <vector>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

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


