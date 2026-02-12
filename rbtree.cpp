#include <concepts>
#include <vector>

using namespace std;

template <typename KEY, typename VALUE = void, typename PRIORITY = void, bool DUPLICATES = false>
struct rbtree
{
	using key_type = KEY;
	using value_type = VALUE;
	using priority_type = PRIORITY;

	// Variable definitions
	// key -> key in set or map
	// value -> value associated with key
	// priority -> augmented value of node derived from value or key
	// current -> combined augmented value of node and its subtrees
	// node -> optional pointer to node responsible for current
	// size -> number of nodes in the subtree including self
	// color -> 0 for black, 1 for red
	// parent -> parent of node
	// left -> left child of node
	// right -> right child of node

	struct rbnode_set
	{
		key_type key = {};

		uint32_t size : 31 = 0;
		uint8_t color : 1 = 0;

		rbnode_set *parent = nullptr, *left = nullptr, *right = nullptr;
	};

	struct rbnode_map
	{
		key_type key = {};
		value_type value = {};

		uint32_t size : 31 = 0;
		uint8_t color : 1 = 0;

		rbnode_map *parent = nullptr, *left = nullptr, *right = nullptr;
	};

	struct rbnode_set_ext
	{
		key_type key = {};
		priority_type priority = {};
		priority_type current = {};

		uint32_t size : 31 = 0;
		uint8_t color : 1 = 0;

		// rbnode_set_ext *node = nullptr;
		rbnode_set_ext *parent = nullptr, *left = nullptr, *right = nullptr;
	};

	struct rbnode_map_ext
	{
		key_type key = {};
		value_type value = {};
		priority_type priority = {};
		priority_type current = {};

		uint32_t size : 31 = 0;
		uint8_t color : 1 = 0;

		// rbnode_map_ext *node = nullptr;
		rbnode_map_ext *parent = nullptr, *left = nullptr, *right = nullptr;
	};

	// Choose the correct node type based on templates
	using rbnode =
		conditional_t<is_same_v<VALUE, void> && is_same_v<PRIORITY, void>, rbnode_set,
					  conditional_t<!is_same_v<VALUE, void> && is_same_v<PRIORITY, void>, rbnode_map,
									conditional_t<is_same_v<VALUE, void> && !is_same_v<PRIORITY, void>, rbnode_set_ext, rbnode_map_ext>>>;

	// Concepts for use
	template <typename CONTAINER>
	static constexpr bool clearable = requires(CONTAINER container) { container.clear(); };

	template <typename CONTAINER, typename TYPE>
	static constexpr bool insertable = requires(CONTAINER container, TYPE type) { container.insert(type); };

	vector<rbnode *> _pool; // pool of all allocated nodes
	vector<rbnode *> _free; // pool of nodes that can be resused

	rbnode *_root;   // root of tree
	uint32_t _count; // number of nodes in tree

	rbtree()
	{
		this->_root = nullptr;
		this->_count = 0;
	}

	~rbtree()
	{
		this->_clear();

		this->_root = nullptr;
		this->_count = 0;
	}

	rbnode *_alloc_node()
	{
		rbnode *node = nullptr;

		this->_count += 1;

		// Reuse any nodes first before allocating new ones
		if (this->_free.size() != 0)
		{
			node = this->_free.back();
			this->_free.pop_back();
		}
		else
		{
			node = new rbnode;
			this->_pool.push_back(node);
		}

		// Always create a red node
		node->size = 1;
		node->color = 1;

		node->left = nullptr;
		node->right = nullptr;
		node->parent = nullptr;

		// Default constructor for complex types
		if constexpr (!is_same_v<VALUE, void>)
		{
			node->value = {};
		}

		if constexpr (!is_same_v<PRIORITY, void>)
		{
			node->priority = {};
			node->current = {};
		}

		return node;
	}

	void _free_node(rbnode *node)
	{
		if (node == nullptr)
		{
			return;
		}

		this->_count -= 1;
		this->_free.push_back(node);
	}

