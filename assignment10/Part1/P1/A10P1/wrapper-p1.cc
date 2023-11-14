#include <iostream>
#include <cstdint>
#include "a10p1.h"

int64_t my_max(int64_t a, int64_t b) {
    if(a > b)
        return a;
    return b;
}

int64_t wain(int64_t* a, int64_t n) {
    int64_t *arr = arena();
    int64_t count_max = 0;
    for(int64_t i = 0; i < 1000000; i++) {
        arr[i] = 0;
    }
    for(int64_t i = 0; i < n; i++) {
        arr[a[i]]++;
    }
    for(int64_t i = 0; i < 1000000; i++) {
        count_max = my_max(count_max, arr[i]);
    }
    return count_max; // replace as appropriate
}

// Do not modify the code below.
int main(int argc, char *argv[]) {
    int64_t l, c;
    int64_t* a;
    std::cout << "Enter length of array: ";
    std::cin >> l;
    a = new int64_t[l];
    for(int64_t i = 0; i < l; ++i) {
        std::cout << "Enter value of array element " << i << " ";
        std::cin >> a[i];
    }
    c = wain(a,l);
    delete [] a;
    std::cerr << c << std::endl;
    return c;
}
