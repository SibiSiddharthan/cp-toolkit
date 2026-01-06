#include <vector>

using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// For all of the binary search routines if no such element exists return the size of the array.

///////////////////////////////////////////////////////////
// Ascending Order
///////////////////////////////////////////////////////////

// Find the largest index of the element which is less than or equal to value in the array.

template <typename T>
size_t bs_lte(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t left = start, right = MIN(end, elements.size() - 1);
	size_t middle = 0;
	size_t index = elements.size();

	if (elements.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

		if (elements[middle] <= value)
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

// Find the smallest index of the element which is greater than or equal to value in the array.

template <typename T>
size_t bs_gte(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t left = start, right = MIN(end, elements.size() - 1);
	size_t middle = 0;
	size_t index = elements.size();

	if (elements.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

		if (elements[middle] >= value)
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

// Find the largest index of the element which is less than value in the array.

template <typename T>
size_t bs_lt(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t left = start, right = MIN(end, elements.size() - 1);
	size_t middle = 0;
	size_t index = elements.size();

	if (elements.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

		if (elements[middle] < value)
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

// Find the smallest index of the element which is greater than value in the array.

template <typename T>
size_t bs_gt(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t left = start, right = MIN(end, elements.size() - 1);
	size_t middle = 0;
	size_t index = elements.size();

	if (elements.size() == 0)
	{
		return 0;
	}

	while (left <= right)
	{
		middle = (left + right) / 2;

		if (elements[middle] > value)
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

// Count the number of elements less than or equal to value in the array.

template <typename T>
size_t bs_lte_count(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t index = bs_lte(elements, value, start, end);

	if (index == elements.size())
	{
		return 0;
	}

	return index + 1;
}

// Count the number of elements greater than or equal to value in the array.

template <typename T>
size_t bs_gte_count(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t index = bs_gte(elements, value, start, end);

	if (index == elements.size())
	{
		return 0;
	}

	return elements.size() - index;
}

// Count the number of elements less than value in the array.

template <typename T>
size_t bs_lt_count(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t index = bs_lt(elements, value, start, end);

	if (index == elements.size())
	{
		return 0;
	}

	return index + 1;
}

// Count the number of elements greater than value in the array.

template <typename T>
size_t bs_gt_count(const vector<T> &elements, T value, size_t start = 0, size_t end = UINT64_MAX)
{
	size_t index = bs_gt(elements, value, start, end);

	if (index == elements.size())
	{
		return 0;
	}

	return elements.size() - index;
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
