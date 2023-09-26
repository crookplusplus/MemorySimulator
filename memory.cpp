/**
 * Authored by Chris Rook
 * 
*/


#include "memory.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

/// @brief Constructor that expilicitly initializes the Node members. Expected use for the tail Node.
Node::Node(){
    nextNode = nullptr;
    processID = HOLE;
    index = -1;
    size = 0;
}

/// @brief Constructor that initializes Node attributes to specific values
/// @param next next Node in the linked list
/// @param pID Process ID or -1 for hole
/// @param ind index of physicalMemory location
/// @param _size size in memory units
Node::Node(Node* next, int pID, int ind, int _size){
    nextNode = next;
    processID = pID;
    index = ind;
    size = _size;
}

/// @brief Constructor to intialize starting process ID at 0; Defaults to !Best Fit
///hole_root_node is allocated at Memory initialization because the physicalMemory is all empty memory
Memory::Memory(){
    IDCounter = 0;
    nodesTraversed = 0;
    deniedAllocations = 0;
    fragments = 0;
    alloRequests = 0;
    bestFit = false;
    hole_root_node = new Node();
    hole_root_node->size = TOTAL_MEMORY_UNITS;
    hole_root_node->index = 0;
    process_root_node = nullptr;
    for (int i= 0; i<TOTAL_MEMORY_UNITS;i++){
        physicalMemory[i] = HOLE;
    }
        for (int j=0; j<2;j++){
        activeProcesses.push_back(j);
    }
}

/// @brief Overloaded Constructor to intialize starting process ID; used to identify Best Fit technique
Memory::Memory(bool Fit){
    IDCounter = 0;
    nodesTraversed = 0;
    deniedAllocations = 0;
    fragments = 0;
    alloRequests = 0;
    bestFit = Fit;
    hole_root_node = new Node();
    hole_root_node->size = TOTAL_MEMORY_UNITS;
    hole_root_node->index = 0;
    process_root_node = nullptr;
    for (int i= 0; i<TOTAL_MEMORY_UNITS;i++){
        physicalMemory[i] = HOLE;
    }
    for (int j=0; j<2;j++){
        activeProcesses.push_back(j);
    }
}


/// @brief Memory class function that destroys the dynamically allocated nodes
void Memory::destroyNodes(){
    destroyLinkedList(hole_root_node);
    destroyLinkedList(process_root_node);
}

/// @brief Memory member function that generates allocation/deallocation requests randomly
void Memory::requestMaker(){
    int requestType; // 0/1 for allocation/deallocation
    int allocationSize; //random size of memory request
    int randomIndex; //random index of process to be deallocated
    int result;
    int removeProcess;
    int activeSize;

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    
    if (activeProcesses.size()> 5) {
        if (hole_root_node == nullptr){
            requestType = 1;
        }
        else if (hole_root_node != nullptr){
            if((hole_root_node->nextNode == nullptr)&&(hole_root_node->size < 10)){
                requestType = 1;
            }
            else if(activeProcesses.size()> 15){
                requestType = 1;
            }
            else {
                requestType = rand() % 2; 
            }
        }
    }
    else {
        requestType = 0;
    }

    if (requestType == 1) {
        activeSize = activeProcesses.size();
        removeProcess = activeProcesses[activeSize-1];
        result = deallocate_mem(removeProcess);
        
        if (result==1){
            activeProcesses.pop_back();
        }
        else if(result == -1){
            std::cout<<"Deallocation Error with process: "<<removeProcess<<std::endl<<std::flush;
            exit(DEALLOC_ERROR);
        }
        
    }

    else {
        //generates number between 3 and 10
        allocationSize =  rand() % 8 + 3; 
        result = allocate_mem(IDCounter, allocationSize);
        if (result ==-1){
            deniedAllocations++;
        }
        else{
            nodesTraversed += result;
        }
        if((result!=-1)&&(activeProcesses.size() > 2)){
            //insert Process ID's at random index to take advantage of vector::pop_back function for deallocation
            randomIndex = rand() % activeProcesses.size();
            activeProcesses.insert(activeProcesses.begin()+ randomIndex, 1, IDCounter);
        }
        else if (alloRequests>=2){
            activeProcesses.push_back(IDCounter);
        }
        IDCounter++;
        alloRequests++;
        int num = fragment_count();
        fragments = fragments + num;
    }
}

