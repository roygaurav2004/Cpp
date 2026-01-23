#include <iostream>

using namespace std;

template<typename Vector>
class VectorIterator{
public:
    using ValueType = typename Vector::ValueType;
    using PointerType = ValueType*;
    using ReferenceType = ValueType&;
public:
    VectorIterator(PointerType ptr) : m_ptr(ptr) {}
    VectorIterator& operator++(){
        m_ptr++;
        return *this;
    }
    VectorIterator operator++(int){
        VectorIterator temp = *this;
        ++(*this);
        return temp;
    }
    VectorIterator& operator--(){
        m_ptr--;
        return *this;
    }
    VectorIterator operator--(int){
        VectorIterator temp = *this;
        --(*this);
        return temp;
    }
    ReferenceType operator[](int index){
        return *(m_ptr + index);
    }
    ReferenceType operator*(){
        return *m_ptr;
    }
    PointerType operator->(){
        return m_ptr;
    }
    bool operator==(const VectorIterator& other) const{
        return m_ptr == other.m_ptr;
    }
    bool operator!=(const VectorIterator& other) const{
        return m_ptr != other.m_ptr;
    }
private:
    PointerType m_ptr;
};

template<typename T>
class Vector{
public:
    using ValueType = T;
    using Iterator = VectorIterator<Vector<T>>;
public:
    Vector(){
        Realloc(2);
    }

    Vector(int size, const T& value) {
        Realloc(size);
        m_size = size;
        for (size_t i = 0; i < size; i++) {
            new(&m_data[i]) T(value);
        }
    }

    Vector(int size) : Vector(size, T()) {}

    Vector(const Vector& other) {
        Realloc(other.m_capacity);
        for (size_t i = 0; i < other.m_size; i++) {
            new(&m_data[i]) T(other.m_data[i]);
        }
        m_size = other.m_size;
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Clear();
            Realloc(other.m_capacity);
            for (size_t i = 0; i < other.m_size; i++) {
                new(&m_data[i]) T(other.m_data[i]);
            }
            m_size = other.m_size;
        }
        return *this;
    }

    Vector(Vector&& other) noexcept{
        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    Vector& operator=(Vector&& other) noexcept{
        if (this != &other) {
            Clear();
            ::operator delete(m_data, m_capacity * sizeof(T));

            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    ~Vector(){
        Clear();
        ::operator delete(m_data, m_capacity * sizeof(T));
    }

    void PushBack(const T& value){
        if(m_size >= m_capacity){
            Realloc(m_capacity + m_capacity/2);
        }
        new(&m_data[m_size]) T(value);
        m_size++;
    }

    template<typename... Args>
    T& EmplaceBack(Args&&... args){
        if(m_size >= m_capacity){
            Realloc(m_capacity + m_capacity/2);
        }
        new(&m_data[m_size]) T(forward<Args>(args)...);
        return m_data[m_size++];
    }

    void PopBack(){
        if(m_size > 0){
            m_size--;
            m_data[m_size].~T();
        }
    }
    
    void Clear(){
        for(size_t i=0; i<m_size; i++){
            m_data[i].~T();
        }
        m_size = 0;
    }


    int Size() const {
        return m_size;
    }

    const T& operator[](size_t index) const {
        return m_data[index];
    }

    T& operator[](size_t index){
        return const_cast<T&>(static_cast<const Vector&>(*this)[index]);
    }

    Iterator begin(){
        return Iterator(m_data);
    }

    Iterator end(){
        return Iterator(m_data + m_size);
    }

    Iterator erase(Iterator pos){
        Iterator next = pos;
        ++next;
        for (Iterator it = pos; it != end(); ++it) {
            *it = std::move(*next);
            ++next;
        }
        PopBack();
        return pos;
    }

private:
    T* m_data = nullptr;
    size_t m_size = 0, m_capacity = 0;

    void Realloc(size_t newCapacity){
        T* newBlock = static_cast<T*>(::operator new(newCapacity * sizeof(T)));
        if(newCapacity < m_size) m_size = newCapacity;

        for(size_t i=0; i<m_size; i++){
            new(&newBlock[i]) T(move(m_data[i]));
        }
        for(size_t i=0; i<m_size; i++) m_data[i].~T();

        ::operator delete(m_data, m_capacity * sizeof(T));
        m_data = newBlock;
        m_capacity = newCapacity;
    }
};

template<typename T>
void PrintVector(const Vector<T>& vec) {
    for (size_t i = 0; i < vec.Size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

int main(){
    // test all
    Vector<int> vec;
    vec.PushBack(1);
    vec.PushBack(2);
    vec.PushBack(3);
    PrintVector(vec);
    vec.Clear();
    Vector<int> vec2(5, 10);
    PrintVector(vec2);
    // emplace
    vec2.EmplaceBack(4);
    vec2.EmplaceBack(5);
    PrintVector(vec2);
    
    Vector<int> vec3(vec2);
    PrintVector(vec3);

    // print using iterators
    for (auto it = vec3.begin(); it != vec3.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    return 0;
}