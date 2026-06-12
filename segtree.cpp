#include "cp.h"

template <typename O, typename T>
concept segtree_operator = requires(O op, T a, uint32_t i) {
	{ op.assign(a, i) } -> std::same_as<T>;
} && binary_operator<O, T>;

template <typename O, typename T, typename L>
concept segtree_operator_ext = requires(O op, T a, L b, uint32_t r) {
	requires std::is_empty_v<L> || requires {
		{ op.apply(a, b, r, r) } -> std::same_as<T>;
		{ op.compose(b, b) } -> std::same_as<L>;
		{ op.reset() } -> std::same_as<L>;
	};
} && segtree_operator<O, T>;

template <typename T, typename O>
	requires segtree_operator<O, T>
struct simple_segment_tree
{
	vector<T> tree;
	stack<array<uint32_t, 3>> st;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	O op;

	void _join(uint32_t index)
	{
		this->tree[index] = this->op.join(this->tree[(index * 2) + 1], this->tree[(index * 2) + 2]);
	}

	template <typename U>
	void _build(const vector<U> &elements)
	{
		this->nearest = 1 << (((32 - __builtin_clz(elements.size())) - 1) + (__builtin_popcount(elements.size()) != 1));
		this->offset = this->nearest - 1;
		this->size = elements.size();

		this->tree = vector<T>((this->nearest << 1) - 1, this->op.identity());

		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset] = this->op.assign(elements[i], i);
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	template <typename U, typename... args>
	simple_segment_tree(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->_build(elements);
	}

	T all()
	{
		return this->tree[0];
	}

	template <typename U>
	void update(uint32_t index, U value)
	{
		uint32_t parent = 0;

		if (index >= this->size)
		{
			return;
		}

		this->tree[index + this->offset] = this->op.assign(value, index);
		index += this->offset;

		while (index != 0)
		{
			parent = (index - 1) / 2;
			index = parent;

			this->_join(index);
		}
	}

	T query(uint32_t left, uint32_t right)
	{
		T value = this->op.identity();

		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push({0, 0, this->nearest - 1});

		while (this->st.size() != 0)
		{
			auto [index, current_left, current_right] = this->st.top();
			uint32_t middle = (current_left + current_right) / 2;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				value = this->op.join(this->tree[index], value);
				continue;
			}

			this->st.push({(index * 2) + 1, current_left, middle});
			this->st.push({(index * 2) + 2, middle + 1, current_right});
		}

		return value;
	}
};

template <typename T, typename L, typename O>
	requires segtree_operator_ext<O, T, L>
struct lazy_segment_tree
{
	vector<T> tree;
	vector<L> lazy;

	stack<array<uint32_t, 3>> st;
	stack<uint32_t> up;

	uint32_t offset;
	uint32_t size;
	uint32_t nearest;

	O op;

	void _join(uint32_t index)
	{
		this->tree[index] = this->op.join(this->tree[(index * 2) + 1], this->tree[(index * 2) + 2]);
	}

	void _apply(uint32_t index, const L &element)
	{
		// Apply to current node
		auto [begin, end] = this->_range(index);
		this->tree[index] = this->op.apply(this->tree[index], element, begin, end);

		if (index < this->offset)
		{
			// Push to children
			this->lazy[index] = this->op.compose(this->lazy[index], element);
		}
	}

