// Essential
#include <iostream>
#include <format>
#include <algorithm>
#include <concepts>
#include <limits>
#include <numeric>

// Containers
#include <string>
#include <vector>
#include <array>
#include <utility>

// Data Structures
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <set>

// Miscellaneous
#include <cmath>

using namespace std;

#if 0
#	define SINGLE_TESTCASE
#endif

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define DIFF(a, b) (MAX(a, b) - MIN(a, b))
#define NEG(a)     (~(a) + 1)
#define ABS(a)     ((a) < 0 ? -(a) : (a))

// pair
template <typename T1, typename T2>
istream &operator>>(istream &is, pair<T1, T2> &p)
{
	return (is >> p.first >> p.second);
}

template <typename T1, typename T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &p)
{
	return (os << format("({} {})", p.first, p.second));
}

// containers
template <typename T>
concept container = requires(T a) {
	a.begin();
	a.end();
} && !std::same_as<std::remove_cvref_t<T>, string>;

template <container T>
istream &operator>>(istream &is, T &container)
{
	for (auto &x : container)
	{
		is >> x;
	}

	return is;
}

template <container T>
ostream &operator<<(ostream &os, const T &container)
{
	for (const auto &x : container)
	{
		os << x << ' ';
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
static void output(T &&...args)
{
	((cout << args << ' '), ...) << endl;
}

#define OUT(...) output(__VA_ARGS__)

template <typename... T>
static void debug(T &&...args)
{
	cerr << "debug: ";
	((cerr << format("{}", args) << ' '), ...) << endl;
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
