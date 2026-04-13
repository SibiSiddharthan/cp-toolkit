#include <vector>
#include <utility>
#include <stack>
#include <queue>
#include <array>
#include <map>
#include <iostream>
#include <algorithm>
#include <concepts>

#include "ds.cpp"

using namespace std;

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define DIFF(a, b) (MAX(a, b) - MIN(a, b))
#define NEG(a)     (~(a) + 1)

template <typename T>
using min_priority_queue = priority_queue<T, vector<T>, greater<T>>;

template <typename T>
using max_priority_queue = priority_queue<T, vector<T>, less<T>>;

template <bool DIRECTED = false, bool TREE = false>
struct graph_base
{
	struct vertex
	{
		// Specifics
		uint64_t property;
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
	graph_base(uint32_t vertex_count, uint32_t edge_count = 0)
		requires(TREE == 0)
	{
		this->vertex_count = vertex_count;
		this->edge_count = edge_count;
	}

	void add_vertex(uint32_t value)
	{
		this->vertices.push_back(value);
		this->vertex_count += 1;
	}

	void read_vertices()
	{
		this->vertices = vector<vertex>(this->vertex_count);

		for (uint32_t i = 0; i < this->vertex_count; ++i)
		{
			cin >> this->vertices[i].property;
		}
	}

	void add_edge(uint32_t source, uint32_t destination)
	{
		this->edges.push_back({source, destination});

		if constexpr (TREE == 0)
		{
			this->edge_count += 1;
		}
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

	uint32_t size()
	{
		return this->vertex_count;
	}
};

using undirected_graph = graph_base<false, false>;
using directed_graph = graph_base<true, false>;
using tree = graph_base<false, true>;

template <typename T>
concept graph_type = same_as<T, undirected_graph> || same_as<T, directed_graph> || same_as<T, tree>;

template <graph_type T>
vector<array<uint32_t, 2>> dfs_parents(T &graph, uint32_t root)
{
	vector<array<uint32_t, 2>> parents(graph.size());
	vector<uint8_t> visited(graph.size(), 0);
	stack<array<uint32_t, 2>> st;

	st.push({root, 0});
	visited[root] = 1;
	parents[root] = {UINT32_MAX, UINT32_MAX};

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		while (start < graph[source].size())
		{
			uint32_t destination = graph[source][start].vertex;

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

				parents[source] = {parent, graph[parent][start].edge};

				st.top()[1] += 1;
			}
		}
	}

	return parents;
}

template <graph_type T>
auto dfs_path(T &graph, uint32_t source, uint32_t destination)
{
	vector<array<uint32_t, 2>> parents = dfs_parents(graph, source);

	vector<uint32_t> path_edges;
	vector<uint32_t> path_vertices;

	uint32_t current = destination;

	path_edges.push_back(parents[current][1]);
	path_vertices.push_back(current);

	while (path_edges.back() != UINT32_MAX)
	{
		current = parents[current][0];

		path_edges.push_back(parents[current][1]);
		path_vertices.push_back(current);
	}

	path_edges.pop_back();

	reverse(path_edges.begin(), path_edges.end());
	reverse(path_vertices.begin(), path_vertices.end());

	return make_pair(path_edges, path_vertices);
}

vector<uint32_t> dfs_sort(directed_graph &graph)
{
	vector<uint8_t> visited(graph.size(), 0);
	vector<uint32_t> order;

	auto dfs = [&](uint32_t index) -> bool
	{
		stack<array<uint32_t, 2>> st;

		st.push({index, 0});
		visited[index] = 1;

		while (st.size() != 0)
		{
			uint32_t source = st.top()[0];
			uint32_t &start = st.top()[1];
			uint8_t pop = 1;

			while (start < graph[source].size())
			{
				uint32_t destination = graph[source][start].vertex;

				if (visited[destination] == 1)
				{
					return 1;
				}

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
				visited[source] = 2;
				order.push_back(source);

				st.pop();

				if (st.size() != 0)
				{
					st.top()[1] += 1;
				}
			}
		}

		return 0;
	};

	uint8_t cycle = 0;

	for (uint32_t i = 0; i < graph.size(); ++i)
	{
		if (visited[i] == 0)
		{
			if (dfs(i))
			{
				cycle = 1;
				break;
			}
		}
	}

	if (cycle)
	{
		return {};
	}

	reverse(order.begin(), order.end());

	return order;
}