/// @brief Allocates num_units units of memory to a process whose
//id is process_id.
/// @param process_id ID for process request memory allocation
/// @param num_units size of memory allocation
/// @return If successful, it returns the number of nodes traversed in the linked list. Otherwise, 
//it returns -1.
int Memory::allocate_mem(int process_id, int num_units){
    Node* hole;
    int nodeCount = 0;
    int* pNodeCount = &nodeCount;
    
    if (!bestFit){
        hole = findFirstFit(hole_root_node, num_units, &nodeCount);

        if (hole == nullptr){
            return -1;
        }
        else {
            writeToMem(hole, num_units, process_id);
            return *pNodeCount;
        }
    }
    else{
        hole = findBestFit(hole_root_node, num_units, &nodeCount);
        if ((hole == nullptr)&&(hole_root_node != nullptr)){
            return -1;
        }
        else {
            writeToMem(hole, num_units, process_id);
            return *pNodeCount;
        }
    }
    return -1;
}

void Memory::writeToMem(Node* hole, int num_size, int process_id){
    Node* search = process_root_node;
    bool found = false; //used to find position of process in linked list
    //write the processID to memory block
    for(int i=0; i <num_size;i++){
        if (physicalMemory[hole->index +i]!=-1){
            std::cout<<"Overwrite attempted by process "<<process_id
            <<" at index: "<<hole->index +i<<"; Memory allocated to "
            <<physicalMemory[hole->index +i]<<std::endl<<std::flush;
        }
        else{
            physicalMemory[hole->index +i] = process_id;
        }
    }
    //create process node for root process linked list
    if (search == nullptr) {
        process_root_node = new Node(nullptr, process_id, hole->index, num_size);
    }
    //Pointer that creates new Node
    else{
        Node* someNode = new Node(nullptr, process_id, hole->index, num_size);
        Node* temp;
        //search process list for position
        while(!found){
            //**************
            if (search->index < someNode->index){
                if (search->nextNode == nullptr){
                    search->nextNode = someNode;
                    found = true;
                }
                else if(search->nextNode->index > someNode->index){
                    temp = search->nextNode;
                    someNode->nextNode = temp;
                    search->nextNode = someNode;
                    found = true;
                }
            }
            if (search->index > someNode->index){
                if(search==process_root_node){
                    temp = process_root_node;
                    process_root_node = someNode;
                    process_root_node->nextNode = temp;
                    found=true;
                }
            }

            else {
                search = search->nextNode;
            }
        }
    }
    
    //adjust hole node members
    if(hole->size > num_size){
        hole->size -= num_size;
        hole->index += num_size;
    }
    //remove hole node if there is a perfect memory fit
    else if (hole->size == num_size){
        Node* temp = hole->nextNode; 
        search = hole_root_node;
    
        //look for previousHole node for proper linking

        while(search != nullptr){
            if(search == hole_root_node){
                hole_root_node = temp;
                delete(hole);
            }
            else if (search->nextNode == hole){
                if(temp == nullptr){
                    search->nextNode = nullptr;
                }
                else{
                    search->nextNode = temp;
                }
                delete(hole);
                break;
            }
            search = search->nextNode;
        }
    }
    found = false;
}

