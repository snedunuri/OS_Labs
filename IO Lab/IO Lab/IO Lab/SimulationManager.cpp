//
//  SimulationManager.cpp
//  IO Lab
//
//  Created by  Satya Nedunuri on 4/16/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>
#include "SimulationManager.hpp"
#include "Scheduler.hpp"

using namespace std;

void
SimulationManager::simulate(){
    
    //initialize vars
    CURRENT_TIME = 0;
    CURRENT_TRACK = 0;
    CURRENT_RUNNING_OPERATION = nullptr;
    if (vflag) printf("TRACE\n");
    
    while (has_next()) {
        
        if(CURRENT_RUNNING_OPERATION == nullptr){
            
            if(sched->isEmpty()){
                //get the next operation from list and add it to the scheduler
                Operation cur_op = get_next_op();
                CURRENT_TIME = cur_op.time;
                cur_op.submission_time = CURRENT_TIME;
                sched->add(cur_op);
                if(vflag) printf("%d:%6d add %d\n", CURRENT_TIME, cur_op.opid, cur_op.track);
            }
            
            //get the next operation from the scheduler and issue it
            Operation temp = sched->get(CURRENT_TRACK);
            CURRENT_RUNNING_OPERATION = &temp;
            CURRENT_RUNNING_OPERATION->issue_time = CURRENT_TIME;
            int track_movement = abs((int) (CURRENT_RUNNING_OPERATION->track - CURRENT_TRACK));
            CURRENT_RUNNING_OPERATION->movement = track_movement;
            CURRENT_RUNNING_OPERATION->completion_time = CURRENT_RUNNING_OPERATION->issue_time + track_movement;
            if(vflag) printf("%d:%6d issue %d %d\n", CURRENT_TIME, CURRENT_RUNNING_OPERATION->opid,
                             CURRENT_RUNNING_OPERATION->track, CURRENT_TRACK);
            
        } else {
            Operation temp = peek_next_op();
            
            if(!operation_list.empty() && temp.time <= CURRENT_RUNNING_OPERATION->completion_time){
                
                Operation cur_op = get_next_op();
                CURRENT_TIME = cur_op.time;
                cur_op.submission_time = CURRENT_TIME;
                sched->add(cur_op);
                if(vflag) printf("%d:%6d add %d\n", CURRENT_TIME, cur_op.opid, cur_op.track);
                
            } else {
                
                finished_ops.push_back(*CURRENT_RUNNING_OPERATION);
                if(vflag) printf("%d:%6d finish %d\n", CURRENT_RUNNING_OPERATION->completion_time, CURRENT_RUNNING_OPERATION->opid, CURRENT_RUNNING_OPERATION->completion_time - CURRENT_RUNNING_OPERATION->submission_time);
                CURRENT_TIME = CURRENT_RUNNING_OPERATION->completion_time;
                CURRENT_TRACK = CURRENT_RUNNING_OPERATION->track;
                CURRENT_RUNNING_OPERATION = nullptr;
                
            }
        }
    }
}

void
SimulationManager::print_info(){
    if(vflag){
        finished_ops.sort(order_by_id());
        
        printf("IOREQS INFO\n");
        
        list<Operation>::iterator it = finished_ops.begin();
        for(int i = 0 ; i < finished_ops.size(); i++){
            printf("%5d: %5d %5d %5d\n", it->opid, it->submission_time, it->issue_time, it->completion_time);
            it++;
        }
    }
}

bool
SimulationManager::has_next(){
    return !operation_list.empty() ||
           !sched->isEmpty() ||
           CURRENT_RUNNING_OPERATION != nullptr;
}

void
SimulationManager::calc_stats(){
    total_time = CURRENT_TIME;
    
    list<Operation>::iterator iter = finished_ops.begin();
    unsigned int max = 0;
    for(int i = 0; i < finished_ops.size(); i++){
        tot_movement += iter->movement;
        avg_turnaround_time += iter->completion_time - iter->submission_time;
        
        //calculate the average wait time
        unsigned int wait_time = iter->issue_time - iter->submission_time;
        avg_wait_time += wait_time;
        if (wait_time > max)
            max = wait_time;
        
        iter++;
    }
    
    avg_turnaround_time /= finished_ops.size();
    avg_wait_time /= finished_ops.size();
    max_wait_time = max;
    
    //print the results
    printf("SUM: %d %d %.2lf %.2lf %d\n",
           total_time,
           tot_movement,
           avg_turnaround_time,
           avg_wait_time,
           max_wait_time);
}

Operation
SimulationManager::peek_next_op(){
    Operation temp = operation_list.front();
    return temp;
}

Operation
SimulationManager::get_next_op(){
    Operation temp = operation_list.front();
    operation_list.pop_front();
    return temp;
}

void
SimulationManager::run(){
    decode_args();
    assign_scheduler();
    read_input();
    simulate();
    print_info();
    calc_stats();
}

int
SimulationManager::decode_args(){
    sflag = 0;
    vflag = 0;
    svalue = NULL;
    int c;
    
    opterr = 0;
    while ((c = getopt (argc, argv, "s:v")) != -1)
        switch (c)
    {
        case 's':
            sflag = 1;
            svalue = optarg;
            break;
        case 'v':
            vflag = 1;
            break;
        case '?':
            if (optopt == 'a' || optopt == 'o' || optopt == 'f' )
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort();
    }
    
    return 0;
}

void
SimulationManager::assign_scheduler(){
    switch (svalue[0]) {
        case 'i':
            sched = new FIFO();
            break;
        case 'j':
            sched = new SSTF();
            break;
        case 's':
            sched = new SCAN();
            break;
        case 'c':
            sched = new CSCAN();
            break;
        case 'f':
            sched = new FSCAN();
            break;
    }
}

void
SimulationManager::read_input(){
    ifstream infile;
    infile.open(argv[argc-1]);
    string line;
    
    if(infile.is_open()){
        unsigned int id = 0;
        while( getline(infile, line)){
            if(line.substr(0,1) == "#")
                continue;
            else{
                stringstream ssin(line);
                string instr[2];
                
                ssin >> instr[0]; //time step
                ssin >> instr[1]; //track
                
                unsigned int time = stoi(instr[0]);
                unsigned int track = stoi(instr[1]);
                
                operation_list.push_back({time, track, id, 0, 0, 0, 0});
                id++;
            }
        }
        infile.close();
    } else {
        cout << "Error: Input file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }
}