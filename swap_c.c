#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void swap(int* a, int* b){ // swap in c implemented using pointers
    int temp = *a;
    *a = *b;
    *b = temp;
}

void generalised_swap(void* a, void* b, size_t sz){
    void* temp = malloc(sz);
    memcpy(temp, a, sz);
    memcpy(a, b, sz);
    memcpy(b, temp, sz);

    free(temp);
}

int main(){
    int a = 10, b = 5;
    swap(&a, &b);
    printf("a = %d, b = %d\n", a, b);

    generalised_swap(&a, &b, sizeof(a));
    printf("a = %d, b = %d\n", a, b);
}