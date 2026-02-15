#include<iostream>

using namespace std;

template<typename Derived>
class Base{
public:
    void interface(){
        static_cast<Derived*>(this)->implementation();
    }

    void implementation(){
        cout << "Base class implementation" << endl;
    }
};

class Derived : public Base<Derived>{
public:
    void implementation(){
        cout << "Derived class implementation" << endl;
    }
};

int main(){
    Base<Derived> b;
    b.interface();

    Derived d;
    Base<Derived>& ref = d;
    ref.interface();
    return 0;
}