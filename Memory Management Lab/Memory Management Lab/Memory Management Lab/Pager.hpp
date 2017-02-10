//
//  Pager.hpp
//  Memory Management Lab
//
//  Created by  Satya Nedunuri on 3/29/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef Pager_hpp
#define Pager_hpp

#include <stdio.h>
#include <sstream>
#include <iostream>
#include <map>
#include "SimulationManager.hpp"
//#include "SimulationManager.cpp"

class SimulationManager;

using namespace std;

class Pager {
public:
    virtual int allocate_frame(SimulationManager *sm) = 0;
    virtual void add_to_list(int i, SimulationManager *sm){};
    virtual string print_page_queue(SimulationManager *sm) = 0;
    virtual void update_frame_list(SimulationManager *sm) {};
    
};

class NRU: public Pager {
    
    int page_replacement_counter = 0;
    
    int allocate_frame(SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);
};


class LRU: public Pager {

    vector<int> frame_list;
    
    int allocate_frame(SimulationManager *sm);
    
    void update_frame_list(SimulationManager *sm);
    
    void add_to_list(int i, SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);
};

class Random: public Pager {
    int allocate_frame(SimulationManager *sm);
    string print_page_queue(SimulationManager *sm);
};

class FIFO: public Pager {
    list<int> page_queue;
    
    int allocate_frame(SimulationManager *sm);
    
    void add_to_list(int i, SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);

};

class SecondChance: public Pager {
    list<int> page_queue;
    
    int allocate_frame(SimulationManager *sm);
    
    void add_to_list(int i, SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);
};

class PHYS_Clock: public Pager {
    int index = 0;
    
    int allocate_frame(SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);
};

class VIRT_Clock: public Pager {
    int index = 0;
    
    int allocate_frame(SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);
};

class PHYS_Aging: public Pager {
    
    map<int, unsigned int> age_vectors;
    
    int allocate_frame(SimulationManager *sm);
    
    void add_to_list(int i, SimulationManager *sm);
    
    string print_page_queue(SimulationManager *sm);
};

class VIRT_Aging: public Pager {
    
    map<int, unsigned int> age_vectors = create_map();
    
    map<int, unsigned int> create_map();
    
    int allocate_frame(SimulationManager *sm);

    string print_page_queue(SimulationManager *sm);
};

#endif /* Pager_hpp */