	void _clear()
	{
		while (this->_pool.size() != 0)
		{
			rbnode *node = this->_pool.back();

			// Clear containers
			if constexpr (clearable<KEY>)
			{
				node->key.clear();
			}

			if constexpr (!is_same_v<VALUE, void> && clearable<VALUE>)
			{
				node->value.clear();
			}

			if constexpr (!is_same_v<PRIORITY, void> && clearable<PRIORITY>)
			{
				node->priority.clear();
			}

			this->_pool.pop_back();
			delete node;
		}

		// Clear the node pools
		this->_pool.clear();
		this->_free.clear();
	}

	bool _color(rbnode *node)
	{
		// Undefined nodes are always black
		if (node == nullptr)
		{
			return 0;
		}

		return node->color;
	}

	priority_type _priority(rbnode *node)
		requires(!std::is_same_v<PRIORITY, void>)
	{
		// Assign priority based on value or key here
		return 0;
	}

	void _size(rbnode *n)
	{
		uint32_t count = 1;

		if (n == nullptr)
		{
			return;
		}

		count += n->left != nullptr ? n->left->size : 0;
		count += n->right != nullptr ? n->right->size : 0;

		n->size = count;
	}

	void _join(rbnode *node)
	{
		if constexpr (!is_same_v<PRIORITY, void>)
		{
			node->priority = this->_priority(node);
			node->current = node->priority;

			if (node->left != nullptr)
			{
				node->current = MIN(node->current, node->left->current);
			}

			if (node->right != nullptr)
			{
				node->current = MIN(node->current, node->right->current);
			}
		}
	}

	void _left_rotate(rbnode *node)
	{
		rbnode *temp = nullptr;

		// The right child of node will always exist.
		//
		//  node = X
		//  temp = Y
		//
		//     X                      Y
		//    / \                    / \
		//   a   Y     --->         X   c
		//      / \                / \
		//     b   c              a   b
		//

		temp = node->right;
		node->right = temp->left;

		if (temp->left != nullptr)
		{
			temp->left->parent = node;
		}

		temp->parent = node->parent;

		if (node->parent == nullptr)
		{
			this->_root = temp;
		}
		else
		{
			if (node->parent->left == node)
			{
				node->parent->left = temp;
			}
			else
			{
				node->parent->right = temp;
			}
		}

		temp->left = node;
		node->parent = temp;

		// Update statisitics
		this->_size(node);
		this->_join(node);

		this->_size(temp);
		this->_join(temp);
	}

	void _right_rotate(rbnode *node)
	{
		rbnode *temp = nullptr;

		// The left child of node will always exist.
		//
		//  node = X
		//  temp = Y
		//
		//       X                      Y
		//      / \                    / \
		//     Y   a      --->        b   X
		//    / \                        / \
		//   b   c                      c   a
		//

		temp = node->left;
		node->left = temp->right;

		if (temp->right != nullptr)
		{
			temp->right->parent = node;
		}

		temp->parent = node->parent;

		if (node->parent == nullptr)
		{
			this->_root = temp;
		}
		else
		{
			if (node->parent->left == node)
			{
				node->parent->left = temp;
			}
			else
			{
				node->parent->right = temp;
			}
		}

		temp->right = node;
		node->parent = temp;

		// Update statisitics
		this->_size(node);
		this->_join(node);

		this->_size(temp);
		this->_join(temp);
	}

	void _transplant(rbnode *u, rbnode *v)
	{
		// Move v to u's position and only update the parent of u
		// Status of u's children and v's children are handled by the caller
		// NOTE: u should always exist
		// NOTE: v can be null

		if (u->parent == nullptr)
		{
			this->_root = v;
		}
		else
		{
			if (u == u->parent->left)
			{
				u->parent->left = v;
			}
			else
			{
				u->parent->right = v;
			}
		}

		if (v != nullptr)
		{
			v->parent = u->parent;
		}
	}

