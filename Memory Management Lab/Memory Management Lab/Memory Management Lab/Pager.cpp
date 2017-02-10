//
//  Pager.cpp
//  Memory Management Lab
//
//  Created by  Satya Nedunuri on 4/9/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include <stdio.h>
#include <limits>
#include "Pager.hpp"

int
NRU::allocate_frame(SimulationManager *sm) {
    
    vector<vector<int>> levels(4, vector<int>());
    
    for(int i = 0; i < sm->NUM_VIRTUAL_PAGES; i++) {
        SimulationManager::pte &entry = sm->page_table.at(i);
        
        if(entry.present){
            if(entry.referenced == 0 && entry.modified == 0){
                vector<int> &temp = levels[0];
                temp.push_back(entry.frame_index);
            }
            else if (entry.referenced == 0 && entry.modified == 1){
                vector<int> &temp = levels[1];
                temp.push_back(entry.frame_index);
            }
            else if (entry.referenced == 1 && entry.modified == 0){
                vector<int> &temp = levels[2];
                temp.push_back(entry.frame_index);
            }
            else{
                vector<int> &temp = levels[3];
                temp.push_back(entry.frame_index);
            }
        }
    }
    
    int counter = 0;
    int ret_val = -1;
    while(counter < 4){
        vector<int> &temp = levels[counter];
        if(temp.empty()){
            counter++;
        } else {
            int index = sm->get_next_rand((int) (temp.size()));
            ret_val = temp[index];
            break;
        }
    }
    
    page_replacement_counter++;
    if(page_replacement_counter == 10){
        for(int i = 0; i < sm->page_table.size(); i++){
            SimulationManager::pte &entry = sm->page_table.at(i);
            entry.referenced = 0;
        }
        page_replacement_counter = 0;
    }
    
    return ret_val;
}
    
string
NRU::print_page_queue(SimulationManager *sm){
    return "";
}

int
LRU::allocate_frame(SimulationManager *sm){
    int temp = frame_list.front();
    frame_list.erase(frame_list.begin());
    frame_list.push_back(temp);
    return temp;
}

void
LRU::update_frame_list(SimulationManager *sm){
    int page = sm->cur_instr.page_num;
    int temp = -1;
    for(int i = 0; i < frame_list.size(); i++){
        if(sm->frame_table[frame_list.at(i)] == page){
            temp = frame_list.at(i);
            frame_list.erase(frame_list.begin() + i);
            break;
        }
    }
    if(temp != -1){
        frame_list.push_back(temp);
    }
}

void
LRU::add_to_list(int i, SimulationManager *sm){
    if(frame_list.size() < sm->NUM_PHYSICAL_FRAMES)
        frame_list.push_back(i);
}

string
LRU::print_page_queue(SimulationManager *sm){
    return "";
}

int
Random::allocate_frame(SimulationManager *sm) {
    return sm->get_next_rand(sm->NUM_PHYSICAL_FRAMES);
}

string
Random::print_page_queue(SimulationManager *sm){ return ""; }

int
FIFO::allocate_frame(SimulationManager *sm) {
    int page = page_queue.front();
    page_queue.pop_front();
    return page;
}

void
FIFO::add_to_list(int i, SimulationManager *sm){
    page_queue.push_back(i);
}

string
FIFO::print_page_queue(SimulationManager *sm) {
    stringstream ss;
    ss << " " << "||";
    for(list<int>::iterator it = page_queue.begin(); it != page_queue.end(); ++it){
        ss << " " << *it;
    }
    return ss.str();
}
    
int
SecondChance::allocate_frame(SimulationManager *sm){
    bool frame_found = false;
    
    int ret_val = -1;
    while(!frame_found){
        int pte_index = sm->frame_table[page_queue.front()];
        SimulationManager::pte &entry = sm->page_table.at(pte_index);
        
        if(entry.referenced){
            entry.referenced = 0;
            int temp = page_queue.front();
            page_queue.erase(page_queue.begin());
            page_queue.push_back(temp);
        } else {
            frame_found = true;
            ret_val = entry.frame_index;
            page_queue.pop_front();
        }
    }
    
    return ret_val;
}

void
SecondChance::add_to_list(int i, SimulationManager *sm){
    page_queue.push_back(i);
}

