#include <vector>
#include <utility>
#include <stack>

using namespace std;

vector<vector<uint32_t>> build_undirected_graph(vector<pair<uint32_t, uint32_t>> &edges, uint32_t count)
{
	vector<vector<uint32_t>> graph(count);

	for (uint32_t i = 0; i < edges.size(); ++i)
	{
		graph[edges[i].first].push_back(edges[i].second);
		graph[edges[i].second].push_back(edges[i].first);
	}

	return graph;
}

