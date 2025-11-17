#include <vector>

using namespace std;

template <typename T>
size_t bs_lte(const vector<T> &v, T a)
{
	size_t left = 0, right = v.size() - 1;
	size_t middle = 0;
	size_t index = v.size();

	if (v.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

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
	}

	return index;
}

template <typename T>
size_t bs_gte(const vector<T> &v, T a)
{
	size_t left = 0, right = v.size() - 1;
	size_t middle = 0;
	size_t index = v.size();

	if (v.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

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
	}

	return index;
}

template <typename T>
size_t bs_lt(const vector<T> &v, T a)
{
	size_t left = 0, right = v.size() - 1;
	size_t middle = 0;
	size_t index = v.size();

	if (v.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

		if (v[middle] < a)
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
	}

	return index;
}

template <typename T>
size_t bs_gt(const vector<T> &v, T a)
{
	size_t left = 0, right = v.size() - 1;
	size_t middle = 0;
	size_t index = v.size();

	if (v.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

		if (v[middle] > a)
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
	}

	return index;
}

pair<uint32_t, uint32_t> merge_array(vector<uint64_t> &a, vector<uint64_t> &b, uint64_t ac, uint64_t bc, uint64_t count)
{
	size_t left = 0, right = 0, middle = 0;

	// a
	left = 0;
	right = ac - 1;
	middle = (left + right) / 2;

	while (left <= right)
	{
		size_t result = bs_gt(b, a[middle], bc);
		size_t index = result + middle;

		// DBG(result, index);

		if (index == count - 1)
		{
			return {middle + 1, result};
		}

		if (index > count - 1)
		{
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

	// b
	left = 0;
	right = bc - 1;
	middle = (left + right) / 2;

	while (left <= right)
	{
		size_t result = bs_gte(a, b[middle], ac);
		size_t index = result + middle;

		// DBG(result, index);

		if (index == count - 1)
		{
			return {result, middle + 1};
		}

		if (index > count - 1)
		{
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

	return {0, 0};
}

void ternary_search()
{
	uint32_t left = 2, right = 999;
	uint32_t m1 = left + (right - left) / 3;
	uint32_t m2 = right - (right - left) / 3;

	uint32_t ans = 0;

	while (left < right)
	{
		if (ans == m1 * m2)
		{
			left = m2 + 1;
		}
		else if (ans == m1 * (m2 + 1))
		{

			left = m1 + 1;
			right = m2;
		}
		else if (ans == (m1 + 1) * (m2 + 1))
		{
			right = m1;
		}

		m1 = left + (right - left) / 3;
		m2 = right - (right - left) / 3;
	}
}
