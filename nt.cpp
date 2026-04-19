#include <cstdint>
#include <vector>
#include <map>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MOD_1097 1000000007
#define MOD_1099 1000000009
#define MOD_FFT  998244353

struct fft_prime
{
	uint64_t mod;
	uint64_t power;
	uint64_t root;
	uint64_t inverse;
};

fft_prime fft_998244353 = {998244353, 23, 15311432, 469870224}; // 119*2^23+1
fft_prime fft_167772161 = {167772161, 25, 243, 114609789};      // 5*2^25+1
fft_prime fft_469762049 = {469762049, 26, 2187, 410692747};     // 7*2^26+1

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

uint64_t isqrt_ceil(uint64_t x)
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
			estimate = middle;
			right = middle - 1;
		}
		else
		{
			left = middle + 1;
		}
	}

	return estimate;
}

uint64_t isqrt_floor(uint64_t x)
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

template <uint64_t MODULO>
struct fast_modncr
{
	vector<uint64_t> factorials;
	vector<uint64_t> inverses;

	fast_modncr(uint64_t n)
	{
		precompute(n);
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

	void precompute(uint64_t n)
	{
		this->factorials = vector<uint64_t>(n + 1, 0);
		this->inverses = vector<uint64_t>(n + 1, 0);

		this->factorials[0] = 1;
		this->factorials[1] = 1;

		for (uint64_t i = 2; i <= n; ++i)
		{
			this->factorials[i] = (this->factorials[i - 1] * i) % MODULO;
		}

		this->inverses[0] = 1;
		this->inverses[1] = 1;

		this->inverses[n] = this->modinv(this->factorials[n], MODULO);

		for (uint64_t i = n - 1; i > 1; --i)
		{
			this->inverses[i] = (this->inverses[i + 1] * (i + 1)) % MODULO;
		}
	}

	void factorial_precompute(uint64_t n)
	{
		this->factorials = vector<uint64_t>(n + 1, 0);

		this->factorials[0] = 1;
		this->factorials[1] = 1;

		for (uint64_t i = 2; i <= n; ++i)
		{
			this->factorials[i] = (this->factorials[i - 1] * i) % MODULO;
		}
	}

	void inverses_precompute(uint64_t n)
	{
		this->inverses = vector<uint64_t>(n + 1, 0);

		this->inverses[0] = 1;
		this->inverses[1] = 1;

		for (uint64_t i = 2; i <= n; ++i)
		{
			this->inverses[i] = (this->inverses[i - 1] * this->modinv(i, MODULO)) % MODULO;
		}
	}

	uint64_t operator()(uint64_t n, uint64_t r)
	{
		return (this->factorials[n] * ((this->inverses[r] * this->inverses[n - r]) % MODULO)) % MODULO;
	}
};

struct ntt
{
	fft_prime info;
	uint64_t mod;
	uint64_t size;
	uint32_t lgsz;
	vector<uint64_t> roots;
	vector<uint64_t> inverses;

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

	uint32_t bitreverse(uint32_t n)
	{
		uint32_t res = 0;

		for (uint32_t b = 0; b < this->lgsz; ++b)
		{
			res |= ((n >> b) & 1) << (this->lgsz - (b + 1));
		}

		return res;
	}

	vector<uint64_t> fft(vector<uint64_t> &elements, vector<uint64_t> &roots)
	{
		vector<uint64_t> result = vector<uint64_t>(this->size, 0);

		for (uint32_t i = 0; i < elements.size(); ++i)
		{
			result[this->bitreverse(i)] = elements[i];
		}

		for (uint64_t step = 2; step <= this->size; step <<= 1)
		{
			uint64_t root = roots[__builtin_ctzll(step)];

			for (uint64_t part = 0; part < this->size; part += step)
			{
				uint64_t current = 1;

				for (uint64_t index = 0; index < step / 2; index += 1)
				{
					uint64_t u = result[part + index];
					uint64_t v = (result[part + index + (step / 2)] * current) % this->mod;

					result[part + index] = (u + v) % this->mod;
					result[part + index + (step / 2)] = ((this->mod + u) - v) % this->mod;

					current = (current * root) % this->mod;
				}
			}
		}

		return result;
	}

	vector<uint64_t> dft(vector<uint64_t> &elements)
	{
		return fft(elements, roots);
	}

	vector<uint64_t> idft(vector<uint64_t> &elements)
	{
		uint64_t inv = this->modinv(this->size, this->mod);
		vector<uint64_t> result = fft(elements, inverses);

		for (uint64_t i = 0; i < this->size; ++i)
		{
			result[i] = (result[i] * inv) % this->mod;
		}

		return result;
	}

	ntt() {}

	ntt(const fft_prime &info)
	{
		this->info = info;
		this->mod = info.mod;

		this->roots = vector<uint64_t>(this->info.power + 1);
		this->inverses = vector<uint64_t>(this->info.power + 1);

		this->roots[0] = 1;
		this->inverses[0] = 1;

		this->roots[this->info.power] = this->info.root;
		this->inverses[this->info.power] = this->info.inverse;

		for (uint32_t i = this->info.power - 1; i != 0; --i)
		{
			this->roots[i] = (this->roots[i + 1] * this->roots[i + 1]) % this->mod;
			this->inverses[i] = (this->inverses[i + 1] * this->inverses[i + 1]) % this->mod;
		}
	}

	vector<uint64_t> operator()(vector<uint64_t> &a, vector<uint64_t> &b)
	{
		uint64_t total = a.size() + b.size();

		this->size = (uint64_t)1 << ((64 - (__builtin_clzll(total) + 1)) + (__builtin_popcountll(total) != 1));
		this->lgsz = __builtin_ctzll(this->size);

		vector<uint64_t> tc(this->size);

		auto ta = dft(a);
		auto tb = dft(b);

		for (uint64_t i = 0; i < this->size; ++i)
		{
			tc[i] = (ta[i] * tb[i]) % this->mod;
		}

		auto result = idft(tc);

		return result;
	}
};

struct ntt_crt
{
	fft_prime info_a;
	fft_prime info_b;

	ntt ntt_a;
	ntt ntt_b;

	uint64_t inv;
	uint64_t mod;

	uint64_t modinv(uint64_t a, uint64_t m)
	{
		uint64_t b = m;
		uint64_t q = 0, r = 0;
		uint64_t u = 1, v = 0, t = 0;

		a %= m;

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

	ntt_crt(const fft_prime &info_a, const fft_prime &info_b, uint64_t mod)
	{
		this->info_a = info_a;
		this->info_b = info_b;

		this->ntt_a = ntt(info_a);
		this->ntt_b = ntt(info_b);

		this->inv = this->modinv(this->info_a.mod, this->info_b.mod);
		this->mod = mod;
	}

	vector<uint64_t> operator()(vector<uint64_t> &a1, vector<uint64_t> &a2, vector<uint64_t> &b1, vector<uint64_t> &b2)
	{
		vector<uint64_t> tc_a = this->ntt_a(a1, a2);
		vector<uint64_t> tc_b = this->ntt_b(b1, b2);
		vector<uint64_t> result(tc_a.size());
		uint64_t temp = 0;

		for (uint64_t i = 0; i < result.size(); ++i)
		{
			temp = ((this->info_b.mod + tc_b[i]) - tc_a[i]) % this->info_b.mod;
			temp = (temp * this->inv) % this->info_b.mod;

			result[i] = ((temp * this->info_a.mod) + tc_a[i]) % this->mod;
		}

		return result;
	}
};
