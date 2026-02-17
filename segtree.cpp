#include <vector>
#include <stack>
#include <map>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct simple_segment_tree
{
	struct node
	{
		uint32_t value;
	};

	vector<node> tree;
	stack<uint32_t> st;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	uint32_t _op(uint32_t a, uint32_t b)
	{
		// Operations

		// add
		return a + b;

		// mul
		// return a * b;

		// min
		// return MIN(a, b);

		// max
		// return MAX(a, b);
	}

	void _join(uint32_t index)
	{
		if (((index * 2) + 2) < (this->offset + this->size))
		{
			this->tree[index].value = this->_op(this->tree[(index * 2) + 1].value, this->tree[(index * 2) + 2].value);
			return;
		}

		if (((index * 2) + 1) < (this->offset + this->size))
		{
			this->tree[index] = this->tree[(index * 2) + 1];
			return;
		}
	}

	pair<uint32_t, uint32_t> _range(uint32_t index)
	{
		uint32_t depth = (32 - __builtin_clz(index + 1)) - 1; // depth of node
		uint32_t count = this->nearest >> depth;              // count of responsibility
		uint32_t segment = (index + 1) & ~(1 << depth);       // index (0 based) of segment at depth
		uint32_t current_left = segment * count;              // left of responsibility
		uint32_t current_right = ((segment + 1) * count) - 1; // right of responsiblity

		return {current_left, current_right};
	}

	void _build(const vector<uint32_t> &elements)
	{
		this->nearest = 1 << ((32 - __builtin_clz(elements.size())) - 1);

		if (this->nearest < elements.size())
		{
			this->nearest <<= 1;
		}

		this->offset = this->nearest - 1;
		this->size = elements.size();

		this->tree = vector<node>(this->offset + this->size);

		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset].value = elements[i];
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	simple_segment_tree(const vector<uint32_t> &elements)
	{
		this->_build(elements);
	}

	simple_segment_tree(uint32_t size)
	{
		this->_build(vector<uint32_t>(size, 0));
	}

	void update(uint32_t index, uint32_t value)
	{
		uint32_t parent = 0;

		if (index >= this->size)
		{
			return;
		}

		index = this->offset + index;

		this->tree[index].value = value;

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			this->_join(index);
		}
	}

	uint32_t query(uint32_t left, uint32_t right)
	{
		uint32_t value = 0;

		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			auto [current_left, current_right] = this->_range(index);

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				value = this->_op(value, this->tree[index].value);
				continue;
			}

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return value;
	}
};

struct lazy_segment_tree
{
	struct node
	{
	};

	struct update
	{
	};

	vector<node> tree;
	vector<update> lazy;

	stack<uint32_t> st;
	stack<uint32_t> up;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	void _join(uint32_t index)
	{
	}

	void _apply(uint32_t index, const update &element)
	{
		if (index >= this->offset + this->size)
		{
			return;
		}

		// Apply to current node

		if (index < this->offset)
		{
			// Push to children
		}
	}

	void _push(uint32_t index)
	{
		if (index < this->offset)
		{
			this->_apply((index * 2) + 1, this->lazy[index]);
			this->_apply((index * 2) + 2, this->lazy[index]);

			this->lazy[index] = {};
		}
	}

	pair<uint32_t, uint32_t> _range(uint32_t index)
	{
		uint32_t depth = (32 - __builtin_clz(index + 1)) - 1; // depth of node
		uint32_t count = this->nearest >> depth;              // count of responsibility
		uint32_t segment = (index + 1) & ~(1 << depth);       // index (0 based) of segment at depth
		uint32_t current_left = segment * count;              // left of responsibility
		uint32_t current_right = ((segment + 1) * count) - 1; // right of responsiblity

		return {current_left, current_right};
	}

	void _build(const vector<uint32_t> &elements)
	{
		this->nearest = 1 << ((32 - __builtin_clz(elements.size())) - 1);

		if (this->nearest < elements.size())
		{
			this->nearest <<= 1;
		}

		this->offset = this->nearest - 1;
		this->size = elements.size();

		this->tree = vector<node>(this->offset + this->size);
		this->lazy = vector<update>(this->offset);

		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset] = {};
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	lazy_segment_tree(const vector<uint32_t> &elements)
	{
		this->_build(elements);
	}

	lazy_segment_tree(uint32_t size)
	{
		this->_build(vector<uint32_t>(size, 0));
	}

	void range_update(uint32_t left, uint32_t right)
	{
		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			auto [current_left, current_right] = this->_range(index);

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				// For beats
				// Determine when all of the nodes are affected
				// Determine when none of the nodes are affected

				continue;
			}

			// Push the updates lazily
			this->_push(index);
			this->up.push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	uint32_t range_query(uint32_t left, uint32_t right)
	{
		uint32_t result = 0;

		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			auto [current_left, current_right] = this->_range(index);

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{

				continue;
			}

			// Push updates
			this->_push(index);

			this->st.push((index * 2) + 1);
			this->st.push((index * 2) + 2);
		}