	pair<rbnode *, uint8_t> _insert_find(key_type key)
	{
		// Find a suitable position for the node and update its parent
		rbnode *node = this->_root;
		rbnode *temp = nullptr;

		while (node != nullptr)
		{
			temp = node;

			if (key < node->key)
			{
				node = node->left;
			}
			else
			{
				if (key == node->key)
				{
					if constexpr (DUPLICATES == false)
					{
						return {node, 0};
					}
				}

				node = node->right;
			}
		}

		node = this->_alloc_node();

		node->key = key;
		node->parent = temp;

		if (temp == nullptr)
		{
			this->_root = node;
			this->_root->color = 0;

			return {node, 0};
		}

		if (node->key < temp->key)
		{
			temp->left = node;
		}
		else
		{
			temp->right = node;
		}

		return {node, 1};
	}

	void _insert_fixup(rbnode *node)
	{
		// Fix any violations that can have occurred during insertion
		// Only property that will be violated is a red node having a red parent
		while (this->_color(node->parent))
		{
			// NOTE: grandparent will always exist here as parent will not be the root node
			rbnode *parent = node->parent;
			rbnode *grandparent = parent->parent;
			rbnode *uncle = nullptr;

			// Check the color parent's sibling, since the tree was valid before insertion
			// the grandparent will be black and the sibling will always exist.
			// If both parent and its sibling are red, change them to black,
			// set grandparent to red repeat with grandparent's ancestors
			// If the colors differ do a series of rotations (atmost 2) to fix the black height and terminate
			if (parent == grandparent->left)
			{
				uncle = grandparent->right;

				if (this->_color(uncle))
				{
					parent->color = 0;
					uncle->color = 0;
					grandparent->color = 1;

					node = grandparent;
				}
				else
				{
					if (node == parent->right)
					{
						node = parent;
						this->_left_rotate(node);
					}

					parent = node->parent;
					grandparent = parent->parent;

					parent->color = 0;
					grandparent->color = 1;

					this->_right_rotate(grandparent);

					break;
				}
			}
			else
			{
				uncle = grandparent->left;

				if (this->_color(uncle))
				{
					parent->color = 0;
					uncle->color = 0;
					grandparent->color = 1;

					node = grandparent;
				}
				else
				{
					if (node == parent->left)
					{
						node = parent;
						this->_right_rotate(node);
					}

					parent = node->parent;
					grandparent = parent->parent;

					parent->color = 0;
					grandparent->color = 1;

					this->_left_rotate(grandparent);

					break;
				}
			}
		}

		this->_root->color = 0;
	}

	rbnode *insert(key_type key)
	{
		auto [node, fixup] = this->_insert_find(key);
		rbnode *parent = node->parent;

		this->_join(node);

		while (parent != nullptr)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		if (fixup)
		{
			this->_insert_fixup(node);
		}

		return node;
	}

	template <typename TYPE>
	rbnode *insert(key_type key, TYPE value)
		requires(!std::is_same_v<VALUE, void>)
	{
		auto [node, fixup] = this->_insert_find(key);
		rbnode *parent = node->parent;

		node->value = value;
		this->_join(node);

		while (parent != nullptr)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		if (fixup)
		{
			this->_insert_fixup(node);
		}

		return node;
	}

	template <typename TYPE>
	rbnode *add(key_type key, TYPE value)
		requires(!std::is_same_v<VALUE, void> && insertable<VALUE, TYPE>)
	{
		auto [node, fixup] = this->_insert_find(key);
		rbnode *parent = node->parent;

		node->value.insert(value);
		this->_join(node);

		while (parent != nullptr)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		if (fixup)
		{
			this->_insert_fixup(node);
		}

		return node;
	}

	template <typename TYPE>
	rbnode *remove(key_type key, TYPE value)
		requires(!std::is_same_v<VALUE, void> && insertable<VALUE, TYPE>)
	{
		rbnode *node = this->find(key);
		rbnode *parent = nullptr;

		if (node == nullptr)
		{
			return nullptr;
		}

		node->value.erase(node->value.find(value));

		if (node->value.size() == 0)
		{
			this->erase(node);
			return nullptr;
		}

		this->_join(node);
		parent = node->parent;

		while (parent != nullptr)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		return node;
	}

