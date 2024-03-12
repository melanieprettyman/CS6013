//
// Created by Melanie Prettyman on 3/11/24.
//

#ifndef A4_MYMALLOC_H
#define A4_MYMALLOC_H

#include "HashTable.h"
#include <cstddef>

class MyMalloc {

public:
    HashTable hashTable;
    MyMalloc();
    ~MyMalloc();
    size_t getPagesToAllocate(size_t bytes);
    void *allocate(size_t bytesToAllocate);
    void deallocate(void *pAddressToDeallocate);

};




#endif //A4_MYMALLOC_H
