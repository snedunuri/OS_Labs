//
//  SimulationManager.hpp
//  IO Lab
//
//  Created by  Satya Nedunuri on 4/16/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef SimulationManager_hpp
#define SimulationManager_hpp

#include <stdio.h>
#include <list>
#include "Scheduler.hpp"
#include "Operation.h"

class Scheduler;

using namespace std;

class SimulationManager{
public:
    SimulationManager(int c, char* v[]){
        argc = c;
        argv = v;
        CURRENT_TIME = 0;
        total_time = 0;
        tot_movement = 0;
        avg_turnaround_time = 0;
        avg_wait_time = 0;
        max_wait_time = 0;
    }
    
    void run();
    
    Operation* CURRENT_RUNNING_OPERATION;
        
private:
    int argc;
    char** argv;
    int sflag;
    int vflag;
    char* svalue;
    unsigned int CURRENT_TIME;
    unsigned int CURRENT_TRACK;
    Scheduler *sched;
    
    //stats
    unsigned int total_time;
    unsigned int tot_movement;
    double avg_turnaround_time;
    double avg_wait_time;
    unsigned int max_wait_time;
    
    
    //data structures
    list<Operation> operation_list;
    list<Operation> finished_ops;
    
    int decode_args ();
    void assign_scheduler();
    void read_input();
    void simulate();
    Operation get_next_op();
    Operation peek_next_op();
    void calc_stats();
    bool has_next();
    void print_info();
};

#endif /* SimulationManager_hpp */
