#include<iostream>
#include<list>
#include<vector>
#include<optional>

template<typename Key, typename Value>
class HashMap{
public:
    HashMap(size_t cap) : mCapacity(cap), maxLoadFactor(0.75) {
        mTable.resize(mCapacity);
    }

    bool containskey(const Key& key){
        auto hashIndex = hashFunction(key);
        for(const auto& [k,v] : mTable[hashIndex]){
            if(k == key) return true;
        }
        return false;
    }

    void insert(const Key& key, const Value& value){
        if(containskey(key)){
            // Key already exists, update the value
            auto hashIndex = hashFunction(key);
            for(auto& [k,v] : mTable[hashIndex]){
                if(k == key){
                    v = value;
                    return;
                }
            }
        } else {
            // Key does not exist, insert new key-value pair
            auto hashIndex = hashFunction(key);
            mTable[hashIndex].emplace_back(key, value);
            mSize++;
            checkLoadFactor();
        }
    }

    void remove(const Key& key){
        if(!containskey(key)) return;
        auto hashIndex = hashFunction(key);
        for(auto it = mTable[hashIndex].begin(); it != mTable[hashIndex].end(); ++it){
            if(it->first == key){
                mTable[hashIndex].erase(it);
                mSize--;
                return;
            }
        }
    }

    std::optional<Value> get(const Key& key){
        if(!containskey(key)) return std::nullopt;
        auto hashIndex = hashFunction(key);
        for(const auto& [k,v] : mTable[hashIndex]){
            if(k == key) return v;
        }
        return std::nullopt;
    }

    size_t size() const{
        return mSize;
    }

    bool isEmpty() const{
        return mSize == 0;
    }
private:
    std::vector<std::list<std::pair<Key, Value>>> mTable;
    size_t mCapacity;
    size_t mSize;
    float maxLoadFactor;

    void checkLoadFactor(){
        float currentLoadFactor = static_cast<float>(mSize) / mCapacity;
        if(currentLoadFactor > maxLoadFactor){
            rehash();
        }
    }

    auto hashFunction(const Key& key) const {
        return std::hash<Key>{}(key) % mCapacity;
    }

    void rehash(){
        int oldCap = mCapacity;
        mCapacity *= 2;
        std::vector<std::list<std::pair<Key, Value>>> newTable(mCapacity);
        for(int i=0; i<oldCap; i++){
            for(const auto& [k,v] : mTable[i]){
                size_t newIndex = hashFunction(k);
                newTable[newIndex].emplace_back(k,v);
            }
        }
        mTable = std::move(newTable);
    }
};

int main(){
    HashMap<int, std::string> map(10);
    map.insert(1, "One");
    map.insert(2, "Two");
    map.insert(3, "Three");

    if(map.containskey(2)){
        std::cout << "Key 2 exists with value: " << *(map.get(2)) << std::endl;
    }

    map.remove(2);

    if(!map.containskey(2)){
        std::cout << "Key 2 has been removed." << std::endl;
    }

    return 0;
}