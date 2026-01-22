template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T d) : data(d), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* sentinel = new Node(T());
        head.store(sentinel);
        tail.store(sentinel);
    }

    void push(T value) {
        Node* newNode = new Node(value);
        while (true) {
            Node* last = tail.load();
            Node* next = last->next.load();

            if (last == tail.load()) { // Verify tail is consistent
                if (next == nullptr) {
                    // Try to link the new node to the end
                    if (last->next.compare_exchange_strong(next, newNode)) {
                        // Successfully linked. Now try to move the tail pointer.
                        tail.compare_exchange_strong(last, newNode);
                        return;
                    }
                } else {
                    // Tail is lagging (another thread added a node but didn't move tail).
                    // We help move the tail forward before trying again.
                    tail.compare_exchange_strong(last, next);
                }
            }
        }
    }

    bool pop(T& result) {
        while (true) {
            Node* first = head.load();
            Node* last = tail.load();
            Node* next = first->next.load();

            if (first == head.load()) {
                if (first == last) {
                    if (next == nullptr) return false; // Queue empty
                    // Tail lagging, help move it
                    tail.compare_exchange_strong(last, next);
                } else {
                    // Read data before moving head
                    result = next->data;
                    if (head.compare_exchange_strong(first, next)) {
                        // In a real system, you'd delete 'first' here
                        // But wait! This is the ABA/Memory Reclamation trap.
                        return true; 
                    }
                }
            }
        }
    }
};