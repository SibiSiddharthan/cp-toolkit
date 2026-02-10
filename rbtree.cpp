#include <concepts>
#include <vector>

using namespace std;

template <typename K, typename V = void, typename P = void, bool DUPLICATES = false>
struct rbtree
{
	using key_type = K;
	using value_type = V;
	using priority_type = P;

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

	using rbnode = conditional_t<is_same_v<V, void> && is_same_v<P, void>, rbnode_set,
								 conditional_t<!is_same_v<V, void> && is_same_v<P, void>, rbnode_map,
											   conditional_t<is_same_v<V, void> && !is_same_v<P, void>, rbnode_set_ext, rbnode_map_ext>>>;

	template <typename T>
	static constexpr bool clearable = requires(T t) { t.clear(); };

	template <typename T>
	static constexpr bool insertable = requires(T t) { t.insert(); };

	vector<rbnode *> _pool; // pool of all allocated nodes
	vector<rbnode *> _free; // pool of nodes that can be resused

	rbnode *_nil;    // tree sentinel
	rbnode *_root;   // root of tree
	uint32_t _count; // number of nodes in tree

	rbtree()
	{
		// Allocate a sentinel
		this->_nil = (rbnode *)malloc(sizeof(rbnode));

		*(this->_nil) = {};

		this->_nil->size = 0;
		this->_nil->color = 0;

		this->_nil->left = this->_nil;
		this->_nil->right = this->_nil;
		this->_nil->parent = this->_nil;

		this->_pool.push_back(this->_nil);

		this->_root = this->_nil;
		this->_count = 0;
	}

	~rbtree()
	{
		this->_clear();

		// Free the nil
		free(this->_pool.back());
		this->_pool.pop_back();

		this->_free.clear();

		this->_root = nullptr;
		this->_count = 0;
	}

	rbnode *_alloc_node()
	{
		rbnode *n = nullptr;

		this->_count += 1;

		if (this->_free.size() != 0)
		{
			n = this->_free.back();
			this->_free.pop_back();
		}
		else
		{
			n = (rbnode *)malloc(sizeof(rbnode));
			this->_pool.push_back(n);
		}

		*n = {};

		n->size = 1;
		n->color = 1;

		n->left = this->_nil;
		n->right = this->_nil;
		n->parent = this->_nil;

		return n;
	}

	void _free_node(rbnode *n)
	{
		if (n == this->_nil)
		{
			return;
		}

		this->_count -= 1;
		this->_free.push_back(n);
	}

	void _clear()
	{
		// Remove everything except the nil node
		while (this->_pool.size() > 1)
		{
			rbnode *n = this->_pool.back();

			if constexpr (clearable<K>)
			{
				n->key.clear();
			}

			if constexpr (!is_same_v<V, void> && clearable<V>)
			{
				n->value.clear();
			}

			if constexpr (!is_same_v<P, void> && clearable<P>)
			{
				n->priority.clear();
			}

			free(n);
			this->_pool.pop_back();
		}
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
		requires(!std::is_same_v<P, void>)
	{
		// Assign priority based on value or key here
		return 0;
	}

	void _size(rbnode *n)
	{
		if (n == this->_nil)
		{
			return;
		}

		n->size = 1 + n->left->size + n->right->size;
	}

	void _join(rbnode *node)
	{
		if constexpr (!is_same_v<P, void>)
		{
			node->priority = this->_priority(node);
			node->current = node->priority;

			if (node->left != this->_nil)
			{
				node->current = MIN(node->current, node->left->current);
			}

			if (node->right != this->_nil)
			{
				node->current = MIN(node->current, node->right->current);
			}
		}
	}

	void _left_rotate(rbnode *n)
	{
		rbnode *t = n->right;

		n->right = t->left;

		if (t->left != this->_nil)
		{
			t->left->parent = n;
		}

		t->parent = n->parent;

		if (n->parent == this->_nil)
		{
			this->_root = t;
		}
		else
		{
			if (n->parent->left == n)
			{
				n->parent->left = t;
			}
			else
			{
				n->parent->right = t;
			}
		}

		t->left = n;
		n->parent = t;

		// Update the orders
		this->_size(n);
		this->_join(n);

		this->_size(t);
		this->_join(t);
	}

	void _right_rotate(rbnode *n)
	{
		rbnode *t = n->left;

		n->left = t->right;

		if (t->right != this->_nil)
		{
			t->right->parent = n;
		}

		t->parent = n->parent;

		if (n->parent == this->_nil)
		{
			this->_root = t;
		}
		else
		{
			if (n->parent->left == n)
			{
				n->parent->left = t;
			}
			else
			{
				n->parent->right = t;
			}
		}

		t->right = n;
		n->parent = t;

		// Update the orders
		this->_size(n);
		this->_join(n);

		this->_size(t);
		this->_join(t);
	}

	void _transplant(rbnode *u, rbnode *v)
	{
		if (u->parent == this->_nil)
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

		v->parent = u->parent;
	}

	rbnode *_insert_find(key_type key)
	{
		rbnode *node = this->_root;
		rbnode *temp = this->_nil;

		while (node != this->_nil)
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
						return node;
					}
				}

