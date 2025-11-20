#include <bits/stdc++.h>
using namespace std;

template<typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };
    
    std::atomic<Node*> head{nullptr};

public:
    void push(T value) {
        Node* newNode = new Node(value);
        Node* oldHead = head.load();
        
        do{
            newNode->next = oldHead;
        } while(!head.compare_exchange_strong(oldHead, newNode));

    }
    
    bool pop(T& result) {
        Node* oldHead = head.load();
        do {
            if (oldHead == nullptr) {
                return false; // Stack is empty
            }
            result = oldHead->data;
        } while (!head.compare_exchange_strong(oldHead, oldHead->next));
        return true;
    }
}; 