string
SecondChance::print_page_queue(SimulationManager *sm) {
    stringstream ss;
    ss << " " << "||";
    for(list<int>::iterator it = page_queue.begin(); it != page_queue.end(); ++it){
        ss << " " << *it;
    }
    return ss.str();
}

int
PHYS_Clock::allocate_frame(SimulationManager *sm) {
    while(true){
        
        if(index >= sm->NUM_PHYSICAL_FRAMES)
            index = 0;
        
        int pte_index = sm->frame_table[index];
        SimulationManager::pte &entry = sm->page_table.at(pte_index);
        if(entry.referenced){
            entry.referenced = 0;
            index++;
        } else {
            index++;
            
            if(index >= sm->NUM_PHYSICAL_FRAMES)
                index = 0;
            
            return sm->page_table.at(pte_index).frame_index;
            
        }
        
    }
}

string
PHYS_Clock::print_page_queue(SimulationManager *sm){
    stringstream ss;
    ss << " " << "|| ";
    ss << "hand = " << index;
    return ss.str();
}



int
VIRT_Clock::allocate_frame(SimulationManager *sm) {
    while(true){
        
        if(index >= sm->NUM_VIRTUAL_PAGES)
            index = 0;
        
        SimulationManager::pte &entry = sm->page_table.at(index);
        if(entry.present){
            if(entry.referenced){
                entry.referenced = 0;
                index++;
            } else {
                index++;
                
                if(index >= sm->NUM_VIRTUAL_PAGES)
                    index = 0;
                
                return entry.frame_index;
            }
        } else {
            index++;
        }
    }
    
}

string
VIRT_Clock::print_page_queue(SimulationManager *sm){
    stringstream ss;
    ss << " " << "|| ";
    ss << "hand = " << index;
    return ss.str();
}

int
PHYS_Aging::allocate_frame(SimulationManager *sm) {
    
    for(int i = 0; i < sm->NUM_PHYSICAL_FRAMES; i++){
        unsigned int &vector = age_vectors.at(i);
        vector = vector >> 1;
        SimulationManager::pte &entry = sm->page_table.at(sm->frame_table[i]);
        vector = vector | (entry.referenced << 31);
        entry.referenced = 0;
    }
    
    unsigned int min = numeric_limits<unsigned int>::max();
    int index = -1;
    for(int i = 0; i < sm->NUM_PHYSICAL_FRAMES; i++){
        if(age_vectors[i] < min){
            min = age_vectors[i];
            index = i;
        }
    }
    return index;
}

void
PHYS_Aging::add_to_list(int i, SimulationManager *sm){
    age_vectors[i] = 0;
}

string
PHYS_Aging::print_page_queue(SimulationManager *sm){
    stringstream ss;
    ss << " " << "|| ";
    for(int i = 0; i < age_vectors.size(); i++){
        ss << i << ":";
        ss << hex << age_vectors[i] << " ";
    }
    return ss.str();
}

map<int, unsigned int>
VIRT_Aging::create_map(){
    map<int, unsigned int> age_vectors;
    for(int i = 0; i < 64; i++){
        age_vectors[i] = 0;
    }
    return age_vectors;
}

int
VIRT_Aging::allocate_frame(SimulationManager *sm) {
    for(int i = 0; i < sm->NUM_VIRTUAL_PAGES; i++){
        unsigned int &vector = age_vectors.at(i);
        SimulationManager::pte &entry = sm->page_table.at(i);
        if(entry.present){
            vector = vector >> 1;
            vector = vector | (entry.referenced << 31);
            entry.referenced = 0;
        } else {
            vector = 0;
        }
    }
    
    unsigned int min = numeric_limits<unsigned int>::max();
    int index = -1;
    for(int i = 0; i < sm->NUM_VIRTUAL_PAGES; i++){
        if(age_vectors[i] < min && sm->page_table.at(i).present){
            min = age_vectors[i];
            index = i;
        }
    }
    return sm->page_table.at(index).frame_index;
}

string
VIRT_Aging::print_page_queue(SimulationManager *sm){
    stringstream ss;
    ss << " " << "|| ";
    for(int i = 0; i < age_vectors.size(); i++){
        if(sm->page_table.at(i).present){
            ss << i << ":";
            ss << hex << age_vectors[i] << " ";
        } else {
            ss << "* ";
        }
    }
    return ss.str();
}
