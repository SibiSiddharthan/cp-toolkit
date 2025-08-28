#include <cstdint>
#include <vector>

using namespace std;

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
