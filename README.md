# Memory Allocation Simulation
&nbsp;&nbsp; This project simulates memory allocation and deallocation in an operating system, evaluating two memory allocation techniques: First Fit and Best Fit. The simulation employs a linked list to manage memory usage.

## Purpose
&nbsp;&nbsp; The purpose of this memory allocation simulation project is to create a platform for studying and evaluating two fundamental memory allocation techniques, namely First Fit and Best Fit, commonly used in operating systems. Memory allocation plays a crucial role in the efficient utilization of a computer's main memory, impacting system performance and resource management.

&nbsp;&nbsp; This program also demonstrates the application and utilization of pointers and linked lists in C++.

## Simulation Execution
&nbsp;&nbsp; To run the simulation, we generate 10,000 memory allocation and deallocation requests using the Request Generation component. For each request, we invoke the appropriate function of the Memory component for both memory allocation techniques. After each request, the performance parameters are updated.


## Assumptions 
- Memory is 256KB each.
- A process may request between 3 and 10 units of memory.
- Memory is requested for each process only once.
- The entire memory allocated to a process is deallocated on a deallocation request.

# Getting Started
1. Clone this repository to your local machine.
2. Build the executables with the following command: _make ./sim_
3. Run the program with the following command: _./sim_

# Results
&nbsp;&nbsp; At the end of the simulation, the simulation statistics will print the performance parameters for both memory allocation techniques.