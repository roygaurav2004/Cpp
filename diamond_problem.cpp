#include <iostream>

using namespace std;

class A{
public:
    int val;
    A(int v) : val(v) {
        cout << "Constructor of A called" << endl;
    }
    void printVal() {
        cout << "Value in A: " << val << endl;
    }
};

class B: virtual public A{
public:
    B(int v) : A(v+10) {
        cout << "Constructor of B called" << endl;
    }
};

class C: virtual public A{
public:
    C(int v) : A(v+20) {
        cout << "Constructor of C called" << endl;
    }
};

class D: public B, public C{
public:
    D(int v) : A(v), B(v), C(v) {
        cout << "Constructor of D called" << endl;
    }
};

int main(){
    D d(5);
    d.printVal(); // Value in A: 5
    return 0;
}