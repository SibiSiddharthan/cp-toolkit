#include <vector>
#include <utility>
#include <stack>
#include <queue>
#include <array>
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define DIFF(a, b) (MAX(a, b) - MIN(a, b))
#define NEG(a)     (~(a) + 1)

template <bool DIRECTED = false, bool TREE = false>
struct graph_base
{
	struct vertex
	{
		// Specifics
		// uint64_t property;
	};

	struct edge
	{
		// Common
		uint32_t source, destination;
		uint8_t ignore = 0;

		// Specifics
		// uint64_t weight;
	};

	struct list
	{
		uint32_t vertex;
		uint32_t edge;
	};

	uint32_t vertex_count;
	uint32_t edge_count;

	vector<edge> edges;
	vector<vertex> vertices;
	vector<vector<list>> adjlist;

	vector<list> &operator[](uint32_t index)
	{
		return adjlist[index];
	}

	// For trees
	graph_base(uint32_t vertex_count)
		requires(TREE != 0)
	{
		this->vertex_count = vertex_count;
		this->edge_count = vertex_count - 1;
	}

	// General graphs
	graph_base(uint32_t vertex_count, uint32_t edge_count)
		requires(TREE == 0)
	{
		this->vertex_count = vertex_count;
		this->edge_count = edge_count;
	}

	void add_vertex(uint32_t value)
	{
		// this->vertices.push_back(value);
	}

	void read_vertices()
	{
		// this->vertices = vector<vertices>(this->vertex_count);

		for (uint32_t i = 0; i < this->vertex_count; ++i)
		{
			// cin >> this->vertices[i].property;
		}
	}

	void add_edge(uint32_t source, uint32_t destination)
	{
		this->edges.push_back({source, destination});
	}

	void read_edges()
	{
		this->edges = vector<edge>(this->edge_count);

		for (uint32_t i = 0; i < this->edge_count; ++i)
		{
			cin >> this->edges[i].source >> this->edges[i].destination;

			this->edges[i].source--;
			this->edges[i].destination--;
			this->edges[i].ignore = 0;
		}
	}

	void build()
	{
		this->adjlist = vector<vector<list>>(this->vertex_count);

		for (uint32_t i = 0; i < this->edge_count; ++i)
		{
			this->adjlist[this->edges[i].source].push_back({this->edges[i].destination, i});

			if constexpr (DIRECTED == 0)
			{
				this->adjlist[this->edges[i].destination].push_back({this->edges[i].source, i});
			}
		}
	}
};

using undirected_graph = graph_base<false, false>;
using directed_graph = graph_base<true, false>;
using tree = graph_base<false, true>;

vector<array<uint32_t, 2>> dfs_parents(undirected_graph &g, uint32_t root)
{
	uint32_t size = g.vertex_count;

	vector<uint8_t> visited(size, 0);
	vector<array<uint32_t, 2>> parents(size);
	stack<array<uint32_t, 2>> st;

	st.push({root, 0});
	visited[root] = 1;
	parents[root] = {UINT32_MAX, UINT32_MAX};

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		while (start < g[source].size())
		{
			uint32_t destination = g[source][start].vertex;

			// New vertex
			if (visited[destination] == 0)
			{
				st.push({destination, 0});
				visited[destination] = 1;

				pop = 0;
				break;
			}

			start += 1;
		}

		if (pop)
		{
			st.pop();

			if (st.size() != 0)
			{
				uint32_t parent = st.top()[0];
				uint32_t start = st.top()[1];

				parents[source] = {g[source][start].vertex, g[source][start].edge};

				st.top()[1] += 1;
			}
		}
	}

	return parents;
}

vector<uint32_t> dfs_bridges(undirected_graph &g, uint32_t index)
{
	uint32_t size = g.vertex_count;
	uint32_t timer = 0;

	vector<uint32_t> bridges;

	vector<uint8_t> visited(size, 0);
	vector<uint32_t> entry(size, 0);
	vector<uint32_t> low(size, 0);

	stack<array<uint32_t, 3>> st;

	st.push({index, index, 0});
	visited[index] = 1;
	entry[index] = ++timer;
	low[index] = entry[index];

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t parent = st.top()[1];
		uint32_t &start = st.top()[2];
		uint8_t skip = 0;
		uint8_t pop = 1;

		while (start < g[source].size())
		{
			uint32_t destination = g[source][start].vertex;

			// Skip parent only once
			if (destination == parent && skip == 0)
			{
				skip = 1;
				continue;
			}

			if (visited[destination])
			{
				low[source] = MIN(low[source], entry[destination]);
			}
			else
			{
				st.push({destination, source, 0});
				visited[destination] = 1;
				entry[destination] = ++timer;
				low[destination] = entry[destination];

				pop = 0;
				break;
			}

			start += 1;
		}

		if (pop)
		{
			st.pop();

			if (st.size() != 0)
			{
				low[parent] = MIN(low[parent], low[source]);

				if (low[source] > entry[parent])
				{
					bridges.push_back(g[parent][start].edge);
				}

				st.top()[2] += 1;
			}
		}
	}

	return bridges;
}