template <graph_type T>
vector<uint32_t> dfs_bridges(T &graph, uint32_t index)
{
	vector<uint8_t> visited(graph.size(), 0);
	vector<uint32_t> entry(graph.size(), 0);
	vector<uint32_t> low(graph.size(), 0);
	stack<array<uint32_t, 3>> st;

	vector<uint32_t> bridges;
	uint32_t timer = 0;

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

		while (start < graph[source].size())
		{
			uint32_t destination = graph[source][start].vertex;

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
					bridges.push_back(graph[parent][start].edge);
				}

				st.top()[2] += 1;
			}
		}
	}

	return bridges;
}

vector<uint32_t> dfs_hash(tree &tree, uint32_t root)
{
	vector<uint8_t> visited(tree.size(), 0);
	stack<array<uint32_t, 2>> st;

	vector<vector<uint32_t>> states(tree.size());
	vector<uint32_t> hashes(tree.size(), UINT32_MAX);
	map<vector<uint32_t>, uint32_t> cache;

	uint32_t ids = 0;

	st.push({root, 0});
	visited[root] = 1;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		while (start < tree[source].size())
		{
			uint32_t destination = tree[source][start].vertex;

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

uint8_t dfs_games(directed_graph &graph, uint32_t index)
{
	vector<uint8_t> visited(graph.size(), 0);
	vector<uint8_t> status(graph.size(), 0);
	stack<array<uint32_t, 2>> st;

	st.push({index, 0});
	visited[index] = 1;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		while (start < graph[source].size())
		{
			uint32_t destination = graph[source][start].vertex;

			// New vertex
			if (visited[destination] == 0)
			{
				st.push({destination, 0});
				visited[destination] = 1;

				pop = 0;
				break;
			}
			else
			{
				status[source] |= ~status[destination];
			}

			start += 1;
		}

		if (pop)
		{
			st.pop();

			if (st.size() != 0)
			{
				status[st.top()[0]] |= ~status[source];
				st.top()[1] += 1;
			}
		}
	}

	return status[index];
}

auto dfs_counts(tree &tree, uint32_t root)
{
	vector<vector<array<uint32_t, 2>>> counts(tree.size());
	vector<uint64_t> totals(tree.size(), 0);
	vector<uint8_t> visited(tree.size(), 0);
	stack<array<uint32_t, 2>> st;

	st.push({root, 0});
	visited[root] = 1;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		for (uint32_t i = start; i < tree[source].size(); ++i)
		{
			uint32_t destination = tree[source][i].vertex;

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
			uint32_t current = 0;
			uint32_t edge = 0;

			// Update parent
			for (uint32_t i = 0; i < counts[source].size(); ++i)
			{
				current += counts[source][i][1];
			}

			st.pop();

			if (st.size() != 0)
			{
				uint32_t parent = st.top()[0];
				uint32_t start = st.top()[1];

				counts[parent].push_back({tree[parent][start].edge, 1 + current});
				st.top()[1] += 1;
			}
		}
	}

	for (uint32_t i = 0; i < tree.size(); ++i)
	{
		for (auto &[e, v] : counts[i])
		{
			totals[i] += v;
		}

		// Include self
		totals[i] += 1;
	}

	return make_pair(counts, totals);
}

vector<uint32_t> dfs_leaves(tree &tree, uint32_t root)
{
	vector<uint32_t> counts(tree.size(), 0);
	vector<uint8_t> visited(tree.size(), 0);
	stack<array<uint32_t, 2>> st;

	st.push({root, 0});
	visited[root] = 1;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		while (start < tree[source].size())
		{
			uint32_t destination = tree[source][start].vertex;

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
			if (tree[source].size() == 1)
			{
				if (source != root)
				{
					counts[source] += 1;
				}
			}

			st.pop();

			if (st.size() != 0)
			{
				uint32_t parent = st.top()[0];
				uint32_t start = st.top()[1];

				counts[parent] += counts[source];

				st.top()[1] += 1;
			}
		}
	}

	return counts;
}

uint32_t dfs_centroid(tree &tree, uint32_t index)
{
	vector<vector<uint32_t>> counts(tree.size());
	vector<uint8_t> visited(tree.size(), 0);
	stack<array<uint32_t, 2>> st;

	uint32_t centroid = UINT32_MAX;
	uint32_t total = 0;

	st.push({index, 0});
	visited[index] = 1;
	total += 1;

	// Count subtree sizes
	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		for (uint32_t i = start; i < tree[source].size(); ++i)
		{
			uint32_t destination = tree[source][i].vertex;
			uint32_t edge = tree[source][i].edge;

			// New vertex
			if (visited[destination] == 0 && tree.edges[edge].ignore == 0)
			{
				st.push({destination, 0});
				visited[destination] = 1;
				total += 1;

				pop = 0;
				break;
			}

			start += 1;
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
				counts[st.top()[0]].push_back(1 + current);
				st.top()[1] += 1;
			}
		}
	}

	// If only a single node is present it is the centroid
	if (total == 1)
	{
		return index;
	}

	// Count the subtree size of parent
	for (uint32_t i = 0; i < tree.size(); ++i)
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

	for (uint32_t i = 0; i < tree.size(); ++i)
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

