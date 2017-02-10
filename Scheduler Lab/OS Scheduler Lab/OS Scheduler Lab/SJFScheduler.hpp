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
        if(!runq.empty()){
            Event* evt = runq.front();
            runq.erase(runq.begin());
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
        list<Event*>::iterator it = runq.begin();
        for(int i = 0; i < runq.size(); i++){
            Event* ptr = *it;
            if(e->get_timestamp() >= ptr->get_timestamp()){
                it++;
            } else {
                break;
            }
        }
        runq.insert(it, e);

    }
    
    bool empty() { return runq.empty(); }
    
    int get_quantum() { return -1; }
    
private:
    list<Event*> runq;
};



#endif /* SJFScheduler_h */
