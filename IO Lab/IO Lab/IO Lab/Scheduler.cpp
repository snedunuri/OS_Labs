//
//  Scheduler.cpp
//  IO Lab
//
//  Created by  Satya Nedunuri on 4/16/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include "Scheduler.hpp"
#include <stdlib.h>
#include <limits>

void
FIFO::add(Operation op){
    op_queue.push_back(op);
}

Operation
FIFO::get(unsigned int cur_track){
    Operation temp = op_queue.front();
    op_queue.erase(op_queue.begin());
    return temp;
}

bool
FIFO::isEmpty(){
    return op_queue.empty();
}

void
SSTF::add(Operation op){
    op_queue.push_back(op);
}

Operation
SSTF::get(unsigned int cur_track){
    
    //iterate through entire list to find closest track number
    int index = -1;
    unsigned int min = numeric_limits<unsigned int>::max();
    list<Operation>::iterator it = op_queue.begin();
    for(int i = 0; i < op_queue.size(); i++){
        unsigned int diff = abs((int)(cur_track - it->track));
        if (diff < min){
            min = diff;
            index = i;
        }
        it++;
    }
    
    //get iterator back to position with lowest track seek
    it = op_queue.begin();
    for(int i = 0; i < index; i++)
        it++;
    
    //return operation
    Operation temp = *it;
    op_queue.erase(it);
    return temp;
}

bool
SSTF::isEmpty(){
    return op_queue.empty();
}

void
SCAN::enqueue(Operation op, int dir){
    
    list<Operation>::iterator it;
    if(dir == 1){
        it = up.begin();
         while(it != up.end() && it->track <= op.track)
             it++;
        up.insert(it, op);
    }
    else{
        it = down.begin();
        while(it != down.end() && it->track <= op.track)
            it++;
        down.insert(it, op);
    }
   
}

void
SCAN::add(Operation op){
    if(current_track == -1){
        current_track = op.track;
        enqueue(op, 1);
        return;
    }
    
    if(op.track >= current_track){
        enqueue(op, 1);
    } else if (op.track < current_track){
        enqueue(op, -1);
    }
    
}

Operation
SCAN::get(unsigned int cur_track){
    if(down.empty())
        direction = 1;
    
    if(up.empty())
        direction = -1;
    
    if(direction == -1){
        Operation temp = down.back();
        down.pop_back();
        current_track = temp.track;
        return temp;
    } else {
        Operation temp = up.front();
        up.pop_front();
        current_track = temp.track;
        return temp;
    }
}

bool
SCAN::isEmpty(){
    return up.empty() && down.empty();
}

void
CSCAN::add(Operation op){
    list<Operation>::iterator it = ops.begin();
    
    while (it != ops.end() && it->track <= op.track)
        it++;
    ops.insert(it, op);
}

Operation
CSCAN::get(unsigned int cur_track){
    iter = ops.begin();
    while(iter != ops.end() && iter->track < cur_track)
        iter++;
    
    if(iter == ops.end())
        iter = ops.begin();
    
    Operation temp = *iter;
    ops.erase(iter);
    return temp;
}

bool
CSCAN::isEmpty(){
    return ops.empty();
}

void
FSCAN::add(Operation op){
    waitq.push_back(op);
}

Operation
FSCAN::get(unsigned int cur_track){
    
    if(runq.empty()){
        swap(runq, waitq);
        direction = 1;
    }
    
    unsigned int min = numeric_limits<unsigned int>::max();
    int index = -1;
    Operation temp;
    if (direction == 1){
        for(int i = 0; i < runq.size(); i++){
            int dist = abs((int) (cur_track - runq[i].track));
            if(runq[i].track >= cur_track && dist < min){
                min = dist;
                index = i;
            }
        }
        
        //search in the other direction
        if (index == -1){
            direction = -1;
            
            for(int i = 0; i < runq.size(); i++){
                int dist = abs((int) (cur_track - runq[i].track));
                if(runq[i].track <= cur_track && dist < min){
                    min = dist;
                    index = i;
                }
            }
            
        }
        
    } else {
        
        for(int i = 0; i < runq.size(); i++){
            int dist = abs((int) (cur_track - runq[i].track));
            if(runq[i].track <= cur_track && dist < min){
                min = dist;
                index = i;
            }
        }
        
        //search in the other direction
        if (index == -1){
            direction = 1;
            
            for(int i = 0; i < runq.size(); i++){
                int dist = abs((int) (cur_track - runq[i].track));
                if(runq[i].track >= cur_track && dist < min){
                    min = dist;
                    index = i;
                }
            }
        }

    }

    
    temp = runq[index];
    runq.erase(runq.begin() + index);
    return temp;
    
}

bool
FSCAN::isEmpty(){
    return runq.empty() && waitq.empty();
}