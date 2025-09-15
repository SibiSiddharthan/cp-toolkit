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
