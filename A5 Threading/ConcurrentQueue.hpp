#pragma once

////////////////////////////////////////////////////////////////////////
//
// Author:Melanie Prettyman
// Date:03/2024
//
// CS 6013
//
// Outline for ConcurrentQueue class
//
////////////////////////////////////////////////////////////////////////

#include <mutex>

template <typename T>
class ConcurrentQueue {

public:
    ConcurrentQueue() :
            head_( new Node{ T{}, nullptr } ), size_( 0 )
    {
        tail_ = head_;
    }

//Adds a new node at the tail of the linked list.
    void enqueue( const T & x ) {
        Node* newNode = new Node(x, nullptr);

        if(newNode == nullptr){
            throw std::runtime_error("[Error: newNode == nullptr] Invalid node initialization.");
        }

        //Critical section; need to protect the global variables from being accessed by multiple threads
        //Lock the mutex
        {
            std::unique_lock<std::mutex> lock(mutex_);
            //set the node that is currently the tail to point the newNode
            tail_->next = newNode;
            //The new node is now the tail
            tail_ = newNode;

            //Check if the queue is empty, init head to point to the new node
            if(tail_ == head_){
                head_->next = newNode;
            }

            //Update size of queue
            size_++;
        }//<----Lock is out of scope and unlocks the mutex
    }

/*dequeue( T * ret ) removes a node from the head of the linked list, and returns the data
 * of the removed node in the variable ret. If the queue is empty, dequeue returns false.
 * If an element was dequeued successfully, dequeue returns true.*/
    bool dequeue( T * ret ) {
        //Critical section; need to protect the global variables from being accessed by multiple threads
        //Lock the mutex
        {
            std::unique_lock<std::mutex> lock(mutex_);

            //If the queue is empty, return false
            if (tail_ == head_) {
                return false;
            }

            Node *nodeToRemove = head_->next;

            // Check if we are about to remove the last element
            if (nodeToRemove == tail_) {
                // Reset tail_ to point to head_ since we are about to remove the last element
                tail_ = head_;
            }
            Node *newHead = nodeToRemove->next;

            //set head to point to the next element
            head_->next = newHead;

            //If ret is not a nullptr, assign the data of nodeToRemove to *ret
            if (ret != nullptr) {
                *ret = nodeToRemove->data;
            }

            // Delete the old head node
            delete nodeToRemove;

            // Decrease the size of the queue
            size_--;

            // Return true since the operation was successful
            return true;
        }//<----Lock is out of scope and unlocks the mutex
    }

    ~ConcurrentQueue() {

        while( head_ != nullptr ) {
            Node* temp = head_->next;
            delete head_;
            head_ = temp;
        }
    }

    int size() const { return size_; }

private:

    struct Node {
        T      data;
        Node * next;
        // Constructor for the Node
        Node(const T& dataValue, Node* nextPtr = nullptr) {
            this->data = dataValue;
            this->next = nextPtr;
        }
    };

    Node * head_;
    Node * tail_;
    int    size_;

    mutable std::mutex mutex_; // Mutex to protect access to the queue

};

