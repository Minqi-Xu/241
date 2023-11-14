#include <iostream>
#include <cstdint>
#include "a10p2.h"

void dealloc(int64_t* str) {
    if(str == NULL) return;
    dealloc(cdr(str));
    snoc(str);
}

int64_t wain(int64_t*, int64_t) {
    char ch;
    int64_t *str = NULL;
    int64_t counting = 0;
    ch = getchar();
    if(ch != EOF) {
        str = cons((int64_t)ch, NULL);
        counting = 1;
    }
    int64_t *current = str;
    while(true) {
        ch = getchar();
        if(ch == EOF)
            break;
        counting++;
        int64_t *n = cons((int64_t)ch, NULL);
        current = setcdr(current, n);
        current = cdr(current);
    }
    current = str;
    while(current != NULL) {
        ch = car(current);
        std::cout << ch;
        current = cdr(current);
    }
    current = str;
    while(current != NULL) {
        ch = car(current);
        std::cout << ch;
        current = cdr(current);
    }
    dealloc(str);
    return counting; // replace as appropriate
}

// Do not modify the code below.
int main(int argc, char *argv[]) {
    int ret = wain(0,0);
    std::cerr << ret << std::endl;
    return ret;
}
