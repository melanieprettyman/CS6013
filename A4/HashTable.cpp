//
// Created by Melanie Prettyman on 3/11/24.
//

#include "HashTable.h"

/**
nullptr: The first parameter is the address hint, which is set to nullptr to let the system choose a suitable address for the mapping.

capacity * sizeof(Entry): The second parameter specifies the size of the memory to be allocated.
                          It is calculated as capacity multiplied by the size of Entry struct.

PROT_READ | PROT_WRITE: The third parameter specifies the protection of the mapped memory region. Allows both reading and writing to the memory.

MAP_ANONYMOUS | MAP_PRIVATE: Specifies the type of mapping. MAP_ANONYMOUS = the mapping is not backed by any file,
                            and MAP_PRIVATE = changes to the mapped region are not visible to other processes mapping the same region.

0, 0: file descriptor and the offset within the file. Using MAP_ANONYMOUS, these parameters are set to 0.
 */

static void* const TOMBSTONE = reinterpret_cast<void*>(-1);

//Constructor for Entry
Entry::Entry(void *inputAdd, size_t inputSize) {
    this->address = inputAdd;
    this->size = inputSize;
}

//Default constructor calls the constructor with an initial capacity of 10
HashTable::HashTable() {
    HashTable(2);
}

//Constructor called by Malloc
//creates a new HashTable with a given initial capacity
HashTable::HashTable(int initialCapacity) {
    this->capacity = initialCapacity;
    this->currentSize = 0;

    //Allocated memory region (array of Entry structs) for HashTable
    this->allocationArray = static_cast<Entry *>(mmap(nullptr, capacity * sizeof(Entry), PROT_READ | PROT_WRITE,
                                               MAP_ANONYMOUS | MAP_PRIVATE, 0, 0));
    //Check if allocation was successful
    if (allocationArray == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    //Initialize each entry (within capacity) to communicate they are not currently in use
    for (int index = 0; index < capacity; ++index) {
        allocationArray[index].address = nullptr;
        allocationArray[index].size = 0;
    }
}

//Destructor for HashTable
//deallocates the memory region for the HashTable object
HashTable::~HashTable() {
    this->allocationArray = nullptr;
}

//Hash function that takes in an address and returns a hashcode
size_t HashTable::hash(long address) {
    return static_cast<size_t>(address % capacity);
}



//Take an Entry--address pointer and sizeInPages, stores it at a specific index in HashTable based on the hashcode and
// returns a bool indicating whether the insert was successful or not

bool HashTable::insert(void *pAddressToInsert, size_t sizeInPages) {
    //Grow table if the table size has reached 70% capacity
    if (this->currentSize >= this->capacity * .7) {
        grow();
    }
    //Calculate hashIndex
    size_t hashIndex = hash(reinterpret_cast<long>(pAddressToInsert));

    //Checks if the hashIndex in the hashtable is already occupied by another entry
    while (allocationArray[hashIndex].address != nullptr && allocationArray[hashIndex].address != TOMBSTONE) {
        //Prevent duplicate addresses from being stored in the hashtable
        if (allocationArray[hashIndex].address == pAddressToInsert) {
            throw std::runtime_error("Address already in use in the hash table.");
        }
        //Move to the next slot in the hashtable
        hashIndex++;
        //Wraps around to the beginning of the allocationArray if the end is reached.
        hashIndex %= capacity;
    }
    //Linear Probing for empty slot or Tombstone. Insert address and size into the hash table.
    if (allocationArray[hashIndex].address == nullptr || allocationArray[hashIndex].address == TOMBSTONE) {
        this->currentSize++;
        allocationArray[hashIndex].address = pAddressToInsert;
        allocationArray[hashIndex].size = sizeInPages;
        return true;
    }

    return false;
}

//Takes in address pointer and removes the entry from the hash table
// returns a bool indicating whether the remove was successful or not
size_t HashTable::remove(void *pAddressToRemove){

    size_t hashIndex = hash(reinterpret_cast<long>(pAddressToRemove));

    //Search for the address in the hash table
    while (allocationArray[hashIndex].address != nullptr) {
        if (allocationArray[hashIndex].address == pAddressToRemove) {
            size_t deletedSize = allocationArray[hashIndex].size;
            //Address set to TOMBSTONE, size set to 0 ~ indicates a lazy delete
            allocationArray[hashIndex].address = TOMBSTONE;
            allocationArray[hashIndex].size = 0;
            currentSize--;
            return deletedSize;
        }
        //Move to the next slot in the hashtable. Wraps around to the beginning of the allocationArray if the end is reached
        hashIndex++;
        hashIndex %= capacity;
    }
    return 0;
}

//Doubles the capacity of the HashTable and rehashes all the entries
void HashTable::grow() {
    //Create a new HashTable with double the capacity
    HashTable largerHashTable = HashTable(this->capacity * 2);

    //Insert all nodes from the former HashMap into the new HashMap
    for (int index = 0; index < capacity; ++index) {
        //If the address is not a tombstone and the size is not 0, insert the Entry into the new HashMap
        if (this->allocationArray[index].address != TOMBSTONE && this->allocationArray[index].size != 0) {
            //Insert the Entry into the new HashMap
            if (!(largerHashTable.insert(this->allocationArray[index].address, this->allocationArray[index].size))) {
                std::cerr << "Failure to add Entry when growing HashTable" << std::endl;
                exit(1);
            }
        }
    }
    //Swap the largerHashTable with the current HashTable
    std::swap(largerHashTable.allocationArray, this->allocationArray);
    std::swap(largerHashTable.capacity, this->capacity);

    //Deallocate the memory region for the largerHashTable (now the old HashTable after the swap)
    if (munmap(largerHashTable.allocationArray, sizeof(Entry) * largerHashTable.capacity) == -1){
        perror("failed to munmap");
        exit(1);
    }
}


