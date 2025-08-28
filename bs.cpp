#include <vector>

using namespace std;

template <typename T>
size_t bs_lte(const vector<T> &v, T a)
{
	size_t left = 0, right = v.size() - 1;
	size_t middle = (left + right) / 2;
	size_t index = v.size();

	if (v.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		if (v[middle] <= a)
		{
			index = middle;
			left = middle + 1;
		}
		else
		{
			if (middle == 0)
			{
				break;
			}

			right = middle - 1;
		}

		middle = (left + right) / 2;
	}

	return index;
}

template <typename T>
size_t bs_gte(const vector<T> &v, T a)
{
	size_t left = 0, right = v.size() - 1;
	size_t middle = (left + right) / 2;
	size_t index = v.size();

	if (v.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		if (v[middle] >= a)
		{
			index = middle;

			if (middle == 0)
			{
				break;
			}

			right = middle - 1;
		}
		else
		{
			left = middle + 1;
		}

		middle = (left + right) / 2;
	}

	return index;
}
