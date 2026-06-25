#include "cp.h"
#include "ds.cpp"
#include "segtree.cpp"

template <typename T>
struct op_min
{
	T identity()
	{
		return numeric_limits<T>::max();
	}

	T join(const T &a, const T &b) const
	{
		return MIN(a, b);
	}

	template <typename U>
	T assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return static_cast<T>(element);
	}
};

struct tree_diameter_node
{
	uint32_t x, y;
};

struct tree_diameter_op
{
	lowest_common_ancestor &lca;

	tree_diameter_op(lowest_common_ancestor &lca) : lca(lca)
	{
	}

	tree_diameter_node identity() const
	{
		return {UINT32_MAX, UINT32_MAX};
	}

	tree_diameter_node join(const tree_diameter_node &left, const tree_diameter_node &right)
	{
		tree_diameter_node result = {UINT32_MAX, UINT32_MAX};
		array<uint32_t, 4> temp = {left.x, left.y, right.x, right.y};
		uint32_t diameter = 0;

		for (uint32_t i = 0; i < 4; ++i)
		{
			if (temp[i] == UINT32_MAX)
			{
				continue;
			}

			for (uint32_t j = i + 1; j < 4; ++j)
			{
				if (temp[j] == UINT32_MAX)
				{
					continue;
				}

				uint32_t dist = lca.distance(temp[i], temp[j]);

				if (dist >= diameter)
				{
					result = {temp[i], temp[j]};
					diameter = dist;
				}
			}
		}

		return result;
	}

	template <typename U>
	tree_diameter_node assign(const U &element, [[maybe_unused]] uint32_t index) const
	{
		return {static_cast<uint32_t>(element), static_cast<uint32_t>(element)};
	}
};

struct lowest_common_ancestor
{
	struct vertex
	{
		// Specifics
		uint64_t placeholder;
	};

	struct edge
	{
		uint32_t source, destination;

		// Specifics
	};

	struct list
	{
		uint32_t vertex;
		uint32_t edge;
	};

	uint32_t root;
	uint32_t vertex_count;
	uint32_t edge_count;

	vector<vertex> vertices;
	vector<edge> edges;
	vector<vector<list>> adjlist;

	vector<uint32_t> tour;
	vector<uint32_t> heights;
	vector<uint32_t> counts;
	vector<pair<uint32_t, uint32_t>> parents;
	vector<pair<uint32_t, uint32_t>> times;

	disjoint_sparse_table<pair<uint32_t, uint32_t>, op_min<pair<uint32_t, uint32_t>>> rmq;
	binary_jumping<> table;

	void _dfs()
	{
		vector<uint8_t> visited(this->size(), 0);
		stack<array<uint32_t, 2>> st;

		uint32_t timer = 0;

		st.push({this->root, 0});
		visited[this->root] = 1;

		this->parents[this->root] = {this->root, UINT32_MAX};
		this->times[this->root].first = timer;
		this->heights[this->root] = 0;

		while (st.size() != 0)
		{
			uint32_t source = st.top()[0];
			uint32_t &start = st.top()[1];
			uint8_t pop = 1;

			this->tour.push_back(source);
			timer += 1;

			for (uint32_t i = start; i < this->adjlist[source].size(); ++i)
			{
				uint32_t destination = this->adjlist[source][i].vertex;

				if (visited[destination] == 0)
				{
					st.push({destination, 0});
					visited[destination] = 1;

					this->times[destination].first = timer;
					this->heights[destination] = this->heights[source] + 1;

					pop = 0;
					break;
				}

				start += 1;
			}

			if (pop)
			{
				this->times[source].second = timer;
				this->counts[source] += 1;

				st.pop();

				if (st.size() != 0)
				{
					this->parents[source] = {st.top()[0], this->adjlist[st.top()[0]][st.top()[1]].edge};
					this->counts[st.top()[0]] += this->counts[source];

					st.top()[1] += 1;
				}
			}
		}
	}