auto dfs_tour(tree &tree, uint32_t root)
{
	vector<uint8_t> visited(tree.size(), 0);
	stack<array<uint32_t, 2>> st;

	vector<pair<uint32_t, uint32_t>> times(tree.size(), {0, 0});
	vector<uint32_t> tour;

	uint32_t timer = 0;

	st.push({root, 0});
	visited[root] = 1;
	times[root].first = timer++;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t &start = st.top()[1];
		uint8_t pop = 1;

		tour.push_back(source);

		for (uint32_t i = start; i < tree[source].size(); ++i)
		{
			uint32_t destination = tree[source][i].vertex;

			if (visited[destination] == 0)
			{
				st.push({destination, 0});
				visited[destination] = 1;
				times[destination].first = timer++;

				pop = 0;
				break;
			}

			start += 1;
		}

		if (pop)
		{
			times[source].second = timer++;
			st.pop();

			if (st.size() != 0)
			{
				st.top()[1] += 1;
			}
		}
	}

	return make_pair(tour, times);
}

template <graph_type T>
vector<array<uint32_t, 2>> dfs_cycle(T &graph, uint32_t index)
{
	vector<uint8_t> visited(graph.size(), 0);
	stack<array<uint32_t, 3>> st;

	vector<array<uint32_t, 2>> cycle;

	st.push({index, index, 0});
	visited[index] = 1;

	while (st.size() != 0)
	{
		uint32_t source = st.top()[0];
		uint32_t parent = st.top()[1];
		uint32_t &start = st.top()[2];
		uint8_t pop = 1;

		while (start < graph[source].size())
		{
			uint32_t destination = graph[source][start].vertex;
			uint32_t edge = graph[source][start].edge;

			if (destination != parent)
			{
				// Cycle detected
				if (visited[destination])
				{
					cycle.push_back({source, edge});

					while (source != destination)
					{
						st.pop();

						edge = graph[st.top()[0]][st.top()[2]].edge;
						source = st.top()[0];

						cycle.push_back({source, edge});
					}

					return cycle;
				}

				st.push({destination, source, 0});
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
				st.top()[1] += 1;
			}
		}
	}

	return cycle;
}

