//
// Created by Melanie Prettyman on 3/25/24.
//
#include <iostream>
#include <cassert>
#include "SerialQueue.hpp"

void enqueueIncreasesSizeForStaticAllocation(){
    SerialQueue<int> queue;
    queue.enqueue(1);
    assert(queue.size() == 1);
}

void dequeueDecreasesSizeForStaticAllocation() {
    SerialQueue<int> queue2;
    queue2.enqueue(1);
    queue2.dequeue(nullptr);
    assert(queue2.size() == 0);
}


void enqueueIncreasesSizeForDynamicAllocation() {
    SerialQueue<int>* queue = new SerialQueue<int>();
    queue->enqueue(1);
    assert(queue->size()== 1);
    delete queue;
}

void dequeueDecreasesSizeForDynamicAllocation() {
    SerialQueue<int>* queue = new SerialQueue<int>();
    queue->enqueue(1);
    queue->dequeue(nullptr);
    assert(queue->size() == 0);
    delete queue;
}

void dynamicDeallocationDeletesAllNodes() {
    SerialQueue<int>* queue = new SerialQueue<int>();
    queue->enqueue(10);
    queue->enqueue(20);
    queue->enqueue(30);

    assert(queue->size() == 3);

    int value;
    queue->dequeue(&value);
    assert(value == 10 && queue->size() == 2);

    queue->dequeue(&value);
    assert(value == 20 && queue->size() == 1);

    queue->dequeue(&value);
    assert(value == 30 && queue->size() == 0);

    delete queue;

}
void behaviorWhenDequeueingEmptyQueue() {
    SerialQueue<int> queue;
    int value;
    assert(queue.dequeue(&value) == false);
}

void new_dynamic_enqueue_test(){
    SerialQueue<int> *dyn_queue_char = new SerialQueue<int>();
    for(int i = 0; i < 100; i++){
        dyn_queue_char->enqueue(i);
    }
    assert(dyn_queue_char->size() == 100);
}

void new_dynamic_dequeue_test(){
    SerialQueue<int> *dyn_queue_char = new SerialQueue<int>();
    for(int i = 0; i < 100; i++){
        dyn_queue_char->enqueue(i);
    }
    assert(dyn_queue_char->size() == 100);

    int value;
    for(int i = 0; i < 100; i++){
        dyn_queue_char->dequeue(&value);
        assert(value == i);
    }
}

int main() {
    enqueueIncreasesSizeForStaticAllocation();
    dequeueDecreasesSizeForStaticAllocation();
    enqueueIncreasesSizeForDynamicAllocation();
    dequeueDecreasesSizeForDynamicAllocation();
    dynamicDeallocationDeletesAllNodes();
    behaviorWhenDequeueingEmptyQueue();
    new_dynamic_enqueue_test();
    new_dynamic_dequeue_test();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
