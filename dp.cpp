#include <vector>
#include <utility>
#include <string>
#include <algorithm>

#include "graph.cpp"

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

	return *max_element(dp.begin(), dp.end());
}

string lcs(string &a, string &b)
{
	uint32_t n = a.size();
	uint32_t m = b.size();

	vector<vector<uint32_t>> dp(n + 1, vector<uint32_t>(m + 1, 0));
	string common;

	for (uint32_t i = 1; i <= n; ++i)
	{
		for (uint32_t j = 1; j <= m; ++j)
		{
			if (a[i - 1] == b[j - 1])
			{
				dp[i][j] = dp[i - 1][j - 1] + 1;
			}
			else
			{
				dp[i][j] = MAX(dp[i - 1][j], dp[i][j - 1]);
			}
		}
	}

	uint32_t x = n, y = m;

	while (x != 0 && y != 0)
	{
		if (a[x - 1] == b[y - 1])
		{
			common += a[x - 1];

			x -= 1;
			y -= 1;
		}
		else
		{
			if (dp[x - 1][y] >= dp[x][y - 1])
			{
				x -= 1;
			}
			else
			{
				y -= 1;
			}
		}
	}

	reverse(common.begin(), common.end());

	return common;
}

vector<uint32_t> dag_longest_path(directed_graph &dag)
{
	vector<uint32_t> order = dfs_sort(dag);
	vector<uint8_t> visited(dag.size(), 0);
	vector<uint32_t> dp(dag.size(), 1);

	for (uint32_t i = dag.size(); i != 0; --i)
	{
		uint32_t vertex = order[i - 1];

		if (visited[vertex])
		{
			continue;
		}

		for (auto [v, e] : dag[vertex])
		{
			dp[vertex] = MAX(dp[vertex], 1 + dp[v]);
		}
	}

	return dp;
}
