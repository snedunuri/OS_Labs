//
//  SimulationManager.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/6/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef SimulationManager_hpp
#define SimulationManager_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include "Process.hpp"
#include "Event.hpp"
#include "Scheduler.hpp"

using namespace std;

class SimulationManager{
    
    
public:
    
    SimulationManager(int ac, char*av[]){
        argc = ac;
        argv = av;
        rand_offset = 0; //-1
        CURRENT_TIME = 0;
        CURRENT_RUNNING_PROCESS = nullptr;
    }
    
    ~SimulationManager(){}
    
    void run();
    
private:
    void read_randfile();
    void read_inputfile();
    int get_next_rand(int burst);
    int decode_args();
    void simulate();
    void assign_scheduler();
    int get_next_event_time();
    void finalize_process(Process* proc);
    void verbose_info(int i, Process* proc);
    void print_all();
    void print_summary_stats();
    Event* get_next_event();
    double calc_io_util();
    
    bool compare_trans (Event* lhs, Event* rhs){
        return lhs->get_state() < rhs->get_state();
    }
    
    int argc;
    char** argv;
    string scheduler_type;
    int num_rands;
    int rand_offset;
    int CURRENT_TIME;
    vector<int> rand_vals;
    vector<Event*> eventq;
    Scheduler *sched;
    Process *CURRENT_RUNNING_PROCESS;
    vector<Process*> finished_procs;
    vector<Interval> interval_arr;
    static const char* const state_arr[];
    int vflag;
    
};

struct OrderProcs {
    bool operator () (Process* lhs, Process* rhs){
        return lhs->get_pid() < rhs->get_pid();
    }
};
#endif /* SimulationManager_hpp */


