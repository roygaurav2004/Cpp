// TODO: Implement a variadic template function called count_true
// that counts how many boolean arguments are true.
// The function should work with any number of boolean arguments.
// Example usage: count_true(true, false, true) should return 2

// Your implementation here 
#include <utility>
#include <iostream>

int count_true(){
    return 0;
}

template<typename First>
int count_true(First first) {
    return (first ? 1 : 0);
}

template<typename First, typename... Rest>
int count_true(First first, Rest... rest) {
    return (first ? 1 : 0) + count_true<Rest...>(rest...);
}

int main() {
    std::cout << count_true(true, false, true) << std::endl;  // Output: 2
    std::cout << count_true(false, false, false) << std::endl; // Output: 0
    std::cout << count_true(true, true, true) << std::endl;   // Output: 3
    return 0;
}

