#include <bits/stdc++.h>
using namespace std;

template<typename T>
class memory_pool {
private:
    union Node {
        Node* next; // Used when the block is FREE
        char data[sizeof(T)]; // Used when the block is ALLOCATED
    };
    int size;
    Node* buf;
    Node* front;

public:
    explicit memory_pool(size_t sz) : size(sz) {
        buf = static_cast<Node*>(::operator new(sz * sizeof(Node)));
        for (size_t i = 0; i < sz; ++i) {
            buf[i].next = (i + 1 < sz) ? &buf[i+1] : nullptr;
        }
        front = buf;
    }

    ~memory_pool(){
        for(int i=0; i<size; i++) buf[i].~Node();
        ::operator delete(buf, size * sizeof(Node));
    }
    
    T* allocate() {
        if (!front) return nullptr;
        Node* node = front;
        front = front->next; 
        return reinterpret_cast<T*>(node->data);
    }

    void deallocate(T* ptr) {
        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = front;
        front = node;
    }
};