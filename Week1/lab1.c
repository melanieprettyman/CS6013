/*********************************************************************
 *
 * Melanie Prettyman
 * CS6013
 * 01/08/23
 * File header comments go here... including student name, class, date, 
 * assignment, description, etc.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // For strlen()
#include <assert.h>


/*********************************************************************
 *
 * The C functions in this lab use patterns and functionality often found in
 * operating system code. Your job is to implement them.  Additionally, write some test
 * cases for each function (stick them in functions [called xyzTests(), etc or similar])
 * Call your abcTests(), etc functions in main().
 *
 * Write your own tests for each function you are testing, then share/combine
 * tests with a classmate.  Try to come up with tests that will break people's code!
 * Easy tests don't catch many bugs! [Note this is one specific case where you may
 * share code directly with another student.  The test function(s) from other students
 * must be clearly marked (add '_initials' to the function name) and commented with the
 * other student's name.
 *
 * Note: you may NOT use any global variables in your solution.
 * 
 * Errata:
 *   - You can use global vars in your testing functions (if necessary).
 *   - Don't worry about testing the free_list(), or the draw_me() functions.
 *
 * You must compile in C mode (not C++).  If you compile from the commandline
 * run clang, not clang++. Submit your solution files on Canvas.
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 * Ah, the joys of using bitwise operators!
 *
 * Hint: you may want to write helper functions for these two functions
 *
 *********************************************************************/
// Function prototypes
unsigned char isolateBytes(unsigned long arg, int index);
void bubbleSort(unsigned char bytes[8], int size);
unsigned char isolateNibbles(unsigned long arg, int index);
unsigned long byte_sort( unsigned long arg ){
    //Declare an array of btyes (type char bc Bytes is not a type in c)
    unsigned char bytes[8];

    //Accessing individual bytes and store in array
    for(int i =0; i< 8; i++){
        bytes[i] = isolateBytes(arg, i);
    }
    //Sorting the Bytes
    bubbleSort(bytes,8);

    //Convert sorted array into one unsigned long 
    unsigned long sortedBytes = 0;
    unsigned long byte = 0;  
        for(int i =0; i< 8; i++){
        //covert each byte to unsigned long and left shift to correct position 
        byte = (unsigned long)(bytes[i]) << (8*i);
        //OR the sortedBytes with the byte to combine into one long 
        sortedBytes |= byte; 
        }

  return sortedBytes;
}

