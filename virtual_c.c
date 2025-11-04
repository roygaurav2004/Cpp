#include<stdio.h>

struct Shape;
struct Circle;

typedef struct _vft{
    double (*area)(void*);
} VFT; // Virtual function table

typedef struct _base{
    VFT* vptr; // virtual base pointer like in cpp
    int a;
    int b; // just to show base members
    void (*display)(void*);
} Shape;

typedef struct _derived{
    Shape base; // contains the vptr
    double r;
} Circle;

void display(void* s){
    Shape* shape = (Shape*)s;
    printf("a = %d b = %d\n", shape->a, shape->b);
}

double area(void* cptr){
    Circle* c = (Circle*)cptr;
    double area = c->r * c->r * 3.14;
    return area;
}

VFT circle_vptr = {&area};

void init_circle(Circle* c, double r){
    (c->base).vptr = &circle_vptr;
    c->r = r;
    (c->base).a = 10;
    (c->base).b = 15;
    c->base.display = display;
}

int main(){
    Circle c1; // creating the derived object
    init_circle(&c1, 5);
    Shape* s = (Shape*)&c1; // base pointer to derived object
    double area = s->vptr->area(s);
    display(s);
    printf("%f\n", area);
}