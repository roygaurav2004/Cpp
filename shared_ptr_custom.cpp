#pragma once

#include<iostream>
#include<atomic>    

struct ControlBlock{
    std::atomic<uint32_t> ref_count;
    ControlBlock() : ref_count(1) {}
};

template<typename T>
class Shared_Ptr{
private:
    T* m_ptr = nullptr;
    ControlBlock* cb = nullptr;
public:
    explicit Shared_Ptr(T* rhs) : m_ptr(rhs), cb(rhs ? new ControlBlock : nullptr) {}
    explicit Shared_Ptr() : m_ptr(nullptr), cb(nullptr) {}

    ~Shared_Ptr(){
        if(cb && cb->ref_count > 0){
            cb->ref_count--;
            if(cb->ref_count == 0){
                delete m_ptr;
                delete cb;
            }
        }
    }

    // copy ctor
    Shared_Ptr(const Shared_Ptr& rhs) : m_ptr(rhs.m_ptr), cb(rhs.cb) {
        cb->ref_count++;
    }
    // move ctor
    Shared_Ptr(Shared_Ptr&& rhs) noexcept : m_ptr(rhs.m_ptr), cb(rhs.cb){
        rhs.m_ptr = nullptr;
        rhs.cb = nullptr;
    }
    // copy assignment
    Shared_Ptr& operator=(const Shared_Ptr& rhs) {
        if(this == &rhs) return *this;
        if(m_ptr){
            cb->ref_count--;
            if(cb->ref_count == 0){
                delete m_ptr;
                delete cb;
            }
        }
        m_ptr = rhs.m_ptr;
        cb = rhs.cb;
        cb->ref_count++;
        return *this;
    }
    // move assignment
    Shared_Ptr& operator=(Shared_Ptr&& rhs) noexcept{
        if(this == &rhs) return *this;
        if(m_ptr){
            cb->ref_count--;
            if(cb->ref_count == 0){
                delete m_ptr;
                delete cb;
            }
        }
        m_ptr = rhs.m_ptr;
        cb = rhs.cb;
        rhs.m_ptr = nullptr;
        rhs.cb = nullptr;
        return *this;
    }
    const T* get() const { return m_ptr; }
    const T* operator->() const { return m_ptr; }
    T* operator->() { return m_ptr; }
    const T& operator*() const { return *m_ptr; }
    T& operator*() { return *m_ptr; }

    explicit operator bool() const { return m_ptr != nullptr; }

    uint32_t use_count() const {
        return cb ? cb->ref_count.load() : 0;
    }

    void reset(T* rhs = nullptr) {
        if (m_ptr == rhs && rhs != nullptr) return;
        Shared_Ptr(rhs).swap(*this);
    }

    void swap(Shared_Ptr& other) noexcept {
        std::swap(m_ptr, other.m_ptr);
        std::swap(cb, other.cb);
    }
};

int main(){
    Shared_Ptr<int> sp(new int(10));
    std::cout << "Value: " << *sp << ", Use count: " << sp.use_count() << std::endl;

    {
        Shared_Ptr<int> sp2 = sp;
        std::cout << "Value: " << *sp2 << ", Use count: " << sp.use_count() << std::endl;
    }

    Shared_Ptr<int> sp3 = std::move(sp);
    std::cout << "Value: " << *sp3 << ", Use count: " << sp3.use_count() << std::endl;

    // change value
    *sp3 = 20;
    std::cout << "Value: " << *sp3 << ", Use count: " << sp3.use_count() << std::endl;

    // null sharedptr
    Shared_Ptr<int> sp4;
    std::cout << "Value: " << (sp4 ? *sp4 : 0) << ", Use count: " << sp4.use_count() << std::endl;

    // check reset
    Shared_Ptr<int> sp5(new int(30));
    std::cout << "Value: " << *sp5 << ", Use count: " << sp5.use_count() << std::endl;
    sp5.reset();
    std::cout << "Value: " << (sp5 ? *sp5 : 0) << ", Use count: " << sp5.use_count() << std::endl;

    return 0;
}