//
//  SimulationManager.cpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/6/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "SimulationManager.hpp"
#include "Event.hpp"
#include "Scheduler.hpp"
#include "FCFSScheduler.hpp"
#include "LCFSScheduler.hpp"
#include "SJFScheduler.hpp"
#include "RRScheduler.hpp"
#include "PRIOScheduler.hpp"

using namespace std;

const char* const SimulationManager::state_arr[] = { "%d %d %d: CREATED -> READY",
                                                     "%d %d %d: READY -> RUNNG cb=%d rem=%d prio=%d",
                                                     "%d %d %d: RUNNG -> BLOCK  ib=%d rem=%d",
                                                     "%d %d %d: BLOCK -> READY",
                                                     "%d %d %d: Done",
                                                     "%d %d %d: RUNNG -> READY  cb=%d rem=%d prio=%d"};



void
SimulationManager::simulate(){
    
    bool PROCESS_RUNNING = false;
    while(!eventq.empty() || !sched->empty()){
        bool CALL_SCHEDULER = false;
        Event* evt = get_next_event();
        Process *proc = evt->get_process();
        
        CURRENT_TIME = evt->get_timestamp();
        
        proc->set_time_in_prev_state(CURRENT_TIME - proc->get_state_ts());
        
        if(proc->get_state() == READY){
            proc->set_cpu_wait_time(proc->get_cpu_wait_time() + proc->get_time_in_prev_state());
        }
        
        switch(evt->get_state()){
                
            case TRANS_TO_READY:
            {
                if(proc->get_state() == CREATE){
                    verbose_info(0, proc);
        
                }
                
                else if (proc->get_state() == BLOCK){
                    verbose_info(3, proc);
                    proc->set_dynamic_prio(proc->get_static_prio() - 1);
                }
                
                if(CURRENT_RUNNING_PROCESS == nullptr){
                    
                    CALL_SCHEDULER = true;
                    
                }
                
                sched->add_event(evt);
                proc->set_state_ts(CURRENT_TIME);
                proc->set_state(READY);
                break;
            }
                
            case TRANS_TO_RUN:
            {
                
                PROCESS_RUNNING = true;
                
                proc->set_state_ts(CURRENT_TIME);
                
                if(proc->get_remaining_exec_time() > 0){
                    
                    //for rr and prio we have to worry about preemption as well as blocking
                    if(dynamic_cast<RRScheduler*>(sched) || dynamic_cast<PRIOScheduler*>(sched)){
                        
                        proc->set_cpu_burst(proc->get_cpu_burst() - sched->get_quantum());
                        proc->set_remaining_exec_time(proc->get_remaining_exec_time() - sched->get_quantum());
                        
                        if(proc->get_cpu_burst() <= 0){
                            Event* new_evt = new Event(proc, TRANS_TO_BLOCK, CURRENT_TIME + proc->get_cpu_burst() + sched->get_quantum());
                            add_event(new_evt);
                        } else {
                            Event* new_evt = new Event(proc, TRANS_TO_PREEMPT, CURRENT_TIME + sched->get_quantum());
                            add_event(new_evt);
                        }
                        
                    } else { //for the other schedulers we only need to create blocking events
                        Event* new_evt = new Event(proc, TRANS_TO_BLOCK, CURRENT_TIME + proc->get_cpu_burst());
                       add_event(new_evt);
                    }
                    proc->set_state(RUN);
                   
                    break;
                } else {
                    
                    finalize_process(proc);
                    if(!eventq.empty()){
                      CURRENT_RUNNING_PROCESS = nullptr;
                    }
                    break;
                }
            }
                
            case TRANS_TO_BLOCK:
            {
                //decrease remaining execution time by the time process spends running
                proc->set_remaining_exec_time(proc->get_remaining_exec_time() - proc->get_cpu_burst());
                
                //setting the io burst
                proc->set_io_burst(get_next_rand(proc->get_IO()));
                
                proc->set_state_ts(CURRENT_TIME);
                
                if(proc->get_remaining_exec_time() != 0){
                    Event* new_evt = new Event(proc, TRANS_TO_READY, CURRENT_TIME + proc->get_io_burst());
                    add_event(new_evt);
                    proc->set_io_time(proc->get_io_time() + proc->get_io_burst());
                    //used to calculate IO utilization
                    interval_arr.push_back({CURRENT_TIME, CURRENT_TIME + proc->get_io_burst(), proc->get_pid()});
                    verbose_info(2, proc);
                } else {
                    //process is finished running
                    finalize_process(proc);
                }
                
                PROCESS_RUNNING = false;
                CURRENT_RUNNING_PROCESS = nullptr;
                CALL_SCHEDULER = true;
                proc->set_state(BLOCK);
                break;
            }
                
            case TRANS_TO_PREEMPT:
            {
                verbose_info(5, proc);
                proc->set_state_ts(CURRENT_TIME);
                proc->set_dynamic_prio(proc->get_dynamic_prio() - 1);
                sched->add_event(evt);
                CALL_SCHEDULER = true;
                proc->set_state(PREEMPT);
                CURRENT_RUNNING_PROCESS = nullptr;
                break;
            }
        }
       
        
        if (CALL_SCHEDULER){
            
            if(get_next_event_time() == CURRENT_TIME){
                continue;
            }
            
            CALL_SCHEDULER = false;
            
            if(CURRENT_RUNNING_PROCESS == nullptr){
                Event* evt2 = sched->get_next_event();
                if(evt2 == nullptr){
                    continue;
                } else {
                    CURRENT_RUNNING_PROCESS = evt2->get_process();
                }
            }
            
            //create event to make process runnable for some time
            Event* new_evt;
            if (!PROCESS_RUNNING)
                new_evt = new Event(CURRENT_RUNNING_PROCESS, TRANS_TO_RUN, CURRENT_TIME);
            else
                new_evt = new Event(CURRENT_RUNNING_PROCESS, TRANS_TO_RUN, CURRENT_TIME);
            
            add_event(new_evt);
            
            
            //setting the cpu burst
            if (!(dynamic_cast<RRScheduler*>(sched) || dynamic_cast<PRIOScheduler*>(sched))){
                CURRENT_RUNNING_PROCESS->set_cpu_burst(min(get_next_rand(CURRENT_RUNNING_PROCESS->get_CB()),
                                                       CURRENT_RUNNING_PROCESS->get_remaining_exec_time()));
            } else {
                if(CURRENT_RUNNING_PROCESS->get_cpu_burst() <= 0){
                    CURRENT_RUNNING_PROCESS->set_cpu_burst(min(get_next_rand(CURRENT_RUNNING_PROCESS->get_CB()),
                                                               CURRENT_RUNNING_PROCESS->get_remaining_exec_time()));
                }
            }
            
            
            CURRENT_RUNNING_PROCESS->set_time_in_prev_state(CURRENT_TIME - CURRENT_RUNNING_PROCESS->get_state_ts());
            
            CURRENT_RUNNING_PROCESS->set_cpu_wait_time(CURRENT_RUNNING_PROCESS->get_cpu_wait_time() + CURRENT_RUNNING_PROCESS->get_time_in_prev_state());
            
            verbose_info(1, CURRENT_RUNNING_PROCESS);
            
            CURRENT_RUNNING_PROCESS->set_state_ts(CURRENT_TIME);
            
        }
    
    }

}

