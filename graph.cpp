#include <vector>
#include <utility>
#include <stack>
#include <queue>

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

vector<vector<uint32_t>> build_edge_graph(vector<pair<uint32_t, uint32_t>> &edges, uint32_t count)
{
	vector<vector<uint32_t>> graph(count);

	for (uint32_t i = 0; i < edges.size(); ++i)
	{
		graph[edges[i].first].push_back(i);
		graph[edges[i].second].push_back(i);
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

vector<uint32_t> dfs_cycle(vector<vector<uint32_t>> &graph, vector<pair<uint32_t, uint32_t>> &edges)
{
	vector<uint32_t> vertex_visited(graph.size(), 0);
	vector<uint32_t> edge_visited(edges.size(), 0);
	vector<uint32_t> parent(graph.size(), UINT32_MAX);
	vector<uint32_t> cycle;
	stack<uint32_t> s;

	uint32_t index = 0;

	vertex_visited[index] = 1;
	parent[index] = index;
	s.push(index);

	while (s.size() != 0)
	{
		uint32_t source = s.top();
		uint32_t count = 0;

		for (uint32_t i = 0; i < graph[source].size(); ++i)
		{
			uint32_t destination = 0;
			uint32_t edge = graph[source][i];

			if (edge_visited[edge])
			{
				continue;
			}

			if (source == edges[edge].first)
			{
				destination = edges[edge].second;
			}
			else
			{
				destination = edges[edge].first;
			}

			edge_visited[edge] = 1;

			// Cycle detected
			if (vertex_visited[destination])
			{
				if (parent[source] != destination)
				{
					uint32_t node = source;

					do
					{
						cycle.push_back(node);
						node = parent[node];

					} while (node != destination);

					cycle.push_back(node);

					return cycle;
				}
			}

			// New vertex
			s.push(destination);
			vertex_visited[destination] = 1;
			parent[destination] = source;
			count += 1;

			break;
		}

		if (count == 0)
		{
			s.pop();
		}
	}

	return cycle;
}

vector<uint32_t> bfs(vector<vector<uint32_t>> &graph, uint32_t index)
{
	vector<uint32_t> distances(graph.size(), 0);
	vector<uint32_t> visited(graph.size(), 0);
	queue<uint32_t> q;

	q.push(index);
	visited[index] = 1;
	distances[index] = 0;

	while (q.size() != 0)
	{
		uint32_t source = q.front();

		for (uint32_t i = 0; i < graph[source].size(); ++i)
		{
			uint32_t destination = graph[source][i];

			if (visited[destination] == 0)
			{
				q.push(destination);
				visited[destination] = 1;
				distances[destination] = distances[source] + 1;
			}
		}

		q.pop();
	}

	return distances;
}

vector<uint64_t> dijkstra(vector<vector<pair<uint32_t, uint64_t>>> &graph, vector<uint64_t> &slowness, uint32_t index, uint32_t n)
{
	priority_queue<pair<uint64_t, uint32_t>, vector<pair<uint64_t, uint32_t>>, greater<pair<uint64_t, uint32_t>>> pq;

	vector<uint64_t> distances(n, UINT64_MAX);

	distances[index] = 0;
	pq.push({0, index});

	while (pq.size() != 0)
	{
		uint32_t source = pq.top().second;
		uint64_t dist = pq.top().first;

		pq.pop();

		if (dist != distances[source])
		{
			continue;
		}

		for (uint32_t i = 0; i < graph[source].size(); ++i)
		{
			uint32_t destination = graph[source][i].first;
			uint64_t weight = graph[source][i].second;

			if (weight + distances[source] < distances[destination])
			{
				distances[destination] = weight + distances[source];
				pq.push({distances[destination], destination});
			}
		}
	}

	return distances;
}