		return result;
	}
};

struct sparse_segment_tree
{
	using range_t = uint32_t;

	struct node
	{
		range_t begin, end;   // responsibility
		uint32_t left, right; // children

		node()
		{
			begin = 0, end = 0, left = 0, right = 0;
		}
	};

	struct update
	{
	};

	vector<node> tree;
	vector<update> lazy;

	stack<uint32_t> st;
	stack<uint32_t> up;

	range_t begin;
	range_t end;

	void _join(uint32_t index)
	{
		uint32_t left = this->tree[index].left;
		uint32_t right = this->tree[index].right;

		if (this->_leaf(index))
		{
			return;
		}

		// Join here
	}

	void _apply(uint32_t index, const update &element)
	{
		// Apply to current node

		// Push to children
	}

	bool _skip(uint32_t index)
	{
		// Condition for skipping push
		return false;
	}

	bool _leaf(uint32_t index)
	{
		if (this->tree[index].begin == this->tree[index].end)
		{
			return true;
		}

		return false;
	}

	void _create(uint32_t index)
	{
		node left = {}, right = {};

		if (this->_leaf(index))
		{
			return;
		}

		if (this->tree[index].left != 0 && this->tree[index].right != 0)
		{
			return;
		}

		// left
		left.begin = this->tree[index].begin;
		left.end = (this->tree[index].begin + this->tree[index].end) / 2;

		this->tree[index].left = this->tree.size();
		this->tree.push_back(left);
		this->lazy.push_back({});

		// right
		right.begin = left.end + 1;
		right.end = this->tree[index].end;

		this->tree[index].right = this->tree.size();
		this->tree.push_back(right);
		this->lazy.push_back({});
	}

	void _push(uint32_t index)
	{
		if (this->_leaf(index))
		{
			this->lazy[index] = {};
			return;
		}

		if (this->_skip(index))
		{
			return;
		}

		this->_apply(this->tree[index].left, this->lazy[index]);
		this->_apply(this->tree[index].right, this->lazy[index]);

		this->lazy[index] = {};
	}

	void _build(range_t begin, range_t end)
	{
		// Create the root
		node root;

		root.begin = begin;
		root.end = end;
		root.left = 0;
		root.right = 0;

		this->tree.push_back(root);
		this->lazy.push_back({});
	}

	sparse_segment_tree(range_t begin, range_t end)
	{
		this->begin = begin;
		this->end = end;

		this->_build(begin, end);
	}

	void range_update(range_t left, range_t right)
	{
		if (left > this->end)
		{
			left = this->begin;
		}

		if (right > this->end)
		{
			right = this->end;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			range_t current_left = this->tree[index].begin;
			range_t current_right = this->tree[index].end;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				// For beats
				// Determine when all of the nodes are affected
				// Determine when none of the nodes are affected

				this->_apply(index, {});
				continue;
			}

			// Create nodes lazily
			this->_create(index);

			// Push the updates lazily
			this->_push(index);
			this->up.push(index);

			this->st.push(this->tree[index].left);
			this->st.push(this->tree[index].right);
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	uint32_t range_query(range_t left, range_t right)
	{
		uint32_t result = 0;

		if (left > this->end)
		{
			left = this->begin;
		}

		if (right > this->end)
		{
			right = this->end;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			range_t current_left = this->tree[index].begin;
			range_t current_right = this->tree[index].end;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{

				continue;
			}

			// Create nodes
			this->_create(index);

			// Push updates
			this->_push(index);

			this->st.push(this->tree[index].left);
			this->st.push(this->tree[index].right);
		}

		return result;
	}
};

struct persistent_segment_tree
{
	using range_t = uint32_t;

	struct node
	{
		uint32_t value;
		range_t begin, end;   // responsibility
		uint32_t left, right; // children
		uint32_t parent;

		node()
		{
			value = 0;
			begin = 0, end = 0, left = 0, right = 0, parent = 0;
		}
	};

	struct update
	{
	};

	struct info
	{
		uint32_t index;
		uint32_t parent;
		uint32_t lr;
	};

	vector<node> tree;
	vector<update> lazy;

	map<uint32_t, uint32_t> roots;
	uint32_t recent;

	stack<info> cm;
	stack<uint32_t> st;
	stack<uint32_t> up;

	range_t begin;
	range_t end;

	void _join(uint32_t index)
	{
		uint32_t left = this->tree[index].left;
		uint32_t right = this->tree[index].right;

		if (this->_leaf(index))
		{
			return;
		}

		// Join here
	}

	void _apply(uint32_t index, const update &element)
	{
		// Apply to current node

		// Push to children
	}

	bool _skip(uint32_t index)
	{
		// Condition for skipping push
		return false;
	}

	bool _leaf(uint32_t index)
	{
		if (this->tree[index].begin == this->tree[index].end)
		{
			return true;
		}

		return false;
	}