	lowest_common_ancestor(uint32_t size = 0)
	{
		this->vertex_count = size;
		this->edge_count = ((size != 0) ? size - 1 : 0);
	}

	vector<list> &operator[](uint32_t index)
	{
		return this->adjlist[index];
	}

	void add_vertex(uint64_t value)
	{
		this->vertices.push_back({value});
		this->vertex_count += 1;
	}

	void read_vertices()
	{
		this->vertices = vector<vertex>(this->vertex_count);

		for (uint32_t i = 0; i < this->vertex_count; ++i)
		{
			cin >> this->vertices[i].placeholder;
		}
	}

	void add_edge(uint32_t source, uint32_t destination)
	{
		this->edges.push_back({source, destination});
		this->edge_count += 1;
	}

	void read_edges()
	{
		this->edges = vector<edge>(this->edge_count);

		for (uint32_t i = 0; i < this->edge_count; ++i)
		{
			cin >> this->edges[i].source >> this->edges[i].destination;

			this->edges[i].source--;
			this->edges[i].destination--;
		}
	}

	void build(uint32_t root)
	{
		this->root = root;

		// Build adjacency list
		this->adjlist = vector<vector<list>>(this->vertex_count);

		for (uint32_t i = 0; i < this->edge_count; ++i)
		{
			this->adjlist[this->edges[i].source].push_back({this->edges[i].destination, i});
			this->adjlist[this->edges[i].destination].push_back({this->edges[i].source, i});
		}

		// Compute necessary information
		this->heights = vector<uint32_t>(this->size());
		this->counts = vector<uint32_t>(this->size());
		this->parents = vector<pair<uint32_t, uint32_t>>(this->size());
		this->times = vector<pair<uint32_t, uint32_t>>(this->size());

		this->_dfs();

		// Build lca
		vector<pair<uint32_t, uint32_t>> elements;

		for (auto i : this->tour)
		{
			elements.push_back({this->heights[i], i});
		}

		this->rmq = disjoint_sparse_table<pair<uint32_t, uint32_t>, op_min<pair<uint32_t, uint32_t>>>(elements);

		// Build ancestor
		this->table = binary_jumping<>(this->parents);

		// Build diameter
	}

	uint32_t size()
	{
		return this->vertex_count;
	}

	uint32_t size(uint32_t index)
	{
		return this->counts[index];
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

	uint32_t height(uint32_t index)
	{
		return this->heights[index];
	}

	uint32_t ancestor(uint32_t index, uint32_t rank)
	{
		if (rank >= this->size())
		{
			return this->root;
		}

		return this->table.query(index, rank).first;
	}

	uint32_t distance(uint32_t a, uint32_t b)
	{
		return (this->heights[a] + this->heights[b]) - (2 * this->heights[this->lca(a, b)]);
	}

	uint8_t is_ancestor(uint32_t a, uint32_t b)
	{
		return (this->times[a].first <= this->times[b].first) && (this->times[b].first < this->times[a].second);
	}

	uint8_t on_path(uint32_t x, uint32_t a, uint32_t b)
	{
		return (this->is_ancestor(x, a) || this->is_ancestor(x, b)) && this->is_ancestor(this->lca(a, b), x);
	}

	template <typename T, typename O>
		requires(!std::is_empty_v<T>)
	T query(binary_jumping<T, O> &bjt, uint32_t u, uint32_t v)
	{
		uint32_t lca = this->lca(u, v);
		O op;

		if (lca == u || lca == v)
		{
			uint32_t distance = this->distance(u, v);

			if (lca == u)
			{
				return bjt.query(v, distance + 1).second;
			}
			else
			{
				return bjt.query(u, distance + 1).second;
			}
		}
		else
		{
			uint32_t du = this->distance(lca, u);
			uint32_t dv = this->distance(lca, v);

			T left = bjt.query(u, du + 1).second;
			T right = bjt.query(v, dv).second;

			if constexpr (requires(O op, T a) {
							  { op.reverse(a) } -> std::same_as<T>;
						  })
			{
				right = op.reverse(right);
			}

			return op.join(left, right);
		}
	}
};

struct heavy_light
{
	lowest_common_ancestor &lca;

