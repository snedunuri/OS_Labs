//
//  Scheduler.hpp
//  IO Lab
//
//  Created by  Satya Nedunuri on 4/16/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef Scheduler_hpp
#define Scheduler_hpp

#include <stdio.h>
#include <queue>
#include "SimulationManager.hpp"
#include "Operation.h"

using namespace std;

class Scheduler {
public:
    virtual void add(Operation op) = 0;
    virtual Operation get(unsigned int cur_track) = 0;
    virtual bool isEmpty() = 0;
};

class FIFO : public Scheduler {
    list<Operation> op_queue;
    void add(Operation op);
    Operation get(unsigned int cur_track);
    bool isEmpty();
};

class SSTF : public Scheduler {
    list<Operation> op_queue;
    void add(Operation op);
    Operation get(unsigned int cur_track);
    bool isEmpty();
};


class SCAN : public Scheduler {
    list<Operation> up;
    list<Operation> down;
    int current_track = -1;
    int direction = 1;
    
    void add(Operation op);
    Operation get(unsigned int cur_track);
    bool isEmpty();
    void enqueue(Operation op, int dir);
};

class CSCAN : public Scheduler {
    list<Operation> ops;
    list<Operation>::iterator iter;
    int current_track = -1;
    
    void add(Operation op);
    Operation get(unsigned int cur_track);
    bool isEmpty();
};

class FSCAN : public Scheduler {
    vector<Operation> runq;
    vector<Operation> waitq;
    int direction = 1;
    
    void add(Operation op);
    Operation get(unsigned int cur_track);
    bool isEmpty();
};

#endif /* Scheduler_hpp */