/// @brief deallocates the memory allocated to the process whose id is process_id.
/// @param process_id ID of the process that is requesting deallocation
/// @return It returns 1, if successful, otherwise –1.
int Memory::deallocate_mem(int process_id){
    int deallocated = -1; //return value 
    int memoryCheck = 0; //increments as memory units are deallocated to compare to mem units that were original allocated
    bool found = false;
    Node* processNode = process_root_node;
    Node* previousProcess = process_root_node;
    Node* hole;

    //Find node that corresponds to the processID
    //head node is the process's node
    if (processNode->processID == process_id){
        found = true;
    }
    //process's node is not head
    else {
        while(!found){
            if(previousProcess->nextNode == nullptr){
                std::cout<<"No process node exists for process ID: "<<process_id<<std::endl<<std::flush;
                return deallocated;
            }
            if(previousProcess->nextNode->processID == process_id){
                processNode = previousProcess->nextNode;
                found = true;

            }
            if(!found){
                previousProcess = previousProcess->nextNode;
            }
        }
    }
    //remove from memory

    for (int i=0; i< processNode->size; i++){
        if (physicalMemory[processNode->index +i]== process_id){
            physicalMemory[processNode->index +i] = HOLE;
            memoryCheck++;
        }
    }

    //Error check for memory unit mismatch
    if (memoryCheck != processNode->size){
        std::cout<<"Simulated Segmentation Fault at process ID: "<<process_id<<std::endl;
        std::cout<<"Allocated Memory Size:"<<processNode->size<<"; Memory Deallocated: "<<memoryCheck<<";"
        <<std::endl<<std::flush;
    }
    
    
    //make hole node
    hole = new Node(nullptr, HOLE, processNode->index, memoryCheck);
    found = false;

    //hole is new head
    if (hole_root_node == nullptr){
        hole_root_node = hole;
    }
    else if(hole->index < hole_root_node->index){
        found = true;
        //check for absorbtion
        if(hole_root_node->index == (hole->index + hole->size)){
            hole_root_node->size += hole->size;
            hole_root_node->index -= hole->size;
        }
        else{
            hole->nextNode = hole_root_node;
            hole_root_node = hole;
        }
    }
    else if(hole_root_node->nextNode == nullptr){
        hole_root_node->nextNode = hole;
    }
    /******MUST CONSIDER THE SIZE*/
    //search for position
    Node* previousHole = hole_root_node;
    while (!found){
        //hole is end
        if ((previousHole->index < hole->index) && (previousHole->nextNode == nullptr)){
            //check for absorbtion left
            if ((previousHole->index + previousHole->size) == hole->index){
                previousHole->size += hole->size;
                delete(hole);
                found = true;
            }
            else{
                //link
                previousHole->nextNode = hole;
            }
        }
        //insert between two nodes
        else if((previousHole->index < hole->index)&&(previousHole->nextNode->index > hole->index)){
            //check for absorbtion left
            if ((previousHole->index + previousHole->size) == hole->index){
                previousHole->size += hole->size;
                delete(hole);
                found = true;
            }
            //check absorbtion right
            else if ((hole->index + hole->size) == previousHole->nextNode->index){
                previousHole->nextNode->index = hole->index;
                previousHole->nextNode->size += hole->size;
                delete(hole);
                found = true;
            }
            //link
            else {
                hole->nextNode = previousHole->nextNode;
                previousHole->nextNode = hole;
                found = true;
            }
        }
        previousHole = previousHole->nextNode;
        
    }


    //adjust linking of process nodes
    //process's node is head node
    if(processNode==process_root_node){
        //edge case for one process node
        if(processNode->nextNode == nullptr){
            process_root_node = nullptr;
            delete(processNode);
        }
        else{
            process_root_node = process_root_node->nextNode;
            delete(processNode);
            deallocated = 1;
        }
    }
    //process's node is last node
    else if(processNode->nextNode == nullptr){
        previousProcess->nextNode = nullptr;
        delete(processNode);
        deallocated = 1;
    }
    //typical case
    else {
        previousProcess->nextNode = processNode->nextNode;
        delete(processNode);
        deallocated = 1;
    }

    previousHole = hole_root_node;
    Node* temp = hole_root_node;
    
    //clean hole linked list
    if (hole_root_node != nullptr){
        while (previousHole->nextNode!=nullptr){
            if ((previousHole->index + previousHole->size)==previousHole->nextNode->index){
                temp = previousHole->nextNode;
                previousHole->size += temp->size;
                previousHole->nextNode = temp->nextNode;
                delete(temp);
            }
            if (previousHole->nextNode!=nullptr){   
                previousHole = previousHole->nextNode;
            }
        }
    }

    return deallocated;
}

/// @brief Method that counts fragments of memory that are 2 units or less
/// @return count of the fragments found
int Memory::fragment_count(){
    int count = 0;
    Node* iterator = hole_root_node;

    if (hole_root_node == nullptr){
        return count;
    }
    else{
        while(iterator!=nullptr){
            if(iterator->size <=2){
                count++;
            }
            iterator = iterator->nextNode;
        }
    }
    return count;
}

/// @brief Memory class function that can be called to run the entire simulation
/// @param numRequests Number of Memory requests to simulate
void Memory::runSim(int numRequests){

    while(numRequests != 0){
        this->requestMaker();
        numRequests--;
    }
    this->statsReport();
}

