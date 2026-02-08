#include <concepts>
#include <vector>

using namespace std;

template <typename K, typename V = void, typename P = void, bool D = false>
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

	struct rbnode_map_ext
	{
		// Common fields
		key_type key;
		value_type value;
		priority_type priority;

		// Order statisitics
		uint32_t size : 31;
		uint8_t color : 1;

		rbnode_map_ext *parent, *left, *right;
	};

	using rbnode =
		conditional_t<is_same_v<V, void> && is_same_v<P, void>, rbnode_set, conditional_t<!is_same_v<V, void>, rbnode_map, rbnode_map_ext>>;

	vector<rbnode *> _pool;
	vector<rbnode *> _free;

	bool _duplicate = D;

	rbnode *_nil;
	rbnode *_root;
	uint32_t _count;

	rbtree()
	{
		// Allocate a sentinel
		this->_nil = (rbnode *)malloc(sizeof(rbnode));

		this->_nil->key = {};
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
		auto clear = [](rbnode *n) {
			// For container values
			// n->value.clear();
		};

		while (this->_pool.size() != 0)
		{
			clear(this->_pool.back());
			free(this->_pool.back());

			this->_pool.pop_back();
		}
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

	uint32_t _size(rbnode *n)
	{
		if (n == this->_nil)
		{
			return 0;
		}

		return 1 + n->left->size + n->right->size;
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
		n->size = this->_size(n);
		t->size = this->_size(t);
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
		n->size = this->_size(n);
		t->size = this->_size(t);
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
					if (this->_duplicate == 0)
					{
						return n;
					}
				}

				n = n->right;
			}
		}

		n = this->_alloc_node();

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
			t->size = this->_size(t);
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

		return n;
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
					p->size = this->_size(p);
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
			m->size = this->_size(m);
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
			p->size = this->_size(p);
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

	void update(rbnode *node)
	{
		if (node == nullptr)
		{
			return;
		}
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

		if (n == nullptr || this->_nil)
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

		return n->size + 1;
	}

	uint32_t count_lte(key_type key)
	{
		rbnode *n = this->find_lte(key);

		if (n == nullptr)
		{
			return 0;
		}

		return n->size + 1;
	}

	uint32_t count_gt(key_type key)
	{
		rbnode *n = this->find_gt(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->size() - n->size;
	}

	uint32_t count_gte(key_type key)
	{
		rbnode *n = this->find_gte(key);

		if (n == nullptr)
		{
			return 0;
		}

		return this->size() - n->size;
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
