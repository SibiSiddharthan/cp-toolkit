#include <string>
#include <vector>
#include <array>
#include <stack>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

uint64_t mods[2] = {4294967279, 4294967291};
uint64_t primes[4] = {73, 137, 101, 103};

vector<uint32_t> kmp_prefix(const string &pattern)
{
	uint32_t size = pattern.size();
	uint32_t i = 1, k = 0;

	vector<uint32_t> lps(size, 0);

	for (uint32_t i = 1; i < size; ++i)
	{
		while (k != 0 && pattern[k] != pattern[i])
		{
			k = lps[k - 1];
		}

		if (pattern[k] == pattern[i])
		{
			k += 1;
		}

		lps[i] = k;
	}

	return lps;
}

uint32_t kmp_search(vector<uint32_t> &lps, const string &pattern, const string &text)
{
	uint32_t j = 0;

	for (uint32_t i = 0; i < text.size(); ++i)
	{
		while (j != 0 && text[i] != pattern[j])
		{
			j = lps[j - 1];
		}

		if (text[i] == pattern[j])
		{
			j += 1;
		}

		if (j == pattern.size())
		{
			return (i + 1) - j;
		}
	}

	return UINT32_MAX;
}

vector<uint32_t> kmp_search_all(vector<uint32_t> &lps, const string &pattern, const string &text)
{
	uint32_t i = 0, j = 0;
	vector<uint32_t> matches;

	for (uint32_t i = 0; i < text.size(); ++i)
	{
		while (j != 0 && text[i] != pattern[j])
		{
			j = lps[j - 1];
		}

		if (text[i] == pattern[j])
		{
			j += 1;
		}

		if (j == pattern.size())
		{
			matches.push_back((i + 1) - j);
			j = lps[j - 1];
		}
	}

	return matches;
}

vector<uint32_t> z_prefix(const string &str)
{
	uint32_t size = str.size();
	uint32_t left = 0, right = 0;

	vector<uint32_t> prefix(size, 0);

	for (uint32_t i = 1; i < size; ++i)
	{
		if (i < right)
		{
			prefix[i] = MIN(right - i, prefix[i - left]);
		}

		while (i + prefix[i] < size && str[prefix[i]] == str[i + prefix[i]])
		{
			prefix[i] += 1;
		}

		if (i + prefix[i] > right)
		{
			left = i;
			right = i + prefix[i];
		}
	}

	return prefix;
}

struct suffix_automaton
{
	struct state
	{
		uint32_t length;
		uint32_t link;

		uint8_t terminal : 1;
		uint32_t set : 26;

		uint32_t next[26];

		uint32_t &operator[](uint32_t index)
		{
			return next[index];
		}
	};

	vector<state> states;

	uint32_t last = 0;
	uint32_t size = 0;

	suffix_automaton(const string &text)
	{
		this->last = this->node(0, UINT32_MAX);
		this->build(text);
	}

	uint32_t node(uint32_t length = 0, uint32_t link = UINT32_MAX)
	{
		this->states.push_back({length, link, 0, 0});
		fill_n(this->states.back().next, 26, UINT32_MAX);

		return this->size++;
	}

	void build(const string &text)
	{
		for (char ch : text)
		{
			uint32_t prev = this->last;
			uint32_t current = this->node(this->states[prev].length + 1);

			while (prev != UINT32_MAX && this->states[prev][ch - 'a'] == UINT32_MAX)
			{
				this->states[prev][ch - 'a'] = current;
				prev = this->states[prev].link;
			}

			if (prev == UINT32_MAX)
			{
				this->states[current].link = 0;
			}
			else
			{
				uint32_t temp = this->states[prev][ch - 'a'];

				if (this->states[prev].length + 1 == this->states[temp].length)
				{
					this->states[current].link = temp;
				}
				else
				{
					uint32_t clone = this->node();

					this->states[clone] = this->states[temp];
					this->states[clone].length = this->states[prev].length + 1;

					while (prev != UINT32_MAX && this->states[prev][ch - 'a'] == temp)
					{
						this->states[prev][ch - 'a'] = clone;
						prev = this->states[prev].link;
					}

					this->states[temp].link = clone;
					this->states[current].link = clone;
				}
			}

			this->last = current;
		}

		uint32_t current = this->last;

		while (current != UINT32_MAX)
		{
			this->states[current].terminal = 1;
			current = this->states[current].link;
		}
	}

	uint8_t operator()(const string &str)
	{
		uint32_t current = 0;

		for (char ch : str)
		{
			if (this->states[current][ch - 'a'] == UINT32_MAX)
			{
				return 0;
			}

			current = this->states[current][ch - 'a'];
		}

		return this->states[current].terminal;
	}
};