void
SimulationManager::add_event(Event *evt){
    list<Event*>::iterator it = eventq.begin();
    for(int i = 0; i < eventq.size(); i++){
        Event* ptr = *it;
        if(evt->get_timestamp() >= ptr->get_timestamp()){
            it++;
        } else {
            break;
        }
    }
    eventq.insert(it, evt);
}

Event*
SimulationManager::get_next_event(){
    
    if(!eventq.empty()){
        Event* evt = eventq.front();
        eventq.erase(eventq.begin());
        return evt;
    } else if (!sched->empty()){
        return sched->get_next_event();
    } else {
        return nullptr;
    }
}

int
SimulationManager::get_next_event_time(){
    
    if (eventq.empty()){
        return -1;
    }
    
    if(!eventq.empty()){
        Event* evt = eventq.front();
        return evt->get_timestamp();
    } else if (!sched->empty()){
        return sched->peek_next_event()->get_timestamp();
    } else {
        return -1;
    }
    
}

double
SimulationManager::calc_io_util(){
    sort(interval_arr.begin(), interval_arr.end(), OrderIntervals());
    
    int index = 0;
    
    for(int i = 0; i < interval_arr.size(); i++){
        
        if(index != 0 && interval_arr[index-1].start_time <= interval_arr[i].end_time){
            
            while(index != 0 && interval_arr[index-1].start_time <= interval_arr[i].end_time){
                interval_arr[index-1].end_time = max(interval_arr[index-1].end_time, interval_arr[i].end_time);
                interval_arr[index-1].start_time = min(interval_arr[index-1].start_time, interval_arr[i].start_time);
                index--;
            }

        } else {
            interval_arr[index] = interval_arr[i];
        }
        index++;
    }
    
    int io_total = 0;
    for (int i = 0; i < index; i++)
        io_total += interval_arr[i].end_time - interval_arr[i].start_time;
    
    return io_total;
}

void
SimulationManager::print_all(){
    //print scheduler type
    sched->print_scheduler_info();
    
    //sort the processes by pid before printing
    sort(finished_procs.begin(), finished_procs.end(), OrderProcs());
    
    //print all the process information
    for(int i = 0; i < finished_procs.size(); i++)
        finished_procs[i]->print_process_info();
    
    //print summary statistics
    print_summary_stats();
}

