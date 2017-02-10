//
//  FCFSScheduler.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/7/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef FCFSScheduler_hpp
#define FCFSScheduler_hpp

#include <stdio.h>
#include <queue>
#include "Scheduler.hpp"
#include "Event.hpp"


class FCFSScheduler : public Scheduler {
    
public:
    
    FCFSScheduler(): Scheduler("FCFS", -1){}
    
    ~FCFSScheduler(){}
    
    Event* get_next_event() {
        if(!runq.empty()){
            Event* evt = runq.front();
            runq.pop();
            return evt;
        } else {
            return nullptr;
        }
    }
    
    Event* peek_next_event() {
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
        runq.push(e);
    }
    
    bool empty() { return runq.empty(); }
    
    int get_quantum() { return numeric_limits<int>::max(); }
    
private:
    queue<Event*> runq;
};

#endif /* FCFSScheduler_hpp */
