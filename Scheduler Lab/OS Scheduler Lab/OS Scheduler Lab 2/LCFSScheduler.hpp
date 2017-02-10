//
//  LCFSScheduler.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/7/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef LCFSScheduler_hpp
#define LCFSScheduler_hpp

#include <stdio.h>
#include <stack>
#include "Scheduler.hpp"
#include "Event.hpp"

class LCFSScheduler : public Scheduler {
    
public:
    
    LCFSScheduler(): Scheduler("LCFS", -1){}
    
    ~LCFSScheduler(){}
    
    Event* get_next_event() {
        if(!runq.empty()){
            Event* evt = runq.top();
            runq.pop();
            return evt;
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
    
    Event* peek_next_event() {
        if(!runq.empty()){
            return runq.top();
        } else {
            return nullptr;
        }
    }
    
    bool empty() { return runq.empty(); }
    
    int get_quantum() { return numeric_limits<int>::max(); }
    
private:
    stack<Event*> runq;
    
};

#endif /* LCFSScheduler_hpp */