void
SimulationManager::print_summary_stats(){
    
    long num_procs = finished_procs.size();
    double throughput = (double) num_procs / CURRENT_TIME * 100.0;
    double cpu_util = 0.0;
    double io_util = 0.0;
    double avg_turnaround = 0.0;
    double avg_cpu_wait = 0.0;
    
    for(int i = 0; i < num_procs; i++){
        Process* proc = finished_procs[i];
        cpu_util += proc->get_total_cpu_time();
        io_util += proc->get_io_time();
        avg_turnaround += proc->get_turnaround();
        avg_cpu_wait += proc->get_cpu_wait_time();
    }
    
    cpu_util = cpu_util / CURRENT_TIME * 100;
    io_util = calc_io_util() / CURRENT_TIME * 100;
    avg_turnaround = avg_turnaround / num_procs;
    avg_cpu_wait = avg_cpu_wait /num_procs;
    
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", CURRENT_TIME, cpu_util, io_util,
                                                      avg_turnaround, avg_cpu_wait, throughput);
}

void
SimulationManager::verbose_info(int i, Process* proc){
    if(vflag) {
        char buffer[100];
        if(i == 0 || i == 3 || i == 4){
            snprintf(buffer, sizeof(buffer), state_arr[i], CURRENT_TIME,
                     proc->get_pid(), proc->get_time_in_prev_state());
        } else if (i == 1 || i == 5) {
            snprintf(buffer, sizeof(buffer), state_arr[i], CURRENT_TIME,
                     proc->get_pid(), proc->get_time_in_prev_state(),
                     proc->get_cpu_burst(), proc->get_remaining_exec_time(),
                     proc->get_dynamic_prio());
        } else if (i == 2) {
            snprintf(buffer, sizeof(buffer), state_arr[i], CURRENT_TIME,
                     proc->get_pid(), proc->get_time_in_prev_state(),
                     proc->get_io_burst(), proc->get_remaining_exec_time());
        }
        string str(buffer);
        cout << str << endl;
    }
}

void
SimulationManager::finalize_process(Process* proc){
    proc->set_finishing_time(CURRENT_TIME);
    proc->set_turnaround_time(CURRENT_TIME - proc->get_arrival_time());
    proc->set_io_time(proc->get_io_time());
    finished_procs.push_back(proc);
    rand_offset--; //need to do this to make the random numbers correspond to ref output
    verbose_info(4, proc);
}


void
SimulationManager::run(){
    decode_args();
    read_randfile();
    read_inputfile();
    assign_scheduler();
    simulate();
    print_all();
}

void
SimulationManager::assign_scheduler(){
    
    string scheduler_type2 = scheduler_type;
    string schedulers[] = {"F", "L", "S", "R", "P"};
    
    transform(scheduler_type.begin(), scheduler_type.end(), scheduler_type.begin(), ::toupper);
    
    if(scheduler_type.compare(schedulers[0]) == 0){
        sched = new FCFSScheduler();
    } else if (scheduler_type.compare(schedulers[1]) == 0){
        sched = new LCFSScheduler();
    } else if (scheduler_type.compare(schedulers[2]) == 0){
        sched = new SJFScheduler();
    } else if (scheduler_type.substr(0, 1).compare(schedulers[3]) == 0){
        string quantum = scheduler_type.substr(1, scheduler_type.length());
        sched = new RRScheduler(atoi(quantum.c_str()));
    } else if (scheduler_type.substr(0, 1).compare(schedulers[4]) == 0){
        string quantum = scheduler_type.substr(1, scheduler_type.length());
        sched = new PRIOScheduler(atoi(quantum.c_str()));
    } else {
        printf("Error: %s is an unrecognized input for scheduler type", scheduler_type2.c_str());
        exit(EXIT_FAILURE);
    }
   
}

int
SimulationManager::get_next_rand(int burst){
    if(rand_offset <= num_rands){
        rand_offset++;
        return 1 + (rand_vals[rand_offset] % burst);
    } else {
        rand_offset = 0;
        return 1 + (rand_vals[rand_offset] % burst);
    }
}

void
SimulationManager::read_inputfile(){
    ifstream infile;
    infile.open(argv[argc-2]);
    string line;
    
    if(infile.is_open()){
        int proc_counter = 0;
        while( getline(infile, line)){
            std::istringstream iss(line);
            int AT, TCT, CB, IO;
            iss >> AT;
            iss >> TCT;
            iss >> CB;
            iss >> IO;
            
            Process *proc = new Process(AT, TCT, CB, IO, get_next_rand(4), proc_counter);
            
            Event* evt = new Event(proc, TRANS_TO_READY, proc->get_arrival_time());
            add_event(evt);
            
            proc_counter++;
        }
        infile.close();
        
    } else {
        cout << "Error: Input file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }
}

void
SimulationManager::read_randfile(){
    ifstream randfile;
    randfile.open(argv[argc-1]);
    
    if(randfile.is_open()){
        randfile >> num_rands;
        
        for(int i = 0; i < num_rands; i++){
            int cur_num;
            randfile >> cur_num;
            rand_vals.push_back(cur_num);
        }
        
        randfile.close();
    } else {
        cout << "Error: Random number file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }

}

int
SimulationManager::decode_args(){
    vflag = 0;
    int c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, "vs:")) != -1)
        switch (c)
    {
        case 'v':
            vflag = 1;
            break;
        case 's':
            scheduler_type = optarg;
            break;
        case '?':
            if (optopt == 's')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
    }
    return 0;
}

