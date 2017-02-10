//
//  Process.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/6/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef Process_hpp
#define Process_hpp

#include <stdio.h>

enum State { READY, PREEMPT, BLOCK, RUN, CREATE };

class Process {

public:
    Process(int ar, int tct, int cb_temp, int ib_temp, int sp, int proc_id){
        arrival_time = ar;
        total_cpu_time = tct;
        cpu_burst = 0;
        io_burst = 0;
        static_priority = sp;
        dynamic_priority = static_priority - 1;
        pid = proc_id;
        remaining_exec_time = total_cpu_time;
        io_time = 0;
        cpu_wait_time = 0;
        CB = cb_temp;
        IO = ib_temp;
        time_in_prev_state = 0;
        state_ts = arrival_time;
        cur_state = CREATE;
        time_left_to_run = 0;
    }
    
    ~Process(){}
    
    void
    print_process_info(){
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", pid,
               arrival_time,
               total_cpu_time,
               CB,
               IO,
               static_priority,
               finishing_time,
               turnaround_time,
               io_time,
               cpu_wait_time);
    }
    
    //getter methods
    int get_arrival_time() { return arrival_time; }
    int get_total_cpu_time() { return total_cpu_time; }
    int get_cpu_burst() { return cpu_burst; }
    int get_remaining_exec_time() { return remaining_exec_time; }
    int get_io_burst() { return io_burst; }
    int get_io_time() { return io_time; }
    int get_CB() { return CB; }
    int get_IO() { return IO; }
    int get_pid() { return pid; }
    int get_time_in_prev_state() { return time_in_prev_state; }
    int get_static_prio() { return static_priority; }
    int get_dynamic_prio() { return dynamic_priority; }
    int get_turnaround() { return turnaround_time; }
    int get_cpu_wait_time() { return cpu_wait_time; }
    int get_state_ts() { return state_ts; }
    int get_time_left_to_run() { return time_left_to_run; }
    State get_state() { return cur_state; }
    
    //setter methods
    void set_cpu_burst(int burst) { cpu_burst = burst; time_left_to_run = burst; }
    void set_remaining_exec_time(int t) { remaining_exec_time = t; }
    void set_finishing_time(int t) { finishing_time = t; }
    void set_turnaround_time(int t) { turnaround_time = t; }
    void set_io_time(int t) { io_time = t; }
    void set_cpu_wait_time(int t) { cpu_wait_time = t; }
    void set_io_burst(int t) { io_burst = t; }
    void set_static_prio(int t) { static_priority = t; }
    void set_time_in_prev_state(int t) { time_in_prev_state = t; }
    void set_state_ts(int t) { state_ts = t; }
    void set_state(State s) { cur_state = s; }
    void set_time_left_to_run(int t) { time_left_to_run = t; }
    void set_dynamic_prio(int i) { dynamic_priority = i; }
    
private:
    int arrival_time;
    int total_cpu_time;
    int cpu_burst;
    int io_burst;
    int static_priority;
    int dynamic_priority;
    int pid;
    int finishing_time;
    int turnaround_time;
    int io_time;
    int cpu_wait_time;
    int remaining_exec_time;
    int CB;
    int IO;
    int time_in_prev_state;
    int state_ts;
    int time_left_to_run;
    State cur_state;
    
};

struct Interval {
    int start_time;
    int end_time;
    int pid;
};

struct OrderIntervals{
    bool operator () (Interval inter1, Interval inter2){
        return inter1.start_time > inter2.start_time;
    }
};

#endif /* Process_hpp */