	uint32_t _duplicate(uint32_t index, uint32_t parent, uint32_t lr)
	{
		node dup = this->tree[index];
		update lz = this->lazy[index];

		index = this->tree.size();

		this->tree.push_back(dup);
		this->lazy.push_back(lz);

		if (parent != 0)
		{
			if (lr == 0)
			{
				this->tree[parent].left = index;
			}
			else
			{
				this->tree[parent].right = index;
			}

			this->tree[index].parent = parent;
		}
		else
		{
			this->recent = index;
		}

		return index;
	}

	void _create(uint32_t index)
	{
		node left = {}, right = {};

		if (this->_leaf(index))
		{
			return;
		}

		if (this->tree[index].left != 0 && this->tree[index].right != 0)
		{
			return;
		}

		// left
		left.begin = this->tree[index].begin;
		left.end = (this->tree[index].begin + this->tree[index].end) / 2;
		left.parent = index;

		this->tree[index].left = this->tree.size();
		this->tree.push_back(left);
		this->lazy.push_back({});

		// right
		right.begin = left.end + 1;
		right.end = this->tree[index].end;
		right.parent = index;

		this->tree[index].right = this->tree.size();
		this->tree.push_back(right);
		this->lazy.push_back({});
	}

	void _push(uint32_t index)
	{
		uint32_t left = this->tree[index].left;
		uint32_t right = this->tree[index].right;

		if (this->_leaf(index))
		{
			this->lazy[index] = {};
			return;
		}

		if (this->_skip(index))
		{
			return;
		}

		if (this->tree[left].parent != index)
		{
			this->_duplicate(left, index, 0);
		}

		if (this->tree[right].parent != index)
		{
			this->_duplicate(right, index, 1);
		}

		this->_apply(this->tree[index].left, this->lazy[index]);
		this->_apply(this->tree[index].right, this->lazy[index]);

		this->lazy[index] = {};
	}

	void _sentinel()
	{
		node sentinel = {};

		sentinel.begin = begin;
		sentinel.end = end;

		this->tree.push_back(sentinel);
		this->lazy.push_back({});
		this->roots.insert({UINT32_MAX, 0});
	}

	void _build(vector<uint32_t> &elements)
	{
		// Create the root
		node root = {};

		root.begin = begin;
		root.end = end;

		this->tree.push_back(root);
		this->lazy.push_back({});
		this->roots.insert({UINT32_MAX, 0});

		// Create all the nodes recursively
		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();

			this->st.pop();

			if (this->_leaf(index))
			{
				this->tree[index].value = elements[this->tree[index].begin];
				continue;
			}

			// Create the nodes
			this->_create(index);
			this->up.push(index);

			this->st.push(this->tree[index].left);
			this->st.push(this->tree[index].right);
		}

		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	persistent_segment_tree(vector<uint32_t> &elements)
	{
		this->begin = 0;
		this->end = elements.size();

		this->_build(elements);
	}

	persistent_segment_tree(range_t begin, range_t end)
	{
		this->begin = begin;
		this->end = end;

		this->_sentinel();
	}

	uint32_t range_update(uint32_t base, uint32_t update, range_t left, range_t right)
	{
		if (left > this->end)
		{
			left = this->begin;
		}

		if (right > this->end)
		{
			right = this->end;
		}

		if (this->roots.contains(base))
		{
			this->cm.push({this->roots[base], 0, 0});
		}
		else
		{
			this->cm.push({this->roots[UINT32_MAX], 0, 0});
		}

		while (this->cm.size() != 0)
		{
			auto [index, parent, lr] = this->cm.top();
			range_t current_left = this->tree[index].begin;
			range_t current_right = this->tree[index].end;

			uint32_t new_index = 0;

			this->cm.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				// For beats
				// Determine when all of the nodes are affected
				// Determine when none of the nodes are affected
				this->_apply(this->_duplicate(index, parent, lr), {});

				continue;
			}

			// Create nodes lazily
			this->_create(index);

			// Push the updates lazily
			this->_push(index);

			// Create new nodes for updated path
			this->up.push(new_index = this->_duplicate(index, parent, lr));

			this->cm.push({this->tree[new_index].left, new_index, 0});
			this->cm.push({this->tree[new_index].right, new_index, 1});
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}

		// Add new root
		this->roots.insert({update, this->recent});
	}

	uint32_t range_query(uint32_t base, range_t left, range_t right)
	{
		uint32_t result = 0;

		if (left > this->end)
		{
			left = this->begin;
		}

		if (right > this->end)
		{
			right = this->end;
		}

		if (this->roots.contains(base))
		{
			this->st.push(this->roots[base]);
		}
		else
		{
			this->st.push(this->roots[UINT32_MAX]);
		}

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			range_t current_left = this->tree[index].begin;
			range_t current_right = this->tree[index].end;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{

				continue;
			}

			// Create nodes
			this->_create(index);

			// Push updates
			this->_push(index);

			this->st.push(this->tree[index].left);
			this->st.push(this->tree[index].right);
		}

		return result;
	}
};