	void _push(uint32_t index)
	{
		if (index < this->offset)
		{
			this->_apply((index * 2) + 1, this->lazy[index]);
			this->_apply((index * 2) + 2, this->lazy[index]);

			this->lazy[index] = this->op.reset();
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

	template <typename U>
	void _build(const vector<U> &elements)
	{
		this->nearest = 1 << (((32 - __builtin_clz(elements.size())) - 1) + (__builtin_popcount(elements.size()) != 1));
		this->offset = this->nearest - 1;
		this->size = elements.size();

		this->tree = vector<T>((this->nearest << 1) - 1, this->op.identity());
		this->lazy = vector<L>(this->offset, this->op.reset());

		for (uint32_t i = 0; i < this->size; ++i)
		{
			this->tree[i + this->offset] = this->op.assign(elements[i], i);
		}

		for (uint32_t i = this->offset; i != 0; --i)
		{
			this->_join(i - 1);
		}
	}

	template <typename U, typename... args>
	lazy_segment_tree(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->_build(elements);
	}

	T all()
	{
		return this->tree[0];
	}

	void update(uint32_t left, uint32_t right, const L &element)
	{
		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push({0, 0, this->nearest - 1});

		while (this->st.size() != 0)
		{
			auto [index, current_left, current_right] = this->st.top();
			uint32_t middle = (current_left + current_right) / 2;

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
				this->_apply(index, element);
				continue;
			}

			// Push the updates lazily
			this->_push(index);
			this->up.push(index);

			this->st.push({(index * 2) + 1, current_left, middle});
			this->st.push({(index * 2) + 2, middle + 1, current_right});
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	T query(uint32_t left, uint32_t right)
	{
		T result = this->op.identity();

		if (left > this->size)
		{
			left = 0;
		}

		if (right >= this->size)
		{
			right = this->size - 1;
		}

		this->st.push({0, 0, this->nearest - 1});

		while (this->st.size() != 0)
		{
			auto [index, current_left, current_right] = this->st.top();
			uint32_t middle = (current_left + current_right) / 2;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				result = this->op.join(this->tree[index], result);
				continue;
			}

			// Push updates
			this->_push(index);

			this->st.push({(index * 2) + 1, current_left, middle});
			this->st.push({(index * 2) + 2, middle + 1, current_right});
		}

		return result;
	}
};

template <typename T, typename L, typename O>
	requires segtree_operator_ext<O, T, L>
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

	vector<node> info;
	vector<T> tree;
	vector<L> lazy;

	stack<uint32_t> st;
	stack<uint32_t> up;

	range_t begin;
	range_t end;

	O op;

	void _join(uint32_t index)
	{
		if (this->_leaf(index))
		{
			return;
		}

		// Join here
		this->tree[index] = this->op.join(this->tree[this->info[index].left], this->tree[this->info[index].right]);
	}

	void _apply(uint32_t index, const L &element)
		requires(!std::is_empty_v<L>)
	{
		// Apply to current node
		this->tree[index] = this->op.apply(this->tree[index], element, this->info[index].begin, this->info[index].end);

		// Push to children
		this->lazy[index] = this->op.compose(this->lazy[index], element);
	}

	bool _leaf(uint32_t index)
	{
		if (this->info[index].begin == this->info[index].end)
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

		if (this->info[index].left != 0 && this->info[index].right != 0)
		{
			return;
		}

		// left
		left.begin = this->info[index].begin;
		left.end = (this->info[index].begin + this->info[index].end) / 2;

		this->info[index].left = this->info.size();
		this->info.push_back(left);
		this->tree.push_back(this->op.identity());

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}

		// right
		right.begin = left.end + 1;
		right.end = this->info[index].end;

		this->info[index].right = this->info.size();
		this->info.push_back(right);
		this->tree.push_back(this->op.identity());

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}
	}

	void _push(uint32_t index)
		requires(!std::is_empty_v<L>)
	{
		if (this->_leaf(index))
		{
			this->lazy[index] = this->op.reset();
			return;
		}

		this->_apply(this->info[index].left, this->lazy[index]);
		this->_apply(this->info[index].right, this->lazy[index]);

		this->lazy[index] = this->op.reset();
	}

	template <typename U>
	void _build(const vector<U> &elements, const vector<uint32_t> &indices, range_t begin, range_t end)
	{
		// Create the root
		node root = {};
		uint32_t count = indices.size();

		root.begin = begin;
		root.end = end;
		root.left = 0;
		root.right = 0;

		this->info.push_back(root);
		this->tree.push_back(this->op.identity());

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}

		for (uint32_t i = 0; i < count; ++i)
		{
			this->st.push(0);

			while (this->st.size() != 0)
			{
				uint32_t index = this->st.top();
				range_t current_left = this->info[index].begin;
				range_t current_right = this->info[index].end;

				this->st.pop();

				if (current_right < indices[i] || current_left > indices[i])
				{
					continue;
				}

				if (current_left >= indices[i] && current_right <= indices[i])
				{
					this->tree[index] = this->op.assign(elements[i], indices[i]);
					continue;
				}

				// Create nodes lazily
				this->_create(index);
				this->up.push(index);

				this->st.push(this->info[index].left);
				this->st.push(this->info[index].right);
			}

			// Join the updated nodes
			while (this->up.size() != 0)
			{
				this->_join(this->up.top());
				this->up.pop();
			}
		}
	}

	template <typename U, typename... args>
	sparse_segment_tree(const vector<U> &elements, const vector<uint32_t> &indices, range_t begin, range_t end, args &&...arg)
		: op(std::forward<args>(arg)...)
	{
		this->begin = begin;
		this->end = end;

		this->_build(elements, indices, begin, end);
	}

	template <typename... args>
	sparse_segment_tree(range_t begin, range_t end, args &&...arg) : op(std::forward<args>(arg)...)
	{
		node root = {};

		this->begin = begin;
		this->end = end;

		// Create the root
		root.begin = begin;
		root.end = end;
		root.left = 0;
		root.right = 0;

		this->info.push_back(root);
		this->tree.push_back(this->op.identity());

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}
	}

	T all()
	{
		return this->tree[0];
	}

	template <typename U>
	void update(range_t left, range_t right, const U &element)
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
			range_t current_left = this->info[index].begin;
			range_t current_right = this->info[index].end;

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

				if constexpr (!std::is_empty_v<L>)
				{
					this->tree[index] = this->op.assign(element, index);
				}
				else
				{
					this->_apply(index, element);
				}

				continue;
			}

			// Create nodes lazily
			this->_create(index);

			// Push the updates lazily
			if constexpr (!std::is_empty_v<L>)
			{
				this->_push(index);
			}

			this->up.push(index);

			this->st.push(this->info[index].left);
			this->st.push(this->info[index].right);
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}
	}

	T query(range_t left, range_t right)
	{
		T result = this->op.identity();

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
			range_t current_left = this->info[index].begin;
			range_t current_right = this->info[index].end;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				result = this->op.join(this->tree[index], result);
				continue;
			}

			// Create nodes
			this->_create(index);

			// Push updates
			if constexpr (!std::is_empty_v<L>)
			{
				this->_push(index);
			}

			this->st.push(this->info[index].left);
			this->st.push(this->info[index].right);
		}

		return result;
	}
};