/// @brief Memory class function that outputs the simulation results to the terminal
void Memory::statsReport() {
    if (alloRequests != 0) {
        double nodesTraversedPerRequest = (double)nodesTraversed / alloRequests;
        double deniedAllocationsPerRequest = (double)deniedAllocations / alloRequests;
        double fragmentsPerRequest = (double)fragments / alloRequests;

        std::cout << std::fixed << std::setprecision(6); // This sets the precision for all following output

        std::cout << "Number of Nodes Traversed per Allocation Request: " << nodesTraversedPerRequest << std::endl;
        std::cout << "Number of Denied Allocations per Allocation Request: " << deniedAllocationsPerRequest << std::endl;
        std::cout << "Number of External Fragments per Allocation Request: " << fragmentsPerRequest << std::endl;
    } else {
        std::cout << "No allocation requests have been made." << std::endl;
    }
}

/// @brief Method that recursively destroys the dynamically allocated Nodes
/// @param node pointer that points to a node to be destroyed
void destroyLinkedList(Node* node) {
    if (node == nullptr){
        return;
    }

    destroyLinkedList(node->nextNode);

    delete(node);
}

/// @brief Helper method used to print the processID member of the Nodes in the linked list
/// @param node Root node
void printLinkedListStatus(Node* node){
    Node* pNode = node;

    if (pNode == nullptr){
        return;
    }
    else if (pNode->processID == HOLE) {
        while(pNode != nullptr){
        //prints new line for clean output and troubleshooting
        
        std::cout<<"("<<pNode->index<< ", "<<pNode->size<<")"<<std::flush;
        pNode = pNode->nextNode;
        
        }
    }
    else {
        while(pNode != nullptr){
        //prints new line for clean output and troubleshooting
        
        std::cout<<"("<<pNode->index<< ", "<<pNode->processID<<", "<<pNode->size<<") -> "<<std::flush;
        pNode = pNode->nextNode;
        }
    }
    std::cout<<std::endl<<std::flush;
}

/// @brief Prints the array values for comparing the linked lists to the memory array
/// @param mem pointer to the Memory object being tested
void printMemory(Memory* mem){
    int process = mem->physicalMemory[0];
    
    for(int i=0; i<TOTAL_MEMORY_UNITS; i++){
        if(process == mem->physicalMemory[i]){
            std::cout<< mem->physicalMemory[i] <<" " <<std::flush;
        }
        else {
            process = mem->physicalMemory[i];
            std::cout<<std::endl;
            std::cout<<i<<"- "<< mem->physicalMemory[i] << " " <<std::flush;
        }
    }
}

/// @brief Helper method that iterates through the list of holes for first fit
/// @param holeRoot Node pointer to root of the linked list of empty memory
/// @param num_units Number of memory units for the request
/// @param nodeCount Number of Nodes transversed to find hole
/// @return Node pointer to the hole that fits. If there is no fit function returns nullptr
Node* findFirstFit(Node* holeRoot, int num_units, int* nodeCount){
    Node* iterator = holeRoot;

    while(iterator != nullptr){
        (*nodeCount)++;
        if (iterator->size >= num_units) {
            return iterator;
        }
        iterator = iterator->nextNode;
    }

    return nullptr;
}

/// @brief Function that finds the best fit memory location for the memory request
/// @param holeRoot Node pointer to root of the linked list of empty memory
/// @param num_units Number of memory units for the request
/// @param nodeCount Number of Nodes transversed to find hole
/// @return Node pointer to the hole that fits. If there is no fit function returns nullptr
Node* findBestFit(Node* holeRoot, int num_units, int* nodeCount){
    Node* best = nullptr;
    Node* iterator = holeRoot;
    int diff = 0;
    int previousDiff = -1; 

    if(holeRoot == nullptr){
        return nullptr;
    }

    else if((holeRoot->nextNode==nullptr)&&(holeRoot->size>num_units)){
        best = holeRoot;
    }

    while(iterator!=nullptr){
        (*nodeCount)++;
        //best case
        if (iterator->size==num_units){
            return iterator;
        }
        else if(iterator->size > num_units){
            diff = iterator->size - num_units;
            if(previousDiff>= 0){
                if(diff < previousDiff){
                    best = iterator;
                }
            }
            previousDiff = diff;
        }
        iterator = iterator->nextNode;
    }
    return best;
}