//Helper function to isolate the bytes 
unsigned char isolateBytes( unsigned long arg, int index){

    //Shift the byte to right to put it in the LMS pos and & with 1's to isolate it
       unsigned char byte = (unsigned char)((arg >> (index *8)) & 0xFF);
    return byte; 

}
//Helper function to sort the bytes array smallest to largest 
void bubbleSort(unsigned char bytes[8], int size){
    //loop through each element in the array
    for(int i =0; i < size -1; i++ ){
        //compare adjacent elements and swap them if they are in the wrong order 
        for(int j=0; j<(size-1)-i; j++){
            if(bytes[j] > bytes[j+1]){
            unsigned char temp = bytes[j]; 
            bytes[j] = bytes[j+1];
            bytes[j+1] = temp; 
            }
        }
    }
     
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort (unsigned long arg)
{
    //Declare an array of nibbles 
    unsigned char nibbles[16];
    //Accessing individual nibble and store in array
    for(int i =0; i< 16; i++){
        nibbles[i] = isolateNibbles(arg, i);
    }
    //Sorting the Bytes
    bubbleSort(nibbles,16);
    
    //Convert sorted array into one unsigned long 
    unsigned long sortedNibbles =0;
    unsigned long nibble=0;  
        for(int i =0; i< 16; i++){
        //covert each byte to unsigned long and left shift to correct position 
        nibble = (unsigned long)(nibbles[i]) << (4*i);
        //OR the sortedBytes with the byte to combine into one long 
        sortedNibbles |= nibble; 
        }

  return sortedNibbles;

}

//Helper function to isolate the bytes 
unsigned char isolateNibbles( unsigned long arg, int index){
    //Shift the byte to right to put it in the LMS pos and & with 1's to isolate it
   unsigned char nibble = (unsigned char)((arg >> (index *4)) & 0xF);

   return nibble; 
}
/*********************************************************************/

typedef struct elt {
  char val;
  struct elt * link;
} Elt;

/*********************************************************************/

/* Forward declaration of "free_list()"... This allows you to call   */
/* free_list() in name_list() [if you'd like].                       */
void free_list( Elt* head ); // [No code goes here!]

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 * 
 * - you must use the "name" variable (change it to be your name).
 *
 * Note, since we're using C, not C++ you can't use new/delete!
 * The analog to delete is the free() function
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *
 * Implement print_list and free_list which should do what you expect.
 * Printing or freeing a nullptr should do nothing.
 *
 * Note: free_list() might be useful for error handling for name_list()... 
 *
 *********************************************************************/

Elt *name_list()
{
   //Declare pointers  
   char * name = "Davison";
   Elt * head = NULL; 
   Elt * current = NULL;  

   //Loop through all the letters in the name stopping at '\0', which marks the end of the string in C
   for(int i =0; name[i] != '\0'; i++){
    //allocate memory equal to the size of struct elt and returns a pointer to that allocated memory
    Elt * pNode = (Elt*)malloc(sizeof(Elt));

    //check if malloc fails  
    if(pNode == NULL){
        printf("Error: malloc failed\n");
        exit(-1);
    }

    //initialize the new node 
    pNode -> val = name[i];
    pNode -> link = NULL; 
    
    //Check if linked list is empty 
    if(head == NULL){
        //set head to point to the first created node
        head = pNode; 
        //update current 
        current = pNode; 
    }
    else{ //LL already contains nodes 
    //set current link to point to the newly created node 
    current -> link = pNode; 
    //update current to be new node 
    current = pNode; 
    }

   } 
   //return the starting point for the LL
   return head;
}

/*********************************************************************/

void print_list( Elt* head )
{ 
    //traverse the LL until the end (points to null)
    while(head != NULL){
        printf("%c ", head->val);
        //assign the value of head->link (which is the address of the next node) to head
        head = head -> link; 
    }
}

/*********************************************************************/

void free_list( Elt* head )
{
    // Create a temp pointer to track the next node
    Elt* temp; 

    while (head != NULL) {
        //set the temp equal to current node 
        temp = head;  
        //move head to the next node
        head = head->link;  
        //free the memory for the current node
        free(temp);  
    }
}

/*********************************************************************
 *
 * draw_me()
 *
 * This function creates a file called 'me.txt' which contains an ASCII-art
 * picture of you (it does not need to be very big).
 * 
 * Use the C stdlib functions: fopen, fclose, fprintf, etc which live in stdio.h
 * - Don't use C++ iostreams
 *
 *********************************************************************/

void draw_me()
{
    // Open file in write mode ('w')
    FILE * pfile = fopen("melanie.txt", "w"); 

    if (pfile != NULL) {
        // writing ASCII art to the file
        fprintf(pfile," /////////////\\\\ \n");
        fprintf(pfile," (((((((((((((( \\\\ \n");
        fprintf(pfile," ))) ~~      ~~  ((( \n");
        fprintf(pfile," ((( (*)     (*) ))) \n");
        fprintf(pfile," )))     <       ((( \n");
        fprintf(pfile," ((( '\'______/`  ))) \n");
        fprintf(pfile," )))'\'___________/(((");
        
        //close the file
        fclose(pfile); 
    } else {
        printf("Error in creating melanie.txt\n");
    }
}

/*********************************************************************
 *
 * Test Code - Place your test functions in this section:
 */

// bool testByteSort() { ... }
// ...
// ...

/*********************************************************************
 *
 * main()
 *
 * The main driver program.  You can place your main() method in this
 * file to make compilation easier, or have it in a separate file.
 *
 *
 *********************************************************************/

int main()
{
     // Test cases
    unsigned long test_cases[] = {
        0x0000000000000000,  // Test Case 1: Input with all zeros
        0xFFFFFFFFFFFFFFFF,  // Test Case 2: Input with all ones
        //0x0102030405060708,  // Test Case 3: Input with sorted bytes
        0x0807060504030201   // Test Case 4: Input with reverse-sorted bytes
    };

    // Expected results for the test cases
    unsigned long expected_results[] = {
        0x0000000000000000,  // Expected result for Test Case 1
        0xFFFFFFFFFFFFFFFF,  // Expected result for Test Case 2
        //0x0102030405060708,  // Expected result for Test Case 3
        0x0102030405060708   // Expected result for Test Case 4 (assuming ascending order)
    };

    // Run tests
    for (int i = 0; i < sizeof(test_cases); ++i) {
        printf("Test Case %d:\n", i + 1);
        unsigned long result = byte_sort(test_cases[i]);
        printf("Sorted result: 0x%016lx\n", result);
        //assert(result == expected_results[i]); // Check if result matches the expected value
        printf("\n");
    }

    return 0;
}
