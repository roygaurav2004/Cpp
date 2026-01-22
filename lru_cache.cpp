#include<iostream>
#include<unordered_map>

template<typename Key, typename Value>
class LRU{
private:
    class Node{
    public:
        Key key; Value value;
        Node* next;
        Node* prev;
        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr), prev(nullptr) {}
    };
    Node* head;
    Node* tail;
    const size_t capacity;
    std::unordered_map<Key, Node*> cache;
public:
    LRU(size_t cap) : capacity(cap), head(new Node(Key(), Value())), tail(new Node(Key(), Value())) {
        head->next = tail;
        tail->prev = head;
    }

    ~LRU(){
        for(auto& pair : cache){
            delete pair.second;
        }
        delete head;
        delete tail;
    }

    int get(const Key& key){
        if(cache.find(key) == cache.end()) return -1;
        Node* node = cache[key];
        node = removeNode(node);
        node = insertAtHead(node);
        return node->value;
    }

    void put(const Key& key, const Value& value){
        if(cache.find(key) != cache.end()){
            Node* node = cache[key];
            node = removeNode(node);
            node = insertAtHead(node);
            node->value = value;
        }else{
            if(cache.size() >= capacity){
                Node* lru = tail->prev;
                cache.erase(lru->key);
                delete removeNode(lru);
            }
            Node* newNode = new Node(key, value);
            cache[key] = insertAtHead(newNode);
        }
    }
private:
    Node* removeNode(Node* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
        return node;
    }
    Node* insertAtHead(Node* node){
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
        return node;
    }
};  

int main(){
    LRU<int, int> lruCache(2);
    lruCache.put(1, 1);
    lruCache.put(2, 2);
    std::cout << lruCache.get(1) << std::endl; // returns 1
    lruCache.put(3, 3); // evicts key 2
    std::cout << lruCache.get(2) << std::endl; // returns -1 (not found)
    lruCache.put(4, 4); // evicts key 1
    std::cout << lruCache.get(1) << std::endl; // returns -1 (not found)
    std::cout << lruCache.get(3) << std::endl; // returns 3
    std::cout << lruCache.get(4) << std::endl; // returns 4
    return 0;
}