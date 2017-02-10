//
//  SJFScheduler.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/11/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef SJFScheduler_h
#define SJFScheduler_h

#include <stdio.h>
#include <queue>
#include <vector>
#include "Scheduler.hpp"
#include "Event.hpp"

class SJFScheduler : public Scheduler {
    
public:
    
    SJFScheduler(): Scheduler("SJF", -1){
        
    }
    
    ~SJFScheduler(){}
    
    Event* get_next_event() {
        stable_sort(runq.begin(), runq.end(), SJFOrder());
        if(!runq.empty()){
            Event* evt = runq.front();
            runq.erase(runq.begin());
            return evt;
        } else {
            return nullptr;
        }
    }
    
    Event* peek_next_event() {
        stable_sort(runq.begin(), runq.end(), SJFOrder());
        if(!runq.empty()){
            return runq.front();
        } else {
            return nullptr;
        }
    }
    
    void add_event(Event* e) {
        if(e->get_process()->get_dynamic_prio() == -1){
            e->get_process()->set_dynamic_prio(e->get_process()->get_static_prio() - 1 );
        }
        runq.push_back(e);
    }
    
    bool empty() { return runq.empty(); }
    
    int get_quantum() { return numeric_limits<int>::max();}
    
private:
    vector<Event*> runq;
};



#endif /* SJFScheduler_h */
