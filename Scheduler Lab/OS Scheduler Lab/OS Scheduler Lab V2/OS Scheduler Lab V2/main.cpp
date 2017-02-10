//
//  main.cpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/6/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SimulationManager.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    
    /*
    if(argc < 3 || argc > 5){
        printf("Usage: sched [-v] [-s<schedspec>] inputfile randfile");
        return 0;
    } */
    
    SimulationManager sm(argc, argv);
    
    sm.run();
    
    return 0;
}




