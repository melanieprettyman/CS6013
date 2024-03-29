//
// Created by Melanie Prettyman on 3/29/24.
//
#include <iostream>
#include <thread>
#include "ConcurrentQueue.hpp"


/*
 */
bool testQueue( int num_producers, int num_consumers, int num_ints ) {
    //a. Create an std::vector of std::threads.
    std::vector<std::thread> threadList;
    //b. reserve space in this vector for all producer and consumer threads
    threadList.reserve(num_producers + num_consumers);
    //c. Create a ConcurrentQueue object statically
    ConcurrentQueue<int> queue;

    //d. Create 'num_producer' producer threads that enqueue num_ints ints into the ConcurrentQueue.
    for(int i = 0; i < num_producers; i++){
        threadList.push_back(std::thread([&queue, num_ints](){
            for(int j = 0; j < num_ints; j++){
                queue.enqueue(j);
            }
        }));
    }
    //e. Create num_consumer consumer threads that dequeue num_ints ints from the ConcurrentQueue.
    for(int i = 0; i < num_consumers; i++){
        threadList.push_back(std::thread([&queue, num_ints](){
            for(int j = 0; j < num_ints; j++){
                int value;
                queue.dequeue(&value);
            }
        }));
    }
    //f. Wait for all threads to join (ie, finish).
    for(auto& thread : threadList){
        thread.join();
    }

    //g. Return true if the number of elements in the queue matches (num_producers - num_consumers)*num_ints, false otherwise.
    // This basically tests if we did the right number of inserts and deletes into the queue.*/
    return queue.size() == (num_producers - num_consumers) * num_ints;
}

int main(int argc, char** argv){
    /*argv to reads in num_producers, num_consumers, and num_ints from the command line.
     * testQueue is called with these parameters and check the return value to determine ConcurrentQueue is working correctly.*/

    std::string argument = argv[1];
    int num_producers = std::stoi(argument);
    argument = argv[2];
    int num_consumers = std::stoi(argument);
    argument = argv[3];
    int num_ints = std::stoi(argument);

    if(testQueue(num_producers, num_consumers, num_ints)) {
        std::cout << "Yay! All tests passed!" << std::endl;
    } else {
        std::cout << "Cowa bummer dude...Tests failed!" << std::endl;
    }

    return 0;

}
