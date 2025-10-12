#include <vector>

using namespace std;

template <typename T>
vector<vector<T>> build_sparse_table(vector<T> &vec)
{
	vector<vector<uint32_t>> sparse;

	sparse.push_back(vec);

	for (uint32_t k = 2; k < n; k *= 2)
	{
		vector<uint32_t> temp;

		for (uint32_t i = 0; i < n - k + 1; ++i)
		{
			temp.push_back(sparse.back()[i] | sparse.back()[i + (k / 2)]);
		}

		sparse.push_back(t);
	}

	return sparse;
}

template <typename T>
T sparse_query(vector<vector<T>> &sparse, size_t left, size_t right)
{
	size_t n = right - left + 1;
	size_t l = sparse.size() - 1;
	size_t a = 0;

	while (n > 0)
	{
		while (n < (1 << l))
		{
			--l;
		}

		a |= sparse[l][left];
		left += (1 << l);
		n -= (1 << l);
	}

	return a;
}

template <typename T>
struct fenwick_tree
{
	vector<T> tree;
	size_t size = 0;

	fenwick_tree(const vector<T> &value)
	{
		this->tree = value;
		this->size = value.size();

		for (size_t i = 0; i < size; ++i)
		{
			size_t above = i + ((size_t)1 << __builtin_ctzll(i + 1));

			if (above < size)
			{
				this->tree[above] = this->tree[above] + this->tree[i];
			}
		}
	}

	T psum(size_t index)
	{
		T sum = 0;
		index += 1;

		while (index != 0)
		{
			sum = sum + tree[index - 1];
			index -= (size_t)1 << __builtin_ctzll(index);
		}

		return sum;
	}

	T sum(size_t left, size_t right)
	{
		if (left >= this->size || right >= this->size)
		{
			return 0;
		}

		if (left == 0)
		{
			return this->psum(right);
		}
		else
		{
			return this->psum(right) - this->psum(left - 1);
		}
	}

	void update(size_t index, T value)
	{
		T old = this->sum(index, index);

		while (index < size)
		{
			this->tree[index] = (this->tree[index] + value) - old;
			index += (size_t)1 << __builtin_ctzll(index + 1);
		}
	}
};