template <typename T, typename L, typename O>
	requires segtree_operator_ext<O, T, L>
struct persistent_segment_tree
{
	using range_t = uint32_t;

	struct node
	{
		range_t begin, end;   // responsibility
		uint32_t left, right; // children
		uint32_t parent;

		node()
		{
			begin = 0, end = 0, left = 0, right = 0, parent = 0;
		}
	};

	vector<node> info;
	vector<T> tree;
	vector<L> lazy;

	map<uint32_t, uint32_t> roots;
	uint32_t recent;

	stack<array<uint32_t, 3>> cm;
	stack<uint32_t> st;
	stack<uint32_t> up;

	range_t begin;
	range_t end;

	O op;

	void _join(uint32_t index)
	{
		if (this->_leaf(index))
		{
			return;
		}

		// Join here
		this->tree[index] = this->op.join(this->tree[this->info[index].left], this->tree[this->info[index].right]);
	}

	void _apply(uint32_t index, const L &element)
		requires(!std::is_empty_v<L>)
	{
		// Apply to current node
		this->tree[index] = this->op.apply(this->tree[index], element, this->info[index].begin, this->info[index].end);

		// Push to children
		this->lazy[index] = this->op.compose(this->lazy[index], element);
	}

	bool _leaf(uint32_t index)
	{
		if (this->info[index].begin == this->info[index].end)
		{
			return true;
		}

		return false;
	}

