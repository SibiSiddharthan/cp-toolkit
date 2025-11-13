#include <string>
#include <vector>

using namespace std;

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
