#include <concepts>
#include <vector>

using namespace std;

template <typename K, typename V = void, typename P = void, bool DUPLICATES = false>
struct rbtree
{
	using key_type = K;
	using value_type = V;
	using priority_type = P;

	struct rbnode_set
	{
		// Common fields
		key_type key;

		// Order statisitics
		uint32_t size : 31;
		uint8_t color : 1;

		rbnode_set *parent, *left, *right;
	};

	struct rbnode_map
	{
		// Common fields
		key_type key;
		value_type value;

		// Order statisitics
		uint32_t size : 31;
		uint8_t color : 1;

		rbnode_map *parent, *left, *right;
	};

	struct rbnode_set_ext
	{
		// Common fields
		key_type key;
		priority_type priority;
		priority_type current;

		// Order statisitics
		uint32_t size : 31;
		uint8_t color : 1;

		// rbnode_set_ext *node;
		rbnode_set_ext *parent, *left, *right;
	};

	struct rbnode_map_ext
	{
		// Common fields
		key_type key;
		value_type value;
		priority_type priority;
		priority_type current;

		// Order statisitics
		uint32_t size : 31;
		uint8_t color : 1;

		// rbnode_map_ext *node;
		rbnode_map_ext *parent, *left, *right;
	};

	using rbnode = conditional_t<is_same_v<V, void> && is_same_v<P, void>, rbnode_set,
								 conditional_t<!is_same_v<V, void> && is_same_v<P, void>, rbnode_map,
											   conditional_t<is_same_v<V, void> && !is_same_v<P, void>, rbnode_set_ext, rbnode_map_ext>>>;

	template <typename T>
	static constexpr bool clearable = requires(T t) { t.clear(); };

	vector<rbnode *> _pool;
	vector<rbnode *> _free;

	rbnode *_nil;
	rbnode *_root;
	uint32_t _count;

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

	void _size(rbnode *n)
	{
		if (n == this->_nil)
		{
			return;
		}

		n->size = 1 + n->left->size + n->right->size;
	}

	void _join(rbnode *n)
	{
		if constexpr (!is_same_v<P, void>)
		{
			n->priority = n->value.front() != nullptr ? n->value.front()->key : UINT32_MAX;
			n->current = n->priority;

			if (n->left != this->_nil)
			{
				n->current = MIN(n->current, n->left->current);
			}

			if (n->right != this->_nil)
			{
				n->current = MIN(n->current, n->right->current);
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

	rbnode *insert(key_type key)
	{
		rbnode *n = this->_root;
		rbnode *t = this->_nil;
		rbnode *result = nullptr;

		while (n != this->_nil)
		{
			t = n;

			if (key < n->key)
			{
				n = n->left;
			}
			else
			{
				if (key == n->key)
				{
					if constexpr (DUPLICATES == false)
					{
						return n;
					}
				}

				n = n->right;
			}
		}

		n = this->_alloc_node();
		result = n;

		n->key = key;
		n->parent = t;

		if (t == this->_nil)
		{
			this->_root = n;
			this->_root->color = 0;

			return n;
		}

		if (n->key < t->key)
		{
			t->left = n;
		}
		else
		{
			t->right = n;
		}

		while (t != this->_nil)
		{
			this->_size(t);
			t = t->parent;
		}

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

		return result;
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
		node->priority = 0;
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
