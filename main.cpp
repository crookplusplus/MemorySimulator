/**
 * Authored by Chris Rook
 * 
*/

#include "memory.h"
#include <iostream>


int main(int argc, char** argv){

    Memory FirstFit(false);
    FirstFit.runSim(10000);
    FirstFit.destroyNodes();

    Memory BestFit(true);
    BestFit.runSim(10000);
    BestFit.destroyNodes();

}