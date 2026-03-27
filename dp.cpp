#include <vector>
#include <utility>
#include <string>
#include <algorithm>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

uint64_t knapsack(vector<pair<uint64_t, uint64_t>> &a, uint32_t w)
{
	vector<uint64_t> dp(w + 1, 0);

	for (uint32_t i = 0; i < a.size(); ++i)
	{
		for (uint32_t j = w; j >= a[i].first; --j)
		{
			dp[j] = MAX(dp[j], dp[j - a[i].first] + a[i].second);
		}
	}

	return *max_element(dp.begin(),dp.end());
}
