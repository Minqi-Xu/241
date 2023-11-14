#include <cstdint>
#include <iostream>
#include "a10p1.h"

int64_t *freelst = arena();
int64_t tot_size = arena()[0];

void print_freelst() {
    if(freelst == 0) {
        std::cerr << "freelst = NULL" << std::endl;
        return;
    }
    int64_t* current = freelst;
    while(current) {
        std::cerr << current << "->";
        current = (int64_t*)current[1];
    }
    std::cerr << std::endl;
}

int64_t *mymalloc(int64_t size) {
    std::cerr << "allocate size " << size <<std::endl;
    if(freelst == arena() && freelst[0] == tot_size) {
        freelst[1] = 0;
    }
    int64_t *current = freelst;
    int64_t **pre_ptr = &freelst;
    while(current) {
        std::cerr << "searching on " << current << std::endl;
        if(current[0] < ((size + 1) * 8)) {
            if(current == freelst) {}
            else pre_ptr = (int64_t**)(current + 1);
            current = (int64_t*)current[1];
        } else if(current[0] < (size + 1 + 2) * 8) {
            *pre_ptr = (int64_t*)current[1];
            std::cerr << "allocated at " << current << std::endl;
            return current + 1;
        } else {
            current[size + 1] = current[0] - (size + 1) * 8;
            current[size + 2] = current[1];
            current[0] = (size + 1) * 8;
            *pre_ptr = current + (size + 1);
            std::cerr << "allocated at " << current << std::endl;
            return current + 1;
        }
    }
    std::cerr << "no enough space" << std::endl;
    return 0;
}

void myfree(int64_t *address) {
    print_freelst();
    if(!address)    return;
    int64_t *target = address - 1;
    int64_t *current = freelst;
    int64_t **pre_ptr = &freelst;
    if(!freelst) {
        freelst = target;
        target[1] = 0;
        return;
    }
    while(current) {
            std::cerr << "target: " << target << " current: " << current << std::endl;
        if(current < target) {
//            if(current == freelst){
//                    std::cerr << "freelst = " << freelst << std::endl;
                    std::cerr << "debug line 64" << std::endl;
//            } else {
                pre_ptr = (int64_t**)(current + 1);
//            }
            current = (int64_t*)current[1];
        } else if(current > target) {
            std::cerr << "debug line 69" << std::endl;
            if(*pre_ptr == freelst) std::cerr << "debug pre_ptr -> freelst" << std::endl;
            *pre_ptr = target;
            if(*pre_ptr == freelst) std::cerr << "debug pre_ptr does not change" << std::endl;
            target[1] = (int64_t)current;
            //if(*pre_ptr != freelst)
                std::cerr << "((((int64_t)pre_ptr[-1])/8) - 1) = " << ((((int64_t)pre_ptr[-1])/8) - 1) << std::endl;
                std::cerr << "target && calculation: " << target << "  " << (int64_t*)(pre_ptr + ((((int64_t)pre_ptr[-1])/8) - 1)) << std::endl;
            if(target[1] == (int64_t)(target + (target[0]/8))) {
                std::cerr << "debug line 78" << std::endl;
                target[0] = target[0] + current[0];
                target[1] = (int64_t)current;
            }
            if(/*pre_ptr != freelst && */target == (int64_t*)(pre_ptr + ((((int64_t)pre_ptr[-1])/8) - 1))) {
                std::cerr << "debug line 83" << std::endl;
                (*pre_ptr)[-1] = (*pre_ptr)[-1] + target[0];
                pre_ptr = (int64_t**)&current[1];
            }
            return;
        } else {
            std::cerr << "WARNING: the address has already deallocated" << std::endl;
            return;
        }
    }
    *pre_ptr = target;
//    (*pre_ptr)[-1] = (*pre_ptr)[-1] + target[0];
    target[1] = 0;
    if(/*pre_ptr != freelst && */target == (int64_t*)(pre_ptr + ((((int64_t)pre_ptr[-1])/8) - 1))) {
        (*pre_ptr)[-1] = (*pre_ptr)[-1] + target[0];
        pre_ptr = (int64_t**)&target[1];
    }
}

int main() {
    int64_t result = 0;
    print_freelst();
    int64_t* one = mymalloc(241);
    print_freelst();
    int64_t* two = mymalloc(241);
    print_freelst();
    myfree(one);
    print_freelst();

    int64_t* evil = mymalloc(240);
    print_freelst();
    int64_t* good = mymalloc(1);
    print_freelst();
    evil[3] = 240;
    good[0] = evil[3]+1;
    myfree(evil);

    int64_t* cs[241];
    for(int i=0; i<241; i++) {
        cs[i] = mymalloc(1);
        cs[i][0] = 1;
    }
    int64_t* big = mymalloc(1000);
    for(int i=0; i<1000; i++) {
        big[i] = 1000;
    }
    print_freelst();
    for(int i=0; i<241; i++) {
        result += cs[i][0];
        myfree(cs[i]);
    }

    result += good[0];
    myfree(good);
    std::cerr << result <<std::endl;
    print_freelst();
    // Returns nonzero exit code if the result is wrong
    return result != 241*2;
}
