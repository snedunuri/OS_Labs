//
//  Event.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/6/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef Event_hpp
#define Event_hpp

#include <stdio.h>
#include "Process.hpp"

enum Transition { TRANS_TO_READY, TRANS_TO_PREEMPT, TRANS_TO_BLOCK, TRANS_TO_RUN };

class Event {
    
public:
    Event (Process *pr, Transition trans, int ts){
        proc = pr;
        transition = trans;
        timestamp = ts;
    }
    
    ~Event(){}
    
    Transition get_state() { return transition; }
    int get_timestamp() { return timestamp; }
    Process* get_process() { return proc; }
    
    void set_timestamp(int t) { timestamp = t; }
    void set_transition(Transition t) { transition = t; }
    
    
private:
    int timestamp;
    Transition transition;
    Process *proc;
    
    
};

struct SJFOrder {
    bool operator() (Event* lhs, Event* rhs){
        int lhs_time_left = lhs->get_process()->get_remaining_exec_time();
        int rhs_time_left = rhs->get_process()->get_remaining_exec_time();
        return lhs_time_left < rhs_time_left;
    }
};

struct OrderEvents {
    bool operator () (Event* lhs, Event* rhs){
        return lhs->get_timestamp() < rhs->get_timestamp();
    }
};

struct OrderPRIO {
    bool operator () (Event* lhs, Event* rhs){
        return lhs->get_process()->get_dynamic_prio() > rhs->get_process()->get_dynamic_prio();
    }
};

#endif /* Event_hpp */
