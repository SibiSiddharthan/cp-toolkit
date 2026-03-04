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

// Sieves

// Get all prime numbers between 1 to n
vector<uint64_t> prime_sieve(uint64_t n)
{
	vector<uint8_t> seen(n + 1, 0);
	vector<uint64_t> primes;

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		if (seen[i] == 0)
		{
			uint64_t j = 2;

			primes.push_back(i);

			while ((i * j) <= n)
			{
				seen[i * j] = 1;
				j++;
			}
		}
	}

	return primes;
}

// Get all factors of numbers from 1 to n
auto factor_sieve(uint64_t n)
{
	vector<vector<uint32_t>> factors(n + 1);

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		uint64_t j = 2;

		factors[i].push_back(i);

		while ((i * j) <= n)
		{
			factors[i * j].push_back(i);
			j++;
		}
	}

	return factors;
}

// Get all prime factors of numbers from 1 to n
auto prime_factor_sieve(uint64_t n)
{
	vector<uint8_t> seen(n + 1, 0);
	map<uint64_t, vector<uint64_t>, greater<uint64_t>> factors;

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		if (seen[i] == 0)
		{
			uint64_t j = 2;

			factors[i].push_back(i);

			while ((i * j) <= n)
			{
				seen[i * j] = 1;
				factors[i * j].push_back(i);
				j++;
			}
		}
	}

	return factors;
}

// Get all combinations of prime factors of numbers from 1 to n
auto pie_sieve(uint64_t n)
{
	vector<uint8_t> seen(n + 1, 0);
	vector<vector<uint64_t>> prime_factors(n + 1);
	vector<vector<uint64_t>> prime_combinations(n + 1);

	for (uint64_t i = 2; i < n + 1; ++i)
	{
		if (seen[i] == 0)
		{
			uint64_t j = 2;

			prime_factors[i].push_back(i);

			while ((i * j) <= n)
			{
				seen[i * j] = 1;
				prime_factors[i * j].push_back(i);
				j++;
			}
		}
	}

	for (uint64_t i = 2; i <= n; ++i)
	{
		vector<uint64_t> combinations;

		if (prime_factors[i].size() == 0)
		{
			continue;
		}

		for (uint64_t j = 1; j < (1 << prime_factors[i].size()); ++j)
		{
			uint64_t value = 1;

			for (uint64_t k = 0; k < prime_factors[i].size(); ++k)
			{
				if (j & (1 << k))
				{
					value *= prime_factors[i][k];
				}
			}

			combinations.push_back(value);
		}

		prime_combinations[i] = combinations;
	}

	return make_pair(prime_factors, prime_combinations);
}

auto count_coprimes(vector<uint32_t> &elems, uint32_t max)
{
	auto [primes, combinations] = pie_sieve(max);
	vector<uint32_t> counts(max + 1, 0);
	vector<uint32_t> result(elems.size());

	for (uint32_t i = 0; i < elems.size(); ++i)
	{
		for (auto j : combinations[elems[i]])
		{
			counts[j] += 1;
		}
	}

	for (uint32_t i = 0; i < elems.size(); ++i)
	{
		uint64_t count = 0;

		for (auto j : combinations[elems[i]])
		{
			counts[j] -= 1;
		}

		for (uint32_t j = 1; j < (1 << primes[elems[i]].size()); ++j)
		{
			uint64_t value = 1;

			for (uint32_t k = 0; k < primes[elems[i]].size(); ++k)
			{
				if (j & (1 << k))
				{
					value *= primes[elems[i]][k];
				}
			}

			// Principle of Inclusion and Exclusion
			if (__builtin_popcountll(j) % 2 != 0)
			{
				count += counts[value];
			}
			else
			{
				count -= counts[value];
			}
		}

		for (auto j : combinations[elems[i]])
		{
			counts[j] += 1;
		}

		result[i] = count;
	}

	return result;
}

vector<uint64_t> powers_mod(uint64_t base, uint64_t mod, uint64_t count)
{
	vector<uint64_t> powers(count + 1, 0);

	powers[0] = 1;

	for (uint64_t i = 1; i <= count; ++i)
	{
		powers[i] = (powers[i - 1] * base) % mod;
	}

	return powers;
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