	uint32_t _duplicate(uint32_t index, uint32_t parent, uint32_t lr)
	{
		uint32_t new_index = this->info.size();

		node dup = this->info[index];
		T el = this->tree[index];
		L lz = this->lazy[index];

		this->info.push_back(this->info[index]);
		this->tree.push_back(this->tree[index]);

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->lazy[index]);
		}

		if (parent != 0)
		{
			if (lr == 0)
			{
				this->info[parent].left = new_index;
			}
			else
			{
				this->info[parent].right = new_index;
			}

			this->info[new_index].parent = parent;
		}
		else
		{
			this->recent = new_index;
		}

		return new_index;
	}

	void _create(uint32_t index)
	{
		node left = {}, right = {};

		if (this->_leaf(index))
		{
			return;
		}

		if (this->info[index].left != 0 && this->info[index].right != 0)
		{
			return;
		}

		// left
		left.begin = this->info[index].begin;
		left.end = (this->info[index].begin + this->info[index].end) / 2;
		left.parent = index;

		this->info[index].left = this->info.size();
		this->info.push_back(left);
		this->tree.push_back(this->op.identity());

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}

		// right
		right.begin = left.end + 1;
		right.end = this->info[index].end;
		right.parent = index;

		this->info[index].right = this->info.size();
		this->info.push_back(right);
		this->tree.push_back(this->op.identity());

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}
	}

	void _push(uint32_t index)
		requires(!std::is_empty_v<L>)
	{
		uint32_t left = this->info[index].left;
		uint32_t right = this->info[index].right;

		if (this->_leaf(index))
		{
			this->lazy[index] = this->op.reset();
			return;
		}

		if (this->info[left].parent != index)
		{
			this->_duplicate(left, index, 0);
		}

		if (this->info[right].parent != index)
		{
			this->_duplicate(right, index, 1);
		}

		this->_apply(this->info[index].left, this->lazy[index]);
		this->_apply(this->info[index].right, this->lazy[index]);

		this->lazy[index] = this->op.reset();
	}

	void _sentinel()
	{
		node sentinel = {};

		sentinel.begin = begin;
		sentinel.end = end;

		this->info.push_back(sentinel);
		this->tree.push_back(this->op.identity());
		this->roots.insert({UINT32_MAX, 0});

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}
	}

	template <typename U>
	void _build(const vector<U> &elements, const vector<uint32_t> &indices, range_t begin, range_t end)
	{
		// Create the root
		node root = {};
		uint32_t count = indices.size();

		root.begin = begin;
		root.end = end;

		this->info.push_back(root);
		this->tree.push_back(this->op.identity());
		this->roots.insert({UINT32_MAX, 0});

		if constexpr (!std::is_empty_v<L>)
		{
			this->lazy.push_back(this->op.reset());
		}

		for (uint32_t i = 0; i < count; ++i)
		{
			this->st.push(0);

			while (this->st.size() != 0)
			{
				uint32_t index = this->st.top();
				range_t current_left = this->info[index].begin;
				range_t current_right = this->info[index].end;

				this->st.pop();

				if (current_right < indices[i] || current_left > indices[i])
				{
					continue;
				}

				if (current_left >= indices[i] && current_right <= indices[i])
				{
					this->tree[index] = this->op.assign(elements[i], indices[i]);
					continue;
				}

				// Create nodes lazily
				this->_create(index);
				this->up.push(index);

				this->st.push(this->info[index].left);
				this->st.push(this->info[index].right);
			}

			// Join the updated nodes
			while (this->up.size() != 0)
			{
				this->_join(this->up.top());
				this->up.pop();
			}
		}
	}

	template <typename U, typename... args>
	persistent_segment_tree(const vector<U> &elements, const vector<uint32_t> &indices, range_t begin, range_t end, args &&...arg)
		: op(std::forward<args>(arg)...)
	{
		this->begin = begin;
		this->end = end;

		this->_build(elements, indices, begin, end);
	}

	template <typename U, typename... args>
	persistent_segment_tree(const vector<U> &elements, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->begin = 0;
		this->end = elements.size() - 1;

		vector<uint32_t> indices(elements.size());

		for (uint32_t i = 0; i < indices.size(); ++i)
		{
			indices[i] = i;
		}

		this->_build(elements, indices, this->begin, this->end);
	}

	template <typename... args>
	persistent_segment_tree(range_t begin, range_t end, args &&...arg) : op(std::forward<args>(arg)...)
	{
		this->begin = begin;
		this->end = end;

		this->_sentinel();
	}

	T all(uint32_t base)
	{
		if (!this->roots.contains(base))
		{
			return this->op.identity();
		}

		return this->tree[this->roots[base]];
	}

	void copy(uint32_t base, uint32_t update)
	{
		uint32_t index = 0;

		if (this->roots.contains(base))
		{
			index = this->roots[base];
		}
		else
		{
			index = this->roots[UINT32_MAX];
		}

		index = this->_duplicate(index, 0, 0);
		this->roots.insert({update, index});
	}

	template <typename U>
	void update(uint32_t base, uint32_t update, range_t left, range_t right, const U &element)
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
			range_t current_left = this->info[index].begin;
			range_t current_right = this->info[index].end;

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

				if constexpr (!std::is_empty_v<L>)
				{
					new_index = this->_duplicate(index, parent, lr));
					this->tree[new_index] = this->op.assign(element, new_index);
				}
				else
				{
					this->_apply(this->_duplicate(index, parent, lr), element);
				}

				continue;
			}

			// Create nodes lazily
			this->_create(index);

			// Push the updates lazily
			if constexpr (!std::is_empty_v<L>)
			{
				this->_push(index);
			}

			// Create new nodes for updated path
			this->up.push(new_index = this->_duplicate(index, parent, lr));

			this->cm.push({this->info[new_index].left, new_index, 0});
			this->cm.push({this->info[new_index].right, new_index, 1});
		}

		// Join the updated nodes
		while (this->up.size() != 0)
		{
			this->_join(this->up.top());
			this->up.pop();
		}

		// Add new root
		this->roots[update] = this->recent;
	}

	T query(uint32_t base, range_t left, range_t right)
	{
		T result = this->op.identity();

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
			range_t current_left = this->info[index].begin;
			range_t current_right = this->info[index].end;

			this->st.pop();

			if (current_right < left || current_left > right)
			{
				continue;
			}

			if (current_left >= left && current_right <= right)
			{
				result = this->op.join(this->tree[index], result);
				continue;
			}

			// Create nodes
			this->_create(index);

			// Push updates
			if constexpr (!std::is_empty_v<L>)
			{
				this->_push(index);
			}

			this->st.push(this->info[index].left);
			this->st.push(this->info[index].right);
		}

		return result;
	}
};

