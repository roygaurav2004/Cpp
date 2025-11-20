#include <bits/stdc++.h>

template<size_t N, typename T, typename... Args>
decltype(auto) deduce_type(T&& arg, Args&&... args) {
    if constexpr (N == 0) {
        return std::forward<T>(arg);
    } else {
        return deduce_type<N - 1>(std::forward<Args>(args)...);
    }
}

template<size_t N, typename... Args>
decltype(auto) get_nth(Args&&... args) {
    static_assert(N < sizeof...(Args), "Index N is out of bounds for argument pack.");
    
    return deduce_type<N>(std::forward<Args>(args)...);
} 

int main(){
    auto&& value = get_nth<2>(10, 20.5, std::string("Hello"), 'A');
    std::cout << value << std::endl; // Output: Hello

    auto&& value2 = get_nth<0>(10, 20.5, std::string("Hello"), 'A');
    std::cout << value2 << std::endl; // Output: 10
    return 0;
}