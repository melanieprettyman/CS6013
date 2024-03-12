#include "MyMalloc.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>

// Global MyMalloc object
MyMalloc myMalloc;

// Overload global new and delete to use MyMalloc
void* operator new(std::size_t size) {
    return myMalloc.allocate(size);
}

void operator delete(void* ptr) noexcept {
    myMalloc.deallocate(ptr);
}

// Test routines
void testSmallAllocations() {
    const size_t numberOfAllocations = 10000;
    const size_t sizeOfAllocation = 128; // Small size allocation

    // Allocate
    std::vector<char*> allocations;
    for (size_t i = 0; i < numberOfAllocations; ++i) {
        char* allocation = static_cast<char*>(myMalloc.allocate(sizeOfAllocation));
        //write data into allocated memory
        for (size_t j = 0; j < sizeOfAllocation; ++j) {
            allocation[j] = static_cast<char>(j % 256);
        }
        allocations.push_back(allocation);
    }

    // Verify
    for (size_t i = 0; i < numberOfAllocations; ++i) {
        for (size_t j = 0; j < sizeOfAllocation; ++j) {
            if (allocations[i][j] != static_cast<char>(j % 256)) {
                std::cerr << "Small allocations test failed." << std::endl;
                return;
            }
        }
    }

    // Deallocate
    for (auto allocation : allocations) {
        myMalloc.deallocate(allocation);
    }

    std::cout << "Small allocations test passed." << std::endl;
}

void testLargeAllocations() {
    const size_t numberOfAllocations = 1000;
    const size_t sizeOfAllocation = 1024 * 1024; // 1 MB allocation

    // Allocate
    std::vector<char*> allocations;
    for (size_t i = 0; i < numberOfAllocations; ++i) {
        char* allocation = static_cast<char*>(myMalloc.allocate(sizeOfAllocation));
        //write data into allocated memory
        for (size_t j = 0; j < sizeOfAllocation; ++j) {
            allocation[j] = static_cast<char>(j % 256); // Example pattern
        }
        allocations.push_back(allocation);
    }

    // Verify
    for (size_t i = 0; i < numberOfAllocations; ++i) {
        for (size_t j = 0; j < sizeOfAllocation; ++j) {
            if (allocations[i][j] != static_cast<char>(j % 256)) {
                std::cerr << "Large allocations test failed." << std::endl;
                return; // Early return on error
            }
        }
    }

    // Deallocate
    for (auto allocation : allocations) {
        myMalloc.deallocate(allocation);
    }

    std::cout << "Large allocations test passed." << std::endl;
}


void benchmarkAllocFree() {
    const size_t numberOfAllocations = 10000;
    const size_t sizeOfAllocation = 128; // Small size allocation

    std::vector<void*> allocations;

    // Benchmark MyMalloc
    auto startMyMalloc = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numberOfAllocations; ++i) {
        allocations.push_back(myMalloc.allocate(sizeOfAllocation));
    }
    for (auto ptr : allocations) {
        myMalloc.deallocate(ptr);
    }
    allocations.clear(); // Clear the vector for the next test
    auto endMyMalloc = std::chrono::high_resolution_clock::now();

    // Benchmark system malloc and free
    auto startSystemMalloc = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numberOfAllocations; ++i) {
        allocations.push_back(malloc(sizeOfAllocation));
    }
    for (auto ptr : allocations) {
        free(ptr);
    }
    auto endSystemMalloc = std::chrono::high_resolution_clock::now();

    // Calculate and display the durations
    auto myMallocDuration = std::chrono::duration_cast<std::chrono::microseconds>(endMyMalloc - startMyMalloc).count();
    auto systemMallocDuration = std::chrono::duration_cast<std::chrono::microseconds>(endSystemMalloc - startSystemMalloc).count();

    std::cout << "MyMalloc allocation + deallocation time for " << numberOfAllocations << " objects: "
              << myMallocDuration << " microseconds." << std::endl;
    std::cout << "System malloc/free allocation + deallocation time for " << numberOfAllocations << " objects: "
              << systemMallocDuration << " microseconds." << std::endl;
}

void testHashTableInsertAndRemove() {
    HashTable table(10); // Assuming an initial capacity of 10

    // Test inserting a value
    void* testAddress1 = reinterpret_cast<void*>(0x1234);
    size_t testSize1 = 4096; // 4 KB, for example
    bool insertResult = table.insert(testAddress1, testSize1);
    assert(insertResult); // Expect insert to succeed

    // Test removing the value
    size_t removedSize = table.remove(testAddress1);
    assert(removedSize == testSize1); // Expect to remove and return the correct size

    // Test removing a non-existing value
    void* nonExistingAddress = reinterpret_cast<void*>(0x5678);
    removedSize = table.remove(nonExistingAddress);
    assert(removedSize == 0); // Expect size 0 for non-existing entry

    std::cout << "HashTable insert and remove tests passed." << std::endl;
}

void testHashTableGrow() {
    HashTable table(2); // Small table to force growth

    // Insert enough values to trigger a grow
    bool insertResult1 = table.insert(reinterpret_cast<void*>(0x1234), 4096);
    bool insertResult2 = table.insert(reinterpret_cast<void*>(0x5678), 4096);
    bool insertResult3 = table.insert(reinterpret_cast<void*>(0x10678), 4096);

    assert(insertResult1 && insertResult2 && insertResult3); // Expect both inserts to succeed

    // Verify table has grown
    // This may require adding a method to your HashTable class to get the current capacity
     assert(table.capacity > 2); // Uncomment if you add such a method
    std::cout << "HashTable grow test passed." << std::endl;
}

int main() {
    try {
        testSmallAllocations();
        testLargeAllocations();
        benchmarkAllocFree();
        testHashTableInsertAndRemove();
        testHashTableGrow();
    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
