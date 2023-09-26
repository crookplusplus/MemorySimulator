/**
 * Authored by Chris Rook
 * 
*/




#ifndef MEMORY_H
#define MEMORY_H

#include <vector>


/**
 * Macros for program constants
*/

#define TOTAL_MEMORY_UNITS 128 // (256KB total memory/ 2KB memory unit)
#define REQUEST_MIN 3 //used for bounds checking for memory allocation requests
#define REQUEST_MAX 10 //used for bounds checking for memory allocation requests
#define HOLE -1 // value used to indicate a hole in memory
#define NUM_OF_REQUEST 10000 //num of processes to test
#define DEALLOC_ERROR 5//Indidcates program is unable to deallocate a process from memory

/// @brief The node class represents a memory unit and makes up the linked list of the total memory
class Node{
    public:

    Node* nextNode; // node points to next node that is either of a different process or hole
    int processID; //value of -1 indicates the memory unit is a hole, and allocated memory otherwise
    int index; //the index of the starting memory unit that is either allocated or a hole
    int size; //size of hole or allocated memory 

    /// @brief Constructor that expilicitly initializes the Node members. Expected use for the tail Node.
    Node();

    /// @brief Constructor that initializes Node attributes to specific values
    /// @param next next Node in the linked list
    /// @param pID Process ID or -1 for hole
    /// @param ind index of physicalMemory location
    /// @param _size size in memory units
    Node(Node *next, int pID, int ind, int _size);
};

/// @brief Memory object that contains all memory available
class Memory{
    public:

    int IDCounter; //int that increments to create unique process IDs
    int nodesTraversed; // total number of nodes traversed
    int deniedAllocations; //total number of denied allocation requests
    int fragments; //total number of external fragments
    int alloRequests; //total number of allocation requests
    bool bestFit;
    Node* hole_root_node; //root node of linked list that points to available holes in memory
    Node* process_root_node; //root node of linked list that points to first memory unit of allocated memory for a process 
    int physicalMemory[TOTAL_MEMORY_UNITS] = {HOLE};
    std::vector<int> activeProcesses;

    /// @brief Constructor to intialize starting process ID at 0; Defaults to !Best Fit
    Memory();
    /// @brief Overloaded Constructor to intialize starting process ID; used to identify Best Fit technique
    Memory(bool bestFit);

    /// @brief Method that randomly generates allocation/deallocation requests
    void requestMaker();

    /// @brief Memory helper function to deallocate the nodes of the linkedLists
    void destroyNodes();

    /// @brief Allocates num_units units of memory to a process whose
    //id is process_id.
    /// @param process_id ID for process request memory allocation
    /// @param num_units size of memory allocation
    /// @return If successful, it returns the number of nodes traversed in the linked list. Otherwise, 
    //it returns -1.
    int allocate_mem(int process_id, int num_units);

    void writeToMem(Node* hole, int num_size, int process_id);

    /// @brief deallocates the memory allocated to the process whose id is process_id.
    /// @param process_id ID of the process that is requesting deallocation
    /// @return It returns 1, if successful, otherwise –1.
    int deallocate_mem(int process_id);

    /// @brief Method that counts fragments of memory that are 2 units or less
    /// @return count of the fragments found
    int fragment_count();

    /// @brief Generates stats reports
    void statsReport();

    void runSim(int numRequests);

    void statusReport();

};

/// @brief Method that recursively destroys the dynamically allocated Nodes
/// @param node pointer that points to a node to be destroyed
void destroyLinkedList(Node* node);

/// @brief Helper method used to print the processID member of the Nodes in the linked list
/// @param node Root node
void printLinkedListStatus(Node* node);

/// @brief Prints the array values for comparing the linked lists to the memory array
/// @param mem pointer to the Memory object being tested
void printMemory(Memory* mem);

/// @brief Helper method that iterates through the list of holes for first fit
/// @param holeRoot Node pointer to root of the linked list of empty memory
/// @param num_units Number of memory units for the request
/// @param nodeCount Pointer to the variable location that keeps track of the nodes traversed
/// @return Node pointer to the hole that fits. If there is no fit function returns nullptr
Node* findFirstFit(Node* holeRoot, int num_units, int* nodeCount);

/// @brief Helper method that iterates through the hole nodes to find the best fit
/// @param holeRoot Node pointer to root of the linked list of empty memory
/// @param num_units Number of memory units for the request
/// @param nodeCount Pointer to the variable location that keeps track of the nodes traversed
/// @return Node pointer to the hole that fits. If there is no fit function returns nullptr
Node* findBestFit(Node* holeRoot, int num_units, int* nodeCount);


#endif