	void erase(rbnode *node)
	{
		rbnode *successor = nullptr;
		rbnode *parent = nullptr;
		rbnode *temp = nullptr;
		uint8_t color = 0;

		if (node == nullptr)
		{
			return;
		}

		color = node->color;

		if (node->left != nullptr && node->right != nullptr)
		{
			// If node contains both children, its successor will be leftmost child of its right child
			// The successor will not have a left child but may have a right child.
			// Transplant the successor to the node, taking on its color and set the new successor as the right child.
			// NOTE: The new successor might be null, we only care about its parent
			rbnode *min = node->right;

			while (min->left != nullptr)
			{
				min = min->left;
			}

			color = min->color;
			parent = min->parent;
			successor = min->right;

			if (min != node->right)
			{
				this->_transplant(min, min->right);
				temp = min->parent;

				// Update statistics from successor to node
				while (parent != node)
				{
					this->_size(parent);
					this->_join(parent);

					parent = parent->parent;
				}

				min->right = node->right;
				node->right->parent = min;
			}
			else
			{
				temp = min;
			}

			this->_transplant(node, min);

			min->left = node->left;
			min->left->parent = min;
			min->color = node->color;

			this->_size(min);
			this->_join(min);
		}
		else
		{
			temp = node->parent; // In case if both children are null

			if (node->left == nullptr)
			{
				successor = node->right;
				this->_transplant(node, successor);
			}
			else
			{
				successor = node->left;
				this->_transplant(node, successor);
			}
		}

		// Update statistics to root
		parent = node->parent;

		while (parent != nullptr)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		this->_free_node(node);

		if (color)
		{
			return;
		}

		parent = temp;

		// Fix any violations in black height that could have occured
		while (successor != this->_root && this->_color(successor) == 0)
		{
			rbnode *sibling = nullptr;

			// It is guaranteed that the parent of the successor will not be null during any of the iterations.
			// Update the parent on subsequent iteration after we update the successor to a valid node
			if (successor != nullptr)
			{
				parent = successor->parent;
			}

			if (successor == parent->left)
			{
				// NOTE: The sibling can be null here. In that case its children are all black and we repeat
				// the loop with parent's ancestors
				sibling = parent->right;

				if (this->_color(sibling))
				{
					// This guarantees sibling exists, so the rotation is valid
					sibling->color = 0;
					parent->color = 1;

					this->_left_rotate(parent);
					sibling = parent->right;
				}

				if (sibling == nullptr || (this->_color(sibling->left) == 0 && this->_color(sibling->right) == 0))
				{
					if (sibling != nullptr)
					{
						sibling->color = 1;
					}

					successor = parent;
				}
				else
				{
					// The sibling exists and has atleast one child which is red,
					// perform rotations and terminate.
					if (this->_color(sibling->right) == 0)
					{
						sibling->left->color = 0;
						sibling->color = 1;

						this->_right_rotate(sibling);
						sibling = parent->right;
					}

					sibling->color = parent->color;
					parent->color = 0;
					sibling->right->color = 0;

					this->_left_rotate(parent);
					successor = this->_root;
					break;
				}
			}
			else
			{
				sibling = parent->left;

				if (this->_color(sibling))
				{
					sibling->color = 0;
					parent->color = 1;

					this->_right_rotate(parent);
					sibling = parent->left;
				}

				if (sibling == nullptr || (this->_color(sibling->left) == 0 && this->_color(sibling->right) == 0))
				{
					if (sibling != nullptr)
					{
						sibling->color = 1;
					}

					successor = parent;
				}
				else
				{
					if (this->_color(sibling->left) == 0)
					{
						sibling->right->color = 0;
						sibling->color = 1;

						this->_left_rotate(sibling);
						sibling = parent->left;
					}

					sibling->color = parent->color;
					parent->color = 0;
					sibling->left->color = 0;

					this->_right_rotate(parent);
					successor = this->_root;
					break;
				}
			}
		}

		// NOTE: This step is important.
		// When the loop terminates, set the current successor to black to maintain
		// red-black properties
		if (successor != nullptr)
		{
			successor->color = 0;
		}
	}

