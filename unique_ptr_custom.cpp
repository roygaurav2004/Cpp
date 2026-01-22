#pragma once

#include<iostream>

template<typename T>
class Unique_Ptr{
private:
    T* m_ptr;
public:
    explicit Unique_Ptr(T* ptr = nullptr) : m_ptr(ptr) {}

    ~Unique_Ptr(){
        if(m_ptr) delete m_ptr;
    }

    // remove the copy operations
    Unique_Ptr(const Unique_Ptr& rhs) = delete;
    Unique_Ptr& operator=(const Unique_Ptr& rhs) = delete;

    // move operations
    Unique_Ptr(Unique_Ptr&& rhs) noexcept : m_ptr(rhs.m_ptr){
        rhs.m_ptr = nullptr;
    }

    Unique_Ptr& operator=(Unique_Ptr&& rhs) noexcept{
        if(this == &rhs) return *this;
        delete m_ptr;
        m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
        return *this;
    }

    const T& operator*() const{return *m_ptr;}
    T& operator*(){ return *m_ptr;}

    const T* operator->() const{return m_ptr;}
    T* operator->(){return m_ptr;}

    void reset(T* rhs = nullptr){
        T* old = m_ptr;
        m_ptr = rhs;
        delete old;
    }

    T* release(){
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }

    T* get() const{
        return m_ptr;
    }

    explicit operator bool() const{return (m_ptr != nullptr);}
};

int main(){
    Unique_Ptr<int> ptr1(new int(5));
    std::cout << *ptr1 << std::endl;

    Unique_Ptr<int> ptr2 = std::move(ptr1);
    std::cout << *ptr2 << std::endl;

    ptr2.reset(new int(10));
    std::cout << *ptr2 << std::endl;

    const Unique_Ptr<int> constPtr(new int(20));
    std::cout << *constPtr << std::endl;
    // *constPtr = 30; // gives error

    Unique_Ptr<const int> constPtr2(new int(40));
    std::cout << *constPtr2 << std::endl;

    return 0;
}