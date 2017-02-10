//
//  Scheduler.hpp
//  OS Scheduler Lab
//
//  Created by  Satya Nedunuri on 3/7/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef Scheduler_hpp
#define Scheduler_hpp

#include <stdio.h>
#include "Event.hpp"
#include <string>

using namespace std;

class Scheduler {
  
public:
    Scheduler(string sched_type, int quantum){
        type = sched_type;
        if (quantum != -1){
            type.append(" ");
            type.append(to_string(quantum));
        }
    }
    
    ~Scheduler(){}
    
    //methods that concrete classes need to implement
    virtual void add_event(Event* e) = 0;
    virtual Event* get_next_event() = 0;
    virtual Event* peek_next_event() = 0;
    virtual bool empty() = 0;
    virtual int get_quantum() = 0;
    
    //method that this class implements
    void
    print_scheduler_info(){
        printf("%s\n", type.c_str());
    }
    
private:
    string type;
    
};

#endif /* Scheduler_hpp */