	void erase(key_type key)
	{
		return this->erase(this->find(key));
	}

	void clear()
	{
		this->_clear();
	}

	void update(rbnode *node)
		requires(!std::is_same_v<PRIORITY, void>)
	{
		if (node == nullptr)
		{
			return;
		}

		node->priority = this->_priority(node);
		node->current = node->priority;

		while (node != nullptr)
		{
			this->_join(node);
			node = node->parent;
		}
	}

	uint32_t query(key_type constraint)
		requires(!std::is_same_v<PRIORITY, void>)
	{
		// Perform a tree walk
		rbnode *node = this->_root;
		rbnode *final = nullptr;
		uint32_t result = UINT32_MAX;

		// Copy the find_* template
		// Example gte
#if 0
		while (node != nullptr)
		{
			if (constraint <= node->key)
			{
				result = MIN(result, node->priority);

				if (node->right != nullptr)
				{
					result = MIN(result, node->right->current);
				}

				final = node;
				node = node->left;
			}
			else
			{
				node = node->right;
			}
		}

		if (final != nullptr)
		{
			if (final->right != nullptr)
			{
				result = MIN(result, final->right->current);
			}
		}
#endif

		// Example lte
#if 0
		while (node != nullptr)
		{
			if (constraint < node->key)
			{

				node = node->left;
			}
			else
			{
				result = MIN(result, node->priority);

				if (node->left != nullptr)
				{
					result = MIN(result, node->left->current);
				}

				final = node;
				node = node->right;
			}
		}

		if (final != nullptr)
		{
			if (final->left != nullptr)
			{
				result = MIN(result, final->left->current);
			}
		}
#endif

		return result;
	}

	rbnode *find(key_type key)
	{
		rbnode *node = this->_root;

		while (node != nullptr)
		{
			if (key == node->key)
			{
				return node;
			}

			if (key < node->key)
			{
				node = node->left;
			}
			else
			{
				node = node->right;
			}
		}

		return nullptr;
	}

	// Get the node based on its rank
	rbnode *get(uint32_t order)
	{
		rbnode *node = this->_root;

		if (order >= this->_count)
		{
			return nullptr;
		}

		while (node != nullptr)
		{
			uint32_t count = 1;

			if (node->left != nullptr)
			{
				count += node->left->size;
			}

			if (count == (order + 1))
			{
				return node;
			}

			if (count > (order + 1))
			{
				node = node->left;
			}
			else
			{
				node = node->right;
				order -= count;
			}
		}

		return nullptr;
	}

	// Get the rank of node (0 based)
	uint32_t order(rbnode *node)
	{
		uint32_t count = 0;

		if (node == nullptr)
		{
			return UINT32_MAX;
		}

		if (node->left != nullptr)
		{
			count += node->left->size;
		}

		while (node != nullptr)
		{
			if (node->parent != nullptr)
			{
				if (node->parent->right == node)
				{
					count += 1;

					if (node->parent->left != nullptr)
					{
						count += node->parent->left->size;
					}
				}
			}

			node = node->parent;
		}

		return count;
	}

	uint8_t contains(key_type key)
	{
		return this->find(key) != nullptr;
	}

	uint32_t size()
	{
		return this->_count;
	}

	// Get the first element
	rbnode *front()
	{
		rbnode *node = this->_root;

		if (node == nullptr)
		{
			return nullptr;
		}

		while (node->left != nullptr)
		{
			node = node->left;
		}

		return node;
	}

	// Get the last element
	rbnode *back()
	{
		rbnode *node = this->_root;

		if (node == nullptr)
		{
			return nullptr;
		}

		while (node->right != nullptr)
		{
			node = node->right;
		}

		return node;
	}