				node = node->right;
			}
		}

		node = this->_alloc_node();

		node->key = key;
		node->parent = temp;

		if (temp == this->_nil)
		{
			this->_root = node;
			this->_root->color = 0;

			return node;
		}

		if (node->key < temp->key)
		{
			temp->left = node;
		}
		else
		{
			temp->right = node;
		}

		return node;
	}

	void _insert_fixup(rbnode *n)
	{
		while (n->parent->color)
		{
			rbnode *p = n->parent;
			rbnode *gp = p->parent;
			rbnode *u = nullptr;

			if (p == gp->left)
			{
				u = gp->right;

				if (u->color)
				{
					p->color = 0;
					u->color = 0;
					gp->color = 1;

					n = gp;
				}
				else
				{
					if (n == p->right)
					{
						n = p;
						this->_left_rotate(n);
					}

					p = n->parent;
					gp = p->parent;

					p->color = 0;
					gp->color = 1;

					this->_right_rotate(gp);
				}
			}
			else
			{
				u = gp->left;

				if (u->color)
				{
					p->color = 0;
					u->color = 0;
					gp->color = 1;

					n = gp;
				}
				else
				{
					if (n == p->left)
					{
						n = p;
						this->_right_rotate(n);
					}

					p = n->parent;
					gp = p->parent;

					p->color = 0;
					gp->color = 1;

					this->_left_rotate(gp);
				}
			}
		}

		this->_root->color = 0;
		this->_nil->color = 0;
	}

	rbnode *insert(key_type key)
	{
		rbnode *node = this->_insert_find(key);
		rbnode *parent = node->parent;

		while (parent != this->_nil)
		{
			this->_size(parent);
			parent = parent->parent;
		}

		this->_insert_fixup(node);

		return node;
	}

	rbnode *insert(key_type key, value_type value)
		requires(!std::is_same_v<V, void>)
	{
		rbnode *node = this->_insert_find(key);
		rbnode *parent = node->parent;

		node->value = value;

		while (parent != this->_nil)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		this->_insert_fixup(node);

		return node;
	}

	template <typename T>
	rbnode *add(key_type key, T value)
		requires(!std::is_same_v<V, void> && insertable<V>)
	{
		rbnode *node = this->_insert_find(key);
		rbnode *parent = node->parent;
		uint8_t fixup = 0;

		if (node->value.size() == 0)
		{
			fixup = 1;
		}

		node->value.insert(value);

		while (parent != this->_nil)
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

	template <typename T>
	rbnode *remove(key_type key, T value)
		requires(!std::is_same_v<V, void> && insertable<V>)
	{
		rbnode *node = this->_insert_find(key);
		rbnode *parent = node->parent;

		node->value.erase(value);

		if (node->value.size() == 0)
		{
			this->erase(node);
			return;
		}

		while (parent != this->_nil)
		{
			this->_size(parent);
			this->_join(parent);

			parent = parent->parent;
		}

		return node;
	}

	void erase(rbnode *n)
	{
		rbnode *t = nullptr;
		rbnode *p = nullptr;
		uint8_t color = 0;

		if (n == nullptr || n == this->_nil)
		{
			return;
		}

		color = n->color;

		if (n->left != this->_nil && n->right != this->_nil)
		{
			rbnode *m = n->right;

			while (m->left != this->_nil)
			{
				m = m->left;
			}

			color = m->color;
			p = m->parent;
			t = m->right;

			if (m != n->right)
			{
				this->_transplant(m, m->right);

				while (p != this->_nil && p != n)
				{
					this->_size(p);
					this->_join(p);

					p = p->parent;
				}

				m->right = n->right;
				n->right->parent = m;
			}
			else
			{
				t->parent = m;
			}

			this->_transplant(n, m);

			m->left = n->left;
			m->left->parent = m;
			m->color = n->color;

			this->_size(m);
			this->_join(m);
		}
		else
		{
			if (n->left == this->_nil)
			{
				t = n->right;
				this->_transplant(n, t);
			}
			else
			{
				t = n->left;
				this->_transplant(n, t);
			}
		}

		p = n->parent;

		while (p != this->_nil)
		{
			this->_size(p);
			this->_join(p);

			p = p->parent;
		}

		this->_free_node(n);

		if (color)
		{
			return;
		}

		while (t != this->_root && t->color == 0)
		{
			rbnode *w = nullptr;

			if (t == t->parent->left)
			{
				w = t->parent->right;

				if (w->color)
				{
					w->color = 0;
					t->parent->color = 1;

					this->_left_rotate(t->parent);
					w = t->parent->right;
				}

				if (w->left->color == 0 && w->right->color == 0)
				{
					w->color = 1;
					t = t->parent;
				}
				else
				{
					if (w->right->color == 0)
					{
						w->left->color = 0;
						w->color = 1;

						this->_right_rotate(w);
						w = t->parent->right;
					}

					w->color = t->parent->color;
					t->parent->color = 0;
					w->right->color = 0;

					this->_left_rotate(t->parent);
					break;
				}
			}
			else
			{
				w = t->parent->left;

				if (w->color)
				{
					w->color = 0;
					t->parent->color = 1;

					this->_right_rotate(t->parent);
					w = t->parent->left;
				}

				if (w->left->color == 0 && w->right->color == 0)
				{
					w->color = 1;
					t = t->parent;
				}
				else
				{
					if (w->left->color == 0)
					{
						w->right->color = 0;
						w->color = 1;

						this->_left_rotate(w);
						w = t->parent->left;
					}

					w->color = t->parent->color;
					t->parent->color = 0;
					w->left->color = 0;

					this->_right_rotate(t->parent);
					break;
				}
			}
		}

		t->color = 0;

		this->_root->color = 0;
		this->_nil->color = 0;
	}

	void clear()
	{
		this->_clear();
	}

	void update(rbnode *node)
		requires(!std::is_same_v<P, void>)
	{
		if (node == nullptr || node == this->_nil)
		{
			return;
		}

		// Assign the priority here
		node->priority = this->_priority(node);
		node->current = node->priority;

		while (node != this->_nil)
		{
			this->_join(node);
			node = node->parent;
		}
	}

	uint32_t query(key_type constraint)
		requires(!std::is_same_v<P, void>)
	{
		rbnode *node = this->_root;
		rbnode *final = nullptr;
		uint32_t result = UINT32_MAX;

		// Copy the find_* template
		// Example gte
#if 0
		while (node != this->_nil)
		{
			if (constraint <= node->key)
			{
				result = MIN(result, node->priority);

				if (node->right != this->_nil)
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
			if (final->right != this->_nil)
			{
				result = MIN(result, final->right->current);
			}
		}
#endif

		// Example lte
#if 0
		while (node != this->_nil)
		{
			if (constraint < node->key)
			{

				node = node->left;
			}
			else
			{
				result = MIN(result, node->priority);

				if (node->left != this->_nil)
				{
					result = MIN(result, node->left->current);
				}

				final = node;
				node = node->right;
			}
		}

		if (final != nullptr)
		{
			if (final->left != this->_nil)
			{
				result = MIN(result, final->left->current);
			}
		}
#endif

		return result;
	}

	rbnode *find(key_type key)
	{
		rbnode *n = this->_root;

		while (n != this->_nil)
		{
			if (key == n->key)
			{
				return n;
			}

			if (key < n->key)
			{
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}

		return nullptr;
	}

	rbnode *get(uint32_t order)
	{
		rbnode *n = this->_root;

		if (order >= this->_count)
		{
			return nullptr;
		}

		while (n != this->_nil)
		{
			uint32_t count = 1 + n->left->size;

			if (count == (order + 1))
			{
				return n;
			}

			if (count > (order + 1))
			{
				n = n->left;
			}
			else
			{
				n = n->right;
				order -= count;
			}
		}

		return nullptr;
	}

	uint32_t order(rbnode *n)
	{
		uint32_t count = 0;

		if (n == nullptr)
		{
			return UINT32_MAX;
		}

		count += n->left->size;

		while (n != this->_nil)
		{
			if (n->parent != this->_nil)
			{
				if (n->parent->right == n)
				{
					count += 1 + n->parent->left->size;
				}
			}

			n = n->parent;
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

	rbnode *front()
	{
		rbnode *n = this->_root;

		if (n == this->_nil)
		{
			return nullptr;
		}

		while (n->left != this->_nil)
		{
			n = n->left;
		}

		return n;
	}

	rbnode *back()
	{
		rbnode *n = this->_root;

		if (n == this->_nil)
		{
			return nullptr;
		}

		while (n->right != this->_nil)
		{
			n = n->right;
		}

		return n;
	}

	rbnode *next(rbnode *n)
	{
		rbnode *t = nullptr;

		if (n == nullptr || n == this->_nil)
		{
			return nullptr;
		}

		if (n->right != this->_nil)
		{
			t = n->right;

			while (t->left != this->_nil)
			{
				t = t->left;
			}

			return t;
		}

		while (n->parent != this->_nil)
		{
			t = n->parent;

			if (t->right == n)
			{
				n = t;
				continue;
			}

			return t;
		}

		return nullptr;
	}

	rbnode *prev(rbnode *n)
	{
		rbnode *t = nullptr;

		if (n == nullptr || n == this->_nil)
		{
			return nullptr;
		}

		if (n->left != this->_nil)
		{
			t = n->left;

			while (t->right != this->_nil)
			{
				t = t->right;
			}

			return t;
		}

		while (n->parent != this->_nil)
		{
			t = n->parent;

			if (t->left == n)
			{
				n = t;
				continue;
			}

			return t;
		}

		return nullptr;
	}

	rbnode *find_lt(key_type key)
	{
		rbnode *n = this->_root;
		rbnode *r = nullptr;

		while (n != this->_nil)
		{
			if (key <= n->key)
			{
				n = n->left;
			}
			else
			{
				r = n;
				n = n->right;
			}
		}

		return r;
	}

	rbnode *find_lte(key_type key)
	{
		rbnode *n = this->_root;
		rbnode *r = nullptr;

		while (n != this->_nil)
		{
			if (key < n->key)
			{
				n = n->left;
			}
			else
			{
				r = n;
				n = n->right;
			}
		}

		return r;
	}

	rbnode *find_gt(key_type key)
	{
		rbnode *n = this->_root;
		rbnode *r = nullptr;

		while (n != this->_nil)
		{
			if (key < n->key)
			{
				r = n;
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}

		return r;
	}

	rbnode *find_gte(key_type key)
	{
		rbnode *n = this->_root;
		rbnode *r = nullptr;

		while (n != this->_nil)
		{
			if (key <= n->key)
			{
				r = n;
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}

		return r;
	}

	uint32_t count_lt(key_type key)
	{
		rbnode *n = this->find_lt(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->order(n) + 1;
	}

	uint32_t count_lte(key_type key)
	{
		rbnode *n = this->find_lte(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->order(n) + 1;
	}

	uint32_t count_gt(key_type key)
	{
		rbnode *n = this->find_gt(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->size() - this->order(n);
	}

	uint32_t count_gte(key_type key)
	{
		rbnode *n = this->find_gte(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->size() - this->order(n);
	}
};

template <typename T>
using ordered_set = rbtree<T, void, void, false>;

template <typename T>
using ordered_multiset = rbtree<T, void, void, true>;

template <typename K, typename V>
using ordered_map = rbtree<K, V, void, false>;

template <typename K, typename V>
using ordered_multimap = rbtree<K, V, void, true>;

template <typename K, typename P>
using augmented_set = rbtree<K, void, P, false>;

template <typename K, typename P>
using augmented_multiset = rbtree<K, void, P, true>;

template <typename K, typename V, typename P>
using augmented_map = rbtree<K, V, P, false>;

template <typename K, typename V, typename P>
using augmented_multimap = rbtree<K, V, P, true>;