vector<uint32_t> dfs_hash(tree &g, uint32_t root)
{
	uint32_t size = g.vertex_count;
	vector<uint8_t> visited(size, 0);
	stack<array<uint32_t, 2>> st;

	vector<uint32_t> hashes(size, UINT32_MAX);
	map<vector<uint32_t>, uint32_t> cache;
	vector<vector<uint32_t>> states(size);

	uint32_t ids = 0;

	st.push({root, 0});
	visited[root] = 1;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		while (start < g[source].size())
		{
			uint32_t destination = g[source][start].vertex;

			if (visited[destination] == 0)
			{
				st.push({destination, 0});
				visited[destination] = 1;

				pop = 0;
				break;
			}

			start += 1;
		}

		if (pop)
		{
			// Create hash
			sort(states[source].begin(), states[source].end());

			if (!cache.contains(states[source]))
			{
				cache.insert({states[source], (hashes[source] = ids++)});
			}
			else
			{
				hashes[source] = cache[states[source]];
			}

			st.pop();

			if (st.size() != 0)
			{
				states[st.top()[0]].push_back(hashes[source]);
				st.top()[1] += 1;
			}
		}
	}

	return hashes;
}

vector<uint32_t> dfs_cycle(vector<vector<uint32_t>> &edge_graph, vector<pair<uint32_t, uint32_t>> &edges)
{
	vector<uint32_t> vertex_visited(edge_graph.size(), 0);
	vector<uint32_t> edge_visited(edges.size(), 0);
	vector<uint32_t> parent(edge_graph.size(), UINT32_MAX);
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

		for (uint32_t i = 0; i < edge_graph[source].size(); ++i)
		{
			uint32_t destination = 0;
			uint32_t edge = edge_graph[source][i];

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

vector<uint32_t> bfs_distances(undirected_graph &g, uint32_t index)
{
	vector<uint32_t> distances(g.vertex_count, 0);
	vector<uint8_t> visited(g.vertex_count, 0);
	queue<uint32_t> qu;

	qu.push(index);
	visited[index] = 1;
	distances[index] = 0;

	while (qu.size() != 0)
	{
		uint32_t source = qu.front();

		for (uint32_t i = 0; i < g[source].size(); ++i)
		{
			uint32_t destination = g[source][i].vertex;

			if (visited[destination] == 0)
			{
				qu.push(destination);
				visited[destination] = 1;
				distances[destination] = distances[source] + 1;
			}
		}

		qu.pop();
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

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

uint32_t longest_hamiltonian_aux(vector<vector<uint32_t>> &graph, map<pair<uint32_t, uint32_t>, uint32_t> &cache, uint32_t visited,
								 uint32_t index)
{
	uint32_t count = 0;

	if (cache.contains({index, visited}))
	{
		return cache[{index, visited}];
	}

	for (uint32_t i = 0; i < graph[index].size(); ++i)
	{
		uint32_t destination = graph[index][i];

		if ((visited & (1 << destination)) == 0)
		{
			count = MAX(count, longest_hamiltonian_aux(graph, cache, visited | (1 << destination), destination));
		}
	}

	count += 1;
	cache[{index, visited}] = count;

	return count;
}

uint32_t longest_hamiltonian_path(vector<vector<uint32_t>> &graph)
{
	uint32_t size = graph.size();
	uint32_t count = 0;

	map<pair<uint32_t, uint32_t>, uint32_t> cache;

	for (uint32_t i = 0; i < size; ++i)
	{
		count = MAX(count, longest_hamiltonian_aux(graph, cache, (1 << i), i));
	}

	return count;
}

uint32_t tree_centroid(vector<vector<pair<uint32_t, uint32_t>>> &graph, vector<edge> &edges, uint32_t index)
{
	vector<vector<uint32_t>> counts(graph.size());
	vector<uint8_t> visited(graph.size(), 0);
	stack<pair<uint32_t, uint32_t>> st;

	uint32_t centroid = UINT32_MAX;
	uint32_t total = 0;

	st.push({index, 0});
	visited[index] = 1;
	total += 1;

	// Count subtree sizes
	while (st.size() != 0)
	{
		uint32_t source = st.top().first;
		uint32_t start = st.top().second;
		uint8_t pop = 1;

		for (uint32_t i = start; i < graph[source].size(); ++i)
		{
			uint32_t destination = graph[source][i].first;
			uint32_t edge = graph[source][i].second;

			// New vertex
			if (visited[destination] == 0 && edges[edge].cut == 0)
			{
				// Update state
				if (i != start)
				{
					st.pop();
					st.push({source, i});
				}

				st.push({destination, 0});
				visited[destination] = 1;
				total += 1;

				pop = 0;
				break;
			}
		}

		if (pop)
		{
			uint32_t current = 0;
			uint32_t edge = 0;

			// Update parent
			for (uint32_t i = 0; i < counts[source].size(); ++i)
			{
				current += counts[source][i];
			}

			st.pop();

			if (st.size() != 0)
			{
				source = st.top().first;
				start = st.top().second;
				edge = graph[source][start].second;

				counts[source].push_back(1 + current);
			}
		}
	}

	// If only a single node is present it is the centroid
	if (total == 1)
	{
		return index;
	}

	// Count the subtree size of parent
	for (uint32_t i = 0; i < graph.size(); ++i)
	{
		uint32_t current = 0;

		if (visited[i])
		{
			for (uint32_t j = 0; j < counts[i].size(); ++j)
			{
				current += counts[i][j];
			}

			if (current != total - 1)
			{
				counts[i].push_back((total - 1) - current);
			}
		}
	}

	for (uint32_t i = 0; i < graph.size(); ++i)
	{
		uint8_t possible = 1;

		if (visited[i] == 0)
		{
			continue;
		}

		for (uint32_t j = 0; j < counts[i].size(); ++j)
		{
			if (counts[i][j] > total / 2)
			{
				possible = 0;
				break;
			}
		}

		if (possible)
		{
			centroid = i;
			break;
		}
	}

	return centroid;
}
