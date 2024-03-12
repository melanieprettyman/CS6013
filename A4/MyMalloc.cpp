//
// Created by Melanie Prettyman on 3/11/24.
//

#include "MyMalloc.h"

//4 kb const page size
const int PAGESIZE = 4096;
const int INITIALCAPACITY = 100;

MyMalloc::MyMalloc() {
    hashTable = HashTable(INITIALCAPACITY);
}

//Destructor for MyMalloc
//deallocates the memory region for the HashTable object
MyMalloc::~MyMalloc() {
    for (int i = 0; i < hashTable.capacity; ++i) {
        //clear out memory mapped for each entry in the hashmap
        if (hashTable.allocationArray[i].size != 0 && hashTable.allocationArray[i].size != -1){
            if (munmap(hashTable.allocationArray[i].address, hashTable.allocationArray[i].size) == -1){
                std::cerr << "**Failed to clear memory in DESTRUCTOR" << std::endl;
                exit(1);
            }
        }
    }
    //clear out memory mapped for the hashmap itself
    if (munmap(hashTable.allocationArray, hashTable.capacity * sizeof(Entry)) == -1){
        perror("**Failed to munmap hashMap within Malloc class");
        exit(1);
    }
    hashTable.allocationArray = nullptr;
}
//Returns the bytes needed rounded up to the nearest page size
//this is used to determine the number of pages to allocate
size_t MyMalloc::getPagesToAllocate(size_t bytes) {
    //Minus 1 for the case in which the size required is exactly a page size
    return ((bytes + (PAGESIZE - 1)) / PAGESIZE) * PAGESIZE;
}

//Allocates bytes by calling mmap and returns a pointer to the address in memory
void *MyMalloc::allocate(size_t bytesToAllocate) {

    if (bytesToAllocate <= 0){
        throw std::runtime_error("**Invalid number of bytes passed to allocate");
    }

    size_t pages = getPagesToAllocate(bytesToAllocate);

    //mmap() syscall to allocate memory
    void *pAddress = mmap(nullptr, pages, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

    if (pAddress == MAP_FAILED){
        perror("**mmap failed");
        exit(1);
    }

    //store the Entry address and size in the hash table for future deallocation
    if (!(hashTable.insert(pAddress, pages))) {
        std::cerr << "**Failed to store node in HashMap" << std::endl;
        exit(1);
    }

    return pAddress;
}

//Deallocate takes in a pointer to the address of memory to be deallocated, uses the member variable hash map to
//retrieve the size of memory size allocated and then call munmap() syscall to deallocate
void MyMalloc::deallocate(void *pAddressToDeallocate) {
    size_t size = hashTable.remove(pAddressToDeallocate);
    if (size == -1){
        throw std::runtime_error("**Failed to remove entry");
    }
    munmap(pAddressToDeallocate, size);

}


