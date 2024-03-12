//
// Created by Melanie Prettyman on 3/11/24.
//

#ifndef HW4_HASHTABLE_H
#define HW4_HASHTABLE_H
#include <cstddef>
#include <sys/mman.h>
#include <iostream>

class Entry {
public:
    void* address;
    size_t size;

    Entry(void* add, size_t size);
};

class HashTable {
public:
    //Pointer to the allocated memory region (array of Entry structs)
    Entry* allocationArray;
    int capacity;
    int currentSize;

public:
    HashTable();
    ~HashTable();
    explicit HashTable(int initialCapacity);
    size_t hash(long address);
    bool insert(void* pAddressToInsert, size_t sizeInPages);
    size_t remove(void *pAddressToRemove);
    void grow();

};


#endif //HW4_HASHTABLE_H










