#include<bits/stdc++.h>
using namespace std;

template<template<typename> typename Predicate, typename... Types>
struct count_if_custom;

template<template<typename> typename Predicate>
struct count_if_custom<Predicate> {
    static constexpr size_t value = 0;
};

template<template<typename> typename Predicate, typename First, typename... Rest>
struct count_if_custom<Predicate, First, Rest...> {
    static constexpr size_t value = Predicate<First>::value + count_if_custom<Predicate, Rest...>::value;
};

int main(){
    constexpr int count = count_if_custom<std::is_integral, int, double, char, float, long>::value;
    cout << count << endl; // Output: 3
}