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
#include <stack>
#include <queue>

#include <map>
#include <set>

// Math
// #include <cmath>

using namespace std;

#if 0
#	define SINGLE_TESTCASE
#endif

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define DIFF(a, b) (MAX(a, b) - MIN(a, b))

// pair
template <typename T1, typename T2>
istream &operator>>(istream &is, pair<T1, T2> &p)
{
	is >> p.first >> p.second;
	return is;
}

template <typename T1, typename T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &p)
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
		os << vec[i];

		if (i + 1 < vec.size())
		{
			os << ' ';
		}
	}

	return os;
}

// map
template <typename T1, typename T2>
ostream &operator<<(ostream &os, const map<T1, T2> &m)
{
	size_t count = 0;

	for (auto &[k, v] : m)
	{
		os << format("({} {})", k, v);
		count += 1;

		if (count + 1 < m.size())
		{
			os << ' ';
		}
	}

	return os;
}

// set
template <typename T>
ostream &operator<<(ostream &os, const set<T> &s)
{
	size_t count = 0;

	for (auto &i : s)
	{
		os << i;
		count += 1;

		if (count + 1 < s.size())
		{
			os << ' ';
		}
	}

	return os;
}

// priority queue
template <typename T>
using min_priority_queue = priority_queue<T, vector<T>, greater<T>>;

template <typename T>
using max_priority_queue = priority_queue<T, vector<T>, less<T>>;

// IO
template <typename... T>
static void input(T &...args)
{
	((cin >> args), ...);
}

#define IN(...) input(__VA_ARGS__)

template <typename... T>
static void __output(ostream &os, T &&...args)
{
	uint32_t count = 0;

	for (const auto a : {args...})
	{
		os << a;
		count += 1;

		if (count + 1 < sizeof...(args))
		{
			os << ' ';
		}
	}

	os << endl;
}

template <typename... T>
static void output(T &&...args)
{
	__output(cout, args...);
}

#define OUT(...) output(__VA_ARGS__)

template <typename... T>
static void debug(T &&...args)
{
	cerr << "debug: ";
	__output(cerr, args...);
}

#ifndef ONLINE_JUDGE
#	define DBG(...) debug(__VA_ARGS__)
#else
#	define DBG(...)
#endif

#define ALL(a) a.begin(), a.end()

#define YES() OUT("Yes")
#define NO()  OUT("No")

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
	IN(t);
#endif

	while (t--)
	{
		solve();
	}

	return 0;
}
