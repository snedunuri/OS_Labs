//
//  main.cpp
//  Memory Management Lab
//
//  Created by  Satya Nedunuri on 3/28/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include <iostream>
#include "SimulationManager.hpp"

int main(int argc, char * argv[]) {
   
    SimulationManager sm(argc, argv);
    sm.run();
    
    return 0;
}
