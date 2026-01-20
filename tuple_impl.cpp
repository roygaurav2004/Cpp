#include <utility>
#include <iostream>

template<typename... Types>
struct TupleImpl {};

template<typename Head, typename... Tail>
struct TupleImpl<Head, Tail...> : TupleImpl<Tail...> {
    Head value;

    template<typename H, typename... T>
    TupleImpl(H&& h, T&&... t)
        : TupleImpl<Tail...>(std::forward<T>(t)...),
          value(std::forward<H>(h)) {}
};

template<int N, typename Head, typename... Tail>
struct GetHelper {
    using Base = TupleImpl<Tail...>;
    using type = typename GetHelper<N-1, Tail...>::type;

    static type& get(TupleImpl<Head, Tail...>& t) {
        Base& parent = t;
        return GetHelper<N-1, Tail...>::get(parent);
    }
};

template<typename Head, typename... Tail>
struct GetHelper<0, Head, Tail...> {
    using type = Head;

    static Head& get(TupleImpl<Head, Tail...>& t) {
        return t.value;
    }
};

// Public Tuple
template<typename... Types>
class Tuple : private TupleImpl<Types...> {
public:
    template<typename... UTypes>
    Tuple(UTypes&&... args)
        : TupleImpl<Types...>(std::forward<UTypes>(args)...) {}

    int size() const { return sizeof...(Types); }

    template<int N>
    typename GetHelper<N, Types...>::type& get() {
        TupleImpl<Types...>& base = *this;
        return GetHelper<N, Types...>::get(base);
    }
};

int main(){
    Tuple<int, double, const char*> myTuple(42, 3.14, "Hello");
    myTuple.get<0>() = 100;  // Modify the int
    myTuple.get<1>() = 2.71;  // Modify the double
    myTuple.get<2>() = "World";  // Modify the string

    // print
    std::cout << myTuple.get<0>() << ", "
              << myTuple.get<1>() << ", "
              << myTuple.get<2>() << std::endl;

    return 0;
}