struct segment_tree_2d
{
	using range_t = uint32_t;

	struct segment_tree_node
	{
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

		range_t begin, end;
		range_t pbegin, pend;

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

		segment_tree_node(range_t pbegin, range_t pend, range_t begin, range_t end)
		{
			this->begin = begin;
			this->end = end;
			this->pbegin = pbegin;
			this->pend = pend;

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

	struct node
	{
		segment_tree_node element;
		range_t begin, end;   // responsibility
		uint32_t left, right; // children

		node(range_t begin, range_t end, range_t sbegin, range_t send) : element(begin, end, sbegin, send)
		{
			begin = 0, end = 0, left = 0, right = 0;
		}
	};

	struct update
	{
		uint64_t value;
		range_t begin, end;
	};

	vector<node> tree;
	vector<vector<update>> lazy;

	stack<uint32_t> st;

	range_t x_begin, x_end;
	range_t y_begin, y_end;

	void _apply(uint32_t index, const update &element)
	{
		// Apply to current node
		this->tree[index].element.range_update(element.begin, element.end);

		// Push to children
		this->lazy[index].push_back(element);
	}

	bool _skip(uint32_t index)
	{
		// Condition for skipping push
		if (this->lazy[index].size() == 0)
		{
			return true;
		}

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
		uint32_t begin = 0, end = 0;

		if (this->_leaf(index))
		{
			return;
		}

		if (this->tree[index].left != 0 && this->tree[index].right != 0)
		{
			return;
		}

		// left
		begin = this->tree[index].begin;
		end = (this->tree[index].begin + this->tree[index].end) / 2;

		node left = node(begin, end, this->y_begin, this->y_end);
		left.begin = begin;
		left.end = end;

		this->tree[index].left = this->tree.size();
		this->tree.push_back(left);
		this->lazy.push_back({});

		// right
		begin = end + 1;
		end = this->tree[index].end;

		node right = node(begin, end, this->y_begin, this->y_end);
		right.begin = begin;
		right.end = end;

		this->tree[index].right = this->tree.size();
		this->tree.push_back(right);
		this->lazy.push_back({});
	}

	void _push(uint32_t index)
	{
		if (this->_leaf(index))
		{
			this->lazy[index].clear();
			return;
		}

		if (this->_skip(index))
		{
			return;
		}

		for (uint32_t i = 0; i < this->lazy[index].size(); ++i)
		{
			this->_apply(this->tree[index].left, this->lazy[index][i]);
			this->_apply(this->tree[index].right, this->lazy[index][i]);
		}

		this->lazy[index].clear();
	}

	void _build(range_t x_begin, range_t x_end, range_t y_begin, range_t y_end)
	{
		// Create the root
		node root = node(x_begin, x_end, y_begin, y_end);

		root.begin = x_begin;
		root.end = x_end;
		root.left = 0;
		root.right = 0;

		this->tree.push_back(root);
		this->lazy.push_back({});
	}

	segment_tree_2d(range_t x_begin, range_t x_end, range_t y_begin, range_t y_end)
	{
		this->x_begin = x_begin;
		this->x_end = y_end;
		this->y_begin = y_begin;
		this->y_end = y_end;

		this->_build(x_begin, x_end, y_begin, y_end);
	}

	void range_update(range_t x_left, range_t x_right, range_t y_left, range_t y_right)
	{
		if (x_left > this->x_end)
		{
			x_left = this->x_begin;
		}

		if (x_right > this->x_end)
		{
			x_right = this->x_end;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			range_t current_left = this->tree[index].begin;
			range_t current_right = this->tree[index].end;

			this->st.pop();

			if (current_right < x_left || current_left > x_right)
			{
				continue;
			}

			if (current_left >= x_left && current_right <= x_right)
			{
				// For beats
				// Determine when all of the nodes are affected
				// Determine when none of the nodes are affected

				this->_apply(index, {y_left, y_right, 0});
				continue;
			}

			// Create nodes lazily
			this->_create(index);

			// Push the updates lazily
			this->_push(index);

			this->st.push(this->tree[index].left);
			this->st.push(this->tree[index].right);
		}
	}

	uint32_t range_query(range_t x_left, range_t x_right, range_t y_left, range_t y_right)
	{
		uint32_t result = 0;

		if (x_left > this->x_end)
		{
			x_left = this->x_begin;
		}

		if (x_right > this->x_end)
		{
			x_right = this->x_end;
		}

		this->st.push(0);

		while (this->st.size() != 0)
		{
			uint32_t index = this->st.top();
			range_t current_left = this->tree[index].begin;
			range_t current_right = this->tree[index].end;

			this->st.pop();

			if (current_right < x_left || current_left > x_right)
			{
				continue;
			}

			if (current_left >= x_left && current_right <= x_right)
			{
				result += this->tree[index].element.range_query(y_left, y_right);
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
