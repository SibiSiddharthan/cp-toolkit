// Essential
#include <iostream>
#include <format>
#include <algorithm>

// Containers
#include <string>
#include <vector>

// Data Structures
// #include <map>
// #include <set>
// #include <unordered_map>
// #include <unordered_set>

// Math
// #include <climits>
// #include <cmath>

// Optional
// #include <stack>
// #include <queue>
// #include <deque>
// #include <utility>

using namespace std;

#if 0
#	define SINGLE_TESTCASE
#endif

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define YES()                  \
	{                          \
		cout << "YES" << endl; \
	}

#define NO()                  \
	{                         \
		cout << "NO" << endl; \
	}

template <typename T>
vector<T> read_vector(uint64_t n)
{
	vector<T> v(n, 0);

	for (uint64_t i = 0; i < n; ++i)
	{
		cin >> v[i];
	}

	return v;
}

template <typename T>
void print_vector(vector<T> &v)
{
	for (uint64_t i = 0; i < v.size() - 1; ++i)
	{
		cout << v[i] << ' ';
	}

	cout << v[v.size() - 1] << endl;
}

static void solve()
{
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	cout.tie(nullptr);

	uint32_t t = 1;

#ifndef SINGLE_TESTCASE
	cin >> t;
#endif

	while (t--)
	{
		solve();
	}

	return 0;
}
