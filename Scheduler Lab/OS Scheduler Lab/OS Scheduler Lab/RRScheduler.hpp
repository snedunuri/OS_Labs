//
//  RRScheduler.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/7/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef RRScheduler_hpp
#define RRScheduler_hpp

#include <stdio.h>
#include "Scheduler.hpp"
#include "Event.hpp"

class RRScheduler : public Scheduler {
    
public:
    
    RRScheduler(int q): Scheduler("RR", q){
        quantum = q;
    }
    
    ~RRScheduler(){}
    
    int get_quantum() { return quantum; }

    Event* get_next_event() {
        if (!runq.empty()){
            Event* evt = runq.front();
            runq.erase(runq.begin());
            return evt;
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
    
    Event* peek_next_event() {
        if(!runq.empty()){
            return runq.front();
        } else {
            return nullptr;
        }
    }

    bool empty() { return runq.empty(); }
    
private:
    vector<Event*> runq;
    int quantum;
};


#endif /* RRScheduler_hpp */
