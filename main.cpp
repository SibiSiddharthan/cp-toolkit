// Essential
#include <iostream>
#include <format>
#include <algorithm>

// Containers
#include <string>
#include <vector>
#include <utility>

// Legacy
#include <cstdint>
#include <climits>

// Data Structures
// #include <map>
// #include <set>
// #include <unordered_map>
// #include <unordered_set>

// Math
// #include <cmath>

// Optional
// #include <stack>
// #include <queue>
// #include <deque>

using namespace std;

#if 0
#	define SINGLE_TESTCASE
#endif

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// pair
template <typename T1, typename T2>
istream &operator>>(istream &is, pair<T1, T2> &p)
{
	is >> p.first >> p.second;
	return is;
}

template <typename T1, typename T2>
ostream &operator<<(ostream &os, pair<T1, T2> &p)
{
	os << format("({} {})", p.first, p.second);
	return os;
}

// vector
template <typename T>
istream &operator>>(istream &is, vector<T> &vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		is >> vec[i];
	}

	return is;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<T> &vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (i + 1 < vec.size())
		{
			os << vec[i] << ' ';
		}
		else
		{
			os << vec[i];
		}
	}

	return os;
}

// IO
template <typename... T>
static void input(T &...args)
{
	((cin >> args), ...);
}

#define IN(...) input(__VA_ARGS__)

template <typename... T>
static void output(T &...args)
{
	((cout << args << " "), ...) << endl;
}

#define OUT(...) output(__VA_ARGS__)

template <typename... T>
static void debug(T &...args)
{
	cerr << "debug: ";
	((cerr << args << " "), ...) << endl;
}

#define DBG(...) debug(__VA_ARGS__)

#define YES() OUT("YES")
#define NO()  OUT("NO")

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
