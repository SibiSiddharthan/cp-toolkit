#pragma once

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

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define DIFF(a, b) (MAX(a, b) - MIN(a, b))
#define NEG(a)     (~(a) + 1)

template <typename T>
using min_priority_queue = priority_queue<T, vector<T>, greater<T>>;

template <typename T>
using max_priority_queue = priority_queue<T, vector<T>, less<T>>;

template <typename O, typename T>
concept binary_operator = requires(O op, T a) {
	{ op.join(a, a) } -> std::same_as<T>;
	{ op.identity() } -> std::same_as<T>;
};

template <typename O, typename T>
concept commutative_operator = requires(O op, T a) {
	{ op.inverse(a, a) } -> std::same_as<T>;
} && binary_operator<O, T>;
