#include <iostream>
#include <iomanip>
using namespace std;

template<typename T, int size>
class StaticArray_Base{
public:
    T* m_arr;
    StaticArray_Base() {
        m_arr = new T[size];
    }
    ~StaticArray_Base() {
        delete[] m_arr;
    }
    void print() const {
        for (int i = 0; i < size; ++i) {
            cout << m_arr[i] << " ";
        }
        cout << endl;
    }
    void commonFunction() const {
        cout << "Common function in StaticArray" << endl;
    }
};

template<typename T, int size>
class StaticArray : public StaticArray_Base<T, size>{};

template<int size>
class StaticArray<double, size> : public StaticArray_Base<double, size>{
public:
    void print() const {
        for (int i = 0; i < size; ++i) {
            cout << fixed << setprecision(2) << this->m_arr[i] << " "; // this is needed for dependent name
        }
        cout << endl;
    }
};

int main(){
    StaticArray<int, 5> intArray;
    intArray.print();
    intArray.commonFunction();

    StaticArray<double, 5> doubleArray;
    doubleArray.print();
    doubleArray.commonFunction();

    return 0;
}