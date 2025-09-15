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

struct undirected_dfs
{
	uint32_t node_count;
	uint32_t edge_count;

	vector<uint32_t> nodes;
	vector<pair<uint32_t, uint32_t>> edges;
	vector<vector<uint32_t>> graph;

	vector<uint32_t> node_visits;
	vector<uint32_t> edge_visits;

	vector<uint32_t> node_times;
	vector<vector<uint32_t>> cycles;

	undirected_dfs(vector<uint32_t> &nodes, vector<pair<uint32_t, uint32_t>> &edges)
	{
		this->nodes = nodes;
		this->edges = edges;

		this->node_count = nodes.size();
		this->edge_count = edges.size();

		this->graph = build_undirected_graph(this->edges, this->node_count);

		this->node_visits = vector<uint32_t>(this->node_count, 0);
		this->edge_visits = vector<uint32_t>(this->edge_count, 0);
	}

	void dfs()
	{
	}
};

void dfs(vector<vector<uint32_t>> &graph, vector<uint32_t> &visited, vector<int64_t> pos, uint32_t index)
{
	stack<uint32_t> s;

	visited[index] = 1;
	s.push(index);
	pos[index] = 0;

	while (s.size() != 0)
	{
		uint32_t top = s.top();
		uint32_t count = 0;

		for (uint32_t j = 0; j < graph[top].size(); ++j)
		{
			if (visited[graph[top][j].first] == 0)
			{
				visited[graph[top][j].first] = 1;
				s.push(graph[top][j].first);
				pos[graph[top][j].first] = pos[top] + graph[top][j].second;
				count += 1;
			}
			else
			{
				if (pos[graph[top][j].first] != pos[top] + graph[top][j].second)
				{
					return;
				}
			}
		}

		if (count == 0)
		{
			s.pop();
		}
	}
}
