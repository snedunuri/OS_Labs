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
            Event* evt = activeq.front();
            activeq.erase(activeq.begin());
            return evt;
        } else {
            
            //swap active and expired queues
            swap(activeq, expiredq);
           
            if(!activeq.empty()){
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
            return activeq.front();
        } else {
            
            //swap active and expired queues
            swap(activeq, expiredq);
            
            if(!activeq.empty()){
                return activeq.front();
            } else {
                return nullptr;
            }
        }
    }
    
    void add_event(Event *e){
        if(e->get_process()->get_dynamic_prio() == -1){
            e->get_process()->set_dynamic_prio(e->get_process()->get_static_prio() - 1 );
            list<Event*>::iterator it = expiredq.begin();
            Event* ptr = *it;
            for(int i = 0; i < expiredq.size(); i++){
                if(e->get_timestamp() > ptr->get_timestamp()){
                    it++;
                    ptr = *it;
                } else if (e->get_timestamp() == ptr->get_timestamp()){
                    int time = e->get_timestamp();
                    if(e->get_process()->get_dynamic_prio() > ptr->get_process()->get_dynamic_prio())
                        break;
                    else {
                        while(it != activeq.end() && ptr->get_process()->get_dynamic_prio() >= e->get_process()->get_dynamic_prio() && ptr->get_timestamp() == time){
                            it++;
                            ptr = *it;
                        }
                        break;
                    }
                } else {
                    break;
                }
            }
            expiredq.insert(it, e);
        } else {
            list<Event*>::iterator it = activeq.begin();
            Event* ptr = *it;
            for(int i = 0; i < activeq.size(); i++){
                if(e->get_timestamp() > ptr->get_timestamp()){
                    it++;
                    ptr = *it;
                } else if (e->get_timestamp() == ptr->get_timestamp()){
                    int time = e->get_timestamp();
                    if(e->get_process()->get_dynamic_prio() > ptr->get_process()->get_dynamic_prio())
                        break;
                    else {
                        while(it != activeq.end() && ptr->get_process()->get_dynamic_prio() >= e->get_process()->get_dynamic_prio() && ptr->get_timestamp() == time){
                            it++;
                            ptr = *it;
                        }
                        break;
                    }
                } else {
                    break;
                }
            }
            activeq.insert(it, e);
        }
        
    }

    bool empty() { return activeq.empty() && expiredq.empty(); }
    
private:
    list<Event*> activeq;
    list<Event*> expiredq;
    int quantum;
    
};

#endif /* PRIOScheduler_hpp */
