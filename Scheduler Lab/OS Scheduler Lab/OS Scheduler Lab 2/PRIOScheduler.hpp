//
//  PRIOScheduler.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/7/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef PRIOScheduler_hpp
#define PRIOScheduler_hpp

#include <stdio.h>
#include <queue>
#include "Scheduler.hpp"
#include "Event.hpp"

class PRIOScheduler : public Scheduler {
    
public:
    
    PRIOScheduler(int q): Scheduler("PRIO", q){
        quantum = q;
    }
    
    ~PRIOScheduler(){}
    
    int get_quantum() { return quantum; }
    
    Event* get_next_event() {
        if(!activeq.empty()){
            stable_sort(activeq.begin(), activeq.end(), OrderPRIO());
            Event* evt = activeq.front();
            activeq.erase(activeq.begin());
            return evt;
        } else {
            
            //swap active and expired queues
            swap(activeq, expiredq);
           
            if(!activeq.empty()){
                stable_sort(activeq.begin(), activeq.end(), OrderPRIO());
                Event* evt = activeq.front();
                activeq.erase(activeq.begin());
                return evt;
            } else {
                return nullptr;
            }
        }
    }
   
    
    Event* peek_next_event() {
        if(!activeq.empty()){
            stable_sort(activeq.begin(), activeq.end(), OrderPRIO());
            return activeq.front();
        } else {
            
            //swap active and expired queues
            swap(activeq, expiredq);
            
            if(!activeq.empty()){
                stable_sort(activeq.begin(), activeq.end(), OrderPRIO());
                return activeq.front();
            } else {
                return nullptr;
            }
        }
    }
    
    void add_event(Event* e) {
        if(e->get_process()->get_dynamic_prio() == -1){
            e->get_process()->set_dynamic_prio(e->get_process()->get_static_prio() - 1 );
            expiredq.push_back(e);
        }else {
             activeq.push_back(e);
        }
    }
    
    bool empty() { return activeq.empty() && expiredq.empty(); } //might have to to watch this for while loop
    
private:
    vector<Event*> activeq;
    vector<Event*> expiredq;
    int quantum;
};

#endif /* PRIOScheduler_hpp */