template <graph_type T>
vector<uint32_t> bfs_distances(T &graph, uint32_t index)
{
	vector<uint32_t> distances(graph.size(), 0);
	vector<uint8_t> visited(graph.size(), 0);
	queue<uint32_t> qu;

	qu.push(index);
	visited[index] = 1;
	distances[index] = 0;

	while (qu.size() != 0)
	{
		uint32_t source = qu.front();

		for (uint32_t i = 0; i < graph[source].size(); ++i)
		{
			uint32_t destination = graph[source][i].vertex;

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

template <graph_type T>
void bfs_multi(T &graph, vector<uint32_t> &sources, vector<uint32_t> &mins)
{
	vector<uint32_t> distances(graph.size(), UINT32_MAX);
	vector<uint32_t> visited(graph.size(), 0);
	queue<uint32_t> qu;

	for (uint32_t s : sources)
	{
		if (visited[s] == 0)
		{
			visited[s] = 1;
			distances[s] = 0;
			qu.push(s);
		}
	}

	while (qu.size() != 0)
	{
		uint32_t source = qu.front();

		for (uint32_t i = 0; i < graph[source].size(); ++i)
		{
			uint32_t destination = graph[source][i].vertex;

			if (visited[destination] == 0)
			{
				qu.push(destination);
				visited[destination] = 1;
				distances[destination] = distances[source] + 1;
				mins[destination] = MIN(mins[destination], mins[source]);
			}
			else
			{
				if (distances[destination] == distances[source] + 1)
				{
					mins[destination] = MIN(mins[destination], mins[source]);
				}
			}
		}

		qu.pop();
	}
}

vector<uint64_t> dijkstra(undirected_graph &graph, uint32_t index)
{
	vector<uint64_t> distances(graph.size(), UINT64_MAX);
	min_priority_queue<pair<uint64_t, uint32_t>> pq;

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
			uint32_t destination = graph[source][i].vertex;
			uint32_t edge = graph[source][i].edge;
			uint64_t weight = graph.edges[edge].ignore;

			if (weight + distances[source] < distances[destination])
			{
				distances[destination] = weight + distances[source];
				pq.push({distances[destination], destination});
			}
		}
	}

	return distances;
}

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

struct successor_graph
{
	vector<uint32_t> graph;
	vector<vector<uint32_t>> backs;
	vector<vector<uint32_t>> cycles;

	vector<pair<uint32_t, uint32_t>> cycle_info;

	uint32_t size;

	successor_graph(uint32_t n)
	{
		this->size = n;
		this->graph = vector<uint32_t>(n);
		this->backs = vector<vector<uint32_t>>(n);

		this->cycle_info = vector<pair<uint32_t, uint32_t>>(n, {UINT32_MAX, UINT32_MAX});
	}

	void read()
	{
		for (uint32_t i = 0; i < this->size; ++i)
		{
			cin >> this->graph[i];
		}
	}

	void build()
	{
		vector<uint8_t> visited(this->size, 0);
		vector<uint32_t> stack;

		// Detect cycles
		for (uint32_t i = 0; i < this->size; ++i)
		{
			vector<uint32_t> cycle;
			uint32_t current = i;

			if (visited[i] != 0)
			{
				continue;
			}

			while (visited[current] == 0)
			{
				visited[current] = 1;
				stack.push_back(current);

				current = this->graph[current];
			}

			while (stack.back() != current)
			{
				cycle.push_back(stack.back());
				stack.pop_back();
			}

			cycle.push_back(current);

			this->cycles.push_back(cycle);
			stack.clear();
		}

		for (uint32_t i = 0; i < this->cycles.size(); ++i)
		{
			for (uint32_t j = 0; j < this->cycles[i].size(); ++j)
			{
				this->cycle_info[this->cycles[i][j]] = {i, j};
			}
		}

		// Build forests
		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->backs[this->graph[i]].push_back(i);
		}

		for (uint32_t i = 0; i < this->size; ++i)
		{
			for (uint32_t j = 0; j < this->backs[i].size(); ++j)
			{
				uint32_t next = this->backs[i][j];

				if (this->cycle_info[next].first != UINT32_MAX)
				{
					continue;
				}
			}
		}
	}
};

struct lowest_common_ancestor
{
	tree &tr;

	vector<uint32_t> tour;
	vector<uint32_t> heights;
	vector<pair<uint32_t, uint32_t>> times;
	disjoint_sparse_table<pair<uint32_t, uint32_t>, ops::min> rmq;

	lowest_common_ancestor(tree &tr) : tr(this->tr)
	{
		vector<pair<uint32_t, uint32_t>> elements;

		auto [tour, times] = dfs_tour(this->tr, 0);

		this->heights = bfs_distances(this->tr, 0);
		this->tour = tour;
		this->times = times;

		for (auto i : this->tour)
		{
			elements.push_back({this->heights[i], i});
		}

		this->rmq = disjoint_sparse_table<pair<uint32_t, uint32_t>, ops::min>(elements);
	}

	auto operator[](uint32_t index)
	{
		return this->tr[index];
	}

	uint32_t lca(uint32_t a, uint32_t b)
	{
		uint32_t begin = times[a].first;
		uint32_t end = times[b].first;

		if (end < begin)
		{
			swap(begin, end);
		}

		return this->rmq.query(begin, end).second;
	}

	uint32_t lca(vector<uint32_t> &nodes)
	{
		uint32_t begin = UINT32_MAX;
		uint32_t end = 0;

		if (nodes.size() < 1)
		{
			return UINT32_MAX;
		}

		for (auto i : nodes)
		{
			begin = MIN(begin, i);
			end = MAX(end, i);
		}

		return this->rmq.query(begin, end).second;
	}

	uint32_t distance(uint32_t a, uint32_t b)
	{
		return (this->heights[a] + this->heights[b]) - (2 * this->heights[this->lca(a, b)]);
	}

	uint8_t is_ancestor(uint32_t a, uint32_t b)
	{
		return (this->times[a].first < this->times[b].first) && (this->times[b].first < this->times[a].second);
	}

	uint8_t on_path(uint32_t x, uint32_t a, uint32_t b)
	{
		return (this->is_ancestor(x, a) || this->is_ancestor(x, b)) && this->is_ancestor(this->lca(a, b), x);
	}
};