	vector<uint32_t> heavy;
	vector<uint32_t> head;
	vector<uint32_t> pos;

	void _dfs()
	{
		stack<array<uint32_t, 2>> st;
		uint32_t timer = 0;

		st.push({this->lca.root, 0});

		while (st.size() != 0)
		{
			uint32_t source = st.top()[0];
			uint32_t &start = st.top()[1];
			uint8_t pop = 1;

			if (this->pos[source] == UINT32_MAX)
			{
				uint32_t child = this->heavy[source];
				uint32_t last = child;

				this->head[source] = source;

				while (child != UINT32_MAX)
				{
					this->head[child] = source;
					last = child;
					child = this->heavy[child];
				}

				child = last;

				if (child != UINT32_MAX)
				{
					while (child != source)
					{
						this->pos[child] = timer++;
						child = this->lca.parents[child].first;
					}
				}

				this->pos[source] = timer++;
			}

			for (uint32_t i = start; i < this->lca[source].size(); ++i)
			{
				uint32_t destination = this->lca[source][i].vertex;

				if (this->lca.parents[source].first != destination)
				{
					st.push({destination, 0});

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

		for (uint32_t i = 0; i < this->lca.size(); ++i)
		{
			if (this->pos[i] == UINT32_MAX)
			{
				this->pos[i] = timer++;
			}
		}
	}

	heavy_light(lowest_common_ancestor &lca) : lca(lca)
	{
		this->heavy = vector<uint32_t>(this->lca.size(), UINT32_MAX);
		this->head = vector<uint32_t>(this->lca.size(), UINT32_MAX);
		this->pos = vector<uint32_t>(this->lca.size(), UINT32_MAX);

		for (uint32_t i = 0; i < this->lca.size(); ++i)
		{
			uint32_t max = 0;
			uint32_t child = UINT32_MAX;

			for (auto [v, e] : this->lca[i])
			{
				if (this->lca.parents[i].first != v)
				{
					if (this->lca.size(v) > max)
					{
						max = this->lca.size(v);
						child = v;
					}
				}
			}

			this->heavy[i] = child;
		}

		this->_dfs();
	}

	template <typename T, typename O, typename U, typename... args>
	simple_segment_tree<T, O> build(const vector<U> &elements, args &&...arg)
	{
		vector<U> data(elements.size());

		for (uint32_t i = 0; i < this->lca.size(); ++i)
		{
			data[this->pos[i]] = elements[i];
		}

		return simple_segment_tree<T, O>(data, std::forward<args>(arg)...);
	}

	template <typename T, typename O, typename... args>
	T query(simple_segment_tree<T, O> &st, uint32_t u, uint32_t v, args &&...arg)
	{
		uint32_t lca = this->lca.lca(u, v);
		O op(std::forward<args>(arg)...);

		// lca(u,v) = u
		auto process = [&](uint32_t u, uint32_t v) -> T
		{
			T result = op.identity();

			while (this->lca.height(this->head[u]) > this->lca.height(v))
			{
				result = op.join(result, st.query(this->pos[u], this->pos[head[u]]));
				u = this->lca.parents[this->head[u]].first;
			}

			result = op.join(result, st.query(this->pos[u], this->pos[v]));

			return result;
		};

		if (lca == u || lca == v)
		{
			if (lca == u)
			{
				return process(v, u);
			}
			else
			{
				return process(u, v);
			}
		}
		else
		{
			uint32_t distance = this->lca.distance(lca, v);
			uint32_t ancestor = this->lca.ancestor(v, distance - 1);

			T left = process(u, lca);
			T right = process(v, ancestor);

			if constexpr (requires(O op, T a) {
							  { op.reverse(a) } -> std::same_as<T>;
						  })
			{
				right = op.reverse(right);
			}

			return op.join(left, right);
		}
	}
};