	// Get the next element in order
	rbnode *next(rbnode *node)
	{
		rbnode *temp = nullptr;

		if (node == nullptr || node == nullptr)
		{
			return nullptr;
		}

		if (node->right != nullptr)
		{
			temp = node->right;

			while (temp->left != nullptr)
			{
				temp = temp->left;
			}

			return temp;
		}

		while (node->parent != nullptr)
		{
			temp = node->parent;

			if (temp->right == node)
			{
				node = temp;
				continue;
			}

			return temp;
		}

		return nullptr;
	}

	// Get the previous element in order
	rbnode *prev(rbnode *node)
	{
		rbnode *temp = nullptr;

		if (node == nullptr)
		{
			return nullptr;
		}

		if (node->left != nullptr)
		{
			temp = node->left;

			while (temp->right != nullptr)
			{
				temp = temp->right;
			}

			return temp;
		}

		while (node->parent != nullptr)
		{
			temp = node->parent;

			if (temp->left == node)
			{
				node = temp;
				continue;
			}

			return temp;
		}

		return nullptr;
	}

	// Find the largest key lesser than given key
	rbnode *find_lt(key_type key)
	{
		rbnode *node = this->_root;
		rbnode *result = nullptr;

		while (node != nullptr)
		{
			if (key <= node->key)
			{
				node = node->left;
			}
			else
			{
				result = node;
				node = node->right;
			}
		}

		return result;
	}

	rbnode *find_lte(key_type key)
	{
		rbnode *node = this->_root;
		rbnode *result = nullptr;

		while (node != nullptr)
		{
			if (key < node->key)
			{
				node = node->left;
			}
			else
			{
				result = node;
				node = node->right;
			}
		}

		return result;
	}

	// Find the smallest key greater than given key
	rbnode *find_gt(key_type key)
	{
		rbnode *node = this->_root;
		rbnode *result = nullptr;

		while (node != nullptr)
		{
			if (key < node->key)
			{
				result = node;
				node = node->left;
			}
			else
			{
				node = node->right;
			}
		}

		return result;
	}

	rbnode *find_gte(key_type key)
	{
		rbnode *node = this->_root;
		rbnode *result = nullptr;

		while (node != nullptr)
		{
			if (key <= node->key)
			{
				result = node;
				node = node->left;
			}
			else
			{
				node = node->right;
			}
		}

		return result;
	}

	// Count the number of elements lesser than given key
	uint32_t count_lt(key_type key)
	{
		rbnode *node = this->find_lt(key);

		if (node == nullptr)
		{
			return 0;
		}

		return this->order(node) + 1;
	}

	uint32_t count_lte(key_type key)
	{
		rbnode *node = this->find_lte(key);

		if (node == nullptr)
		{
			return 0;
		}

		return this->order(node) + 1;
	}

	// Count the number of elements greater than given key
	uint32_t count_gt(key_type key)
	{
		rbnode *node = this->find_gt(key);

		if (node == nullptr)
		{
			return 0;
		}

		return this->size() - this->order(node);
	}

	uint32_t count_gte(key_type key)
	{
		rbnode *node = this->find_gte(key);

		if (node == nullptr)
		{
			return 0;
		}

		return this->size() - this->order(node);
	}
};

template <typename KEY>
using ordered_set = rbtree<KEY, void, void, false>;

template <typename KEY>
using ordered_multiset = rbtree<KEY, void, void, true>;

template <typename KEY, typename VALUE>
using ordered_map = rbtree<KEY, VALUE, void, false>;

template <typename KEY, typename VALUE>
using ordered_multimap = rbtree<KEY, VALUE, void, true>;

template <typename KEY, typename PRIORITY>
using augmented_set = rbtree<KEY, void, PRIORITY, false>;

template <typename KEY, typename PRIORITY>
using augmented_multiset = rbtree<KEY, void, PRIORITY, true>;

template <typename KEY, typename VALUE, typename PRIORITY>
using augmented_map = rbtree<KEY, VALUE, PRIORITY, false>;

template <typename KEY, typename VALUE, typename PRIORITY>
using augmented_multimap = rbtree<KEY, VALUE, PRIORITY, true>;
