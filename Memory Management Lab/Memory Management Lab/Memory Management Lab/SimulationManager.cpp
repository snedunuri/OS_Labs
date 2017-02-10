//
//  SimulationManager.cpp
//  Memory Management Lab
//
//  Created by  Satya Nedunuri on 3/28/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <list>
#include "SimulationManager.hpp"
#include "Pager.hpp"

using namespace std;


void
SimulationManager::simulate(){
    
    //initialize free list
    for(int i = 0; i < NUM_PHYSICAL_FRAMES; i++)
        free_list.push_back(i);
    
    //initialize page table
    for(int i = 0; i < NUM_VIRTUAL_PAGES; i++){
        page_table.push_back({0,0,0,0,0});
    }
    
    //initialize frame table
    for(int i = 0; i < NUM_VIRTUAL_PAGES; i++)
        frame_table[i] = -1;
    
    
    //loop until all instructions are processed
    while(!instr_list.empty()){
        cur_instr = get_next_instruction();
        pte &entry = page_table.at(cur_instr.page_num);
        
        //checks for output opt flag
        print_instr_info(cur_instr);
        
        pager->update_frame_list(this);
        if(!entry.present){ //virtual page not in frame table
            int frame = get_frame();
            if(entry.paged_out){
                page_in(frame);
            } else {
                zero(frame);
            }
            map(entry, frame);
            pager->add_to_list(entry.frame_index, this);
        }
        update_pte(entry);

        if(page_table_per_instr)
            print_page_table_state();
        
        if(frame_table_per_instr)
            print_frame_table_state(1);
        
        stats.instr_count++;
    }
    
}

void
SimulationManager::update_pte(SimulationManager::pte &entry){
    
    if(cur_instr.operation){ //write operation
        entry.modified = 1;
    }
    
    //******************************
    //entry is referenced for both read and write operations
    entry.referenced = 1;
}

void
SimulationManager::map(SimulationManager::pte &entry, int frame){
    if(output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "MAP", cur_instr.page_num, frame);
    
    stats.maps++;
    
    //virtual page occupies physical frame
    entry.present = 1;
    
    //add index of frame to pte
    entry.frame_index = frame;
    
    //index of pte in page table
    int index = (int) (&entry - &page_table[0]);
    
    frame_table[frame] = index;
    
}

void
SimulationManager::print_instr_info(Instruction instr){
    if(output_opt)
        printf("==> inst: %1d %d\n", instr.operation, instr.page_num);
}


void
SimulationManager::page_in(int frame){
   
    pte &entry = page_table.at(frame_table[frame]);
    
    if(output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "UNMAP", frame_table[frame], frame);
    
    //increment unmap counters
    stats.unmaps++;
    
    if (entry.modified) {
        if (output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "OUT", frame_table[frame], frame);
        
        //entry is modified so it is swapped to disk
        entry.paged_out = 1;
        
        //flip the modified bit
        entry.modified = 0;
        
        stats.outs++;
    }
    
    if (output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "IN",  cur_instr.page_num, frame);
    
    //entry is no longer in the frame table
    entry.present = 0;
    
    //entry is no longer referenced
    entry.referenced = 0;
    
    //increment in counters
    stats.ins++;
}

void
SimulationManager::zero(int frame){
    int index = frame_table[frame];
    
    if(index != -1){ //index == -1 when a page was never mapped to that frame
        pte &entry = page_table.at(index);
        if(entry.present){
            if(entry.modified){
                if(output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "UNMAP", frame_table[frame], frame);
                if(output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "OUT", frame_table[frame], frame);
                
                //set modified bit to false after page is swapped to disk
                entry.modified = 0;
                
                //set paged out bit to true since page is on disk
                entry.paged_out = 1;
                
                //increment stats for outs
                stats.outs++;
            } else {
                if(output_opt) printf("%d: %-7s%2d  %2d\n", stats.instr_count, "UNMAP", frame_table[frame], frame);
            }
            
            
            //entry is no longer in the frame table
            entry.present = 0;
            
            stats.unmaps++;
        }
    }
    
    //frame zeroed
    if (output_opt) printf("%d: %-7s    %2d\n", stats.instr_count, "ZERO", frame);
    
    stats.zeros++;
}

void
SimulationManager::print_all(){
    if (pagetable_opt)
        print_page_table_state();
    if(frametable_opt)
        print_frame_table_state(0);
    if (summary_opt)
        print_summary_stats();
}

void
SimulationManager::print_frame_table_state(int i){
    stringstream ss;
    for(int i = 0; i < NUM_PHYSICAL_FRAMES; i++){
        (frame_table[i] == -1) ? ss << "*" : ss << frame_table[i];
        ss << " ";
    }
    
    if(i){ //FIFO, SC pager
        ss << pager->print_page_queue(this);
    }
    cout << ss.str() << endl;
}

void
SimulationManager::print_summary_stats(){
    //compute the total cost in cpu cycles
    stats.total_cost = 0;
    stats.total_cost += stats.instr_count;
    stats.total_cost += stats.unmaps * MAP_UNMAP_CYCLES;
    stats.total_cost += stats.maps * MAP_UNMAP_CYCLES;
    stats.total_cost += stats.ins * PAGE_IN_OUT_CYCLES;
    stats.total_cost += stats.outs * PAGE_IN_OUT_CYCLES;
    stats.total_cost += stats.zeros * ZERO_CYCLES;
    
    printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
           stats.instr_count, stats.unmaps, stats.maps, stats.ins, stats.outs, stats.zeros, stats.total_cost);
}

void
SimulationManager::print_page_table_state(){
    stringstream os;
    for(int i = 0; i < page_table.size(); i++){
        pte &entry = page_table[i];
        
        //entry was modified so it was swapped to disk
        if(entry.paged_out && !entry.present){
            os << "# ";
            continue;
        }
        
        //entry not swapped because it was not modified
        
        if(!entry.modified && !entry.present){
            os << "* ";
            continue;
        }
        
        os << i << ":";
        
        (entry.referenced) ? os << "R" : os << "-";
        (entry.modified) ? os << "M" : os << "-";
        (entry.paged_out) ? os << "S" : os << "-";
        
        os << " ";
    }
    cout << os.str() << endl;
}

SimulationManager::Instruction
SimulationManager::get_next_instruction(){
    Instruction temp = instr_list.front();
    instr_list.erase(instr_list.begin());
    return temp;
}

int
SimulationManager::get_frame(){
    int frame = allocate_frame_from_free_list();
    if(frame == -1)
        frame = pager->allocate_frame(this);
    return frame;
}

int
SimulationManager::allocate_frame_from_free_list(){
    if(free_list.empty())
        return -1;
    else{
        int temp = free_list.front();
        free_list.erase(free_list.begin());
        return temp;
    }
}

void
SimulationManager::run(){
    get_args();
    decode_args();
    read_inputfile();
    read_randfile();
    simulate();
    print_all();
}

int
SimulationManager::get_args(){
    aflag = 0;
    oflag = 0;
    fflag = 0;
    avalue = NULL;
    ovalue = NULL;
    fvalue = NULL;
    //int index;
    int c;
    
    opterr = 0;
    while ((c = getopt (argc, argv, "a:o:f:")) != -1)
        switch (c)
    {
        case 'a':
            aflag = 1;
            avalue = optarg;
            break;
        case 'o':
            oflag = 1;
            ovalue = optarg;
            break;
        case 'f':
            fflag = 1;
            fvalue = optarg;
            break;
        case '?':
            if (optopt == 'a' || optopt == 'o' || optopt == 'f' )
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort();
    }
    
    return 0;
    
}

void
SimulationManager::decode_args(){
    
    //decode the algo type
    if(aflag){
        switch(avalue[0]){
                case 'N':
                    pager = new NRU();
                    break;
                case 'l':
                    pager = new LRU();
                    break;
                case 'r':
                    pager = new Random();
                    break;
                case 'f':
                    pager = new FIFO();
                    break;
                case 's':
                    pager = new SecondChance();
                    break;
                case 'c':
                    pager = new PHYS_Clock();
                    break;
                case 'X':
                    pager = new VIRT_Clock();
                    break;
                case 'a':
                    pager = new PHYS_Aging();
                    break;
                case 'Y':
                    pager = new VIRT_Aging();
                    break;
        }
    } else {
        aflag = 1;
        pager = new LRU();
    }

    //decode the output args
    if(oflag){
        int counter = 0;
        while(ovalue[counter] != '\0'){
            switch(ovalue[counter]){
                    case 'O':
                        output_opt = 1;
                        break;
                    case 'P':
                        pagetable_opt = 1;
                        break;
                    case 'F':
                        frametable_opt = 1;
                        break;
                    case 'S':
                        summary_opt = 1;
                        break;
                    case 'p':
                        page_table_per_instr = 1;
                        break;
                    case 'f':
                        frame_table_per_instr = 1;
                        break;
                    case 'a':
                        aging_per_instr = 1;
                        break;
            }
            counter++;
        }
    }
    
    //set the number of physical frames
    if(fflag){
        NUM_PHYSICAL_FRAMES = atoi(fvalue);
    } else {
        fflag = 1;
        NUM_PHYSICAL_FRAMES = 32;
    }
}

void
SimulationManager::read_inputfile(){
    ifstream infile;
    infile.open(argv[argc-2]);
    string line;
    
    if(infile.is_open()){
        while( getline(infile, line)){
            if(line.substr(0,1) == "#")
                continue;
            else{
                stringstream ssin(line);
                string instr[2];
                
                ssin >> instr[0]; //operation
                ssin >> instr[1]; //page number
                
                int operation = stoi(instr[0]);
                int page_num = stoi(instr[1]);
                
                if((operation == 0 || operation == 1) && page_num < NUM_VIRTUAL_PAGES)
                    instr_list.push_back(Instruction{operation, page_num});
                else
                    continue; //invalid instructions are skipped
            }
        }
        infile.close();
    } else {
        cout << "Error: Input file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }
}


void
SimulationManager::read_randfile(){
    ifstream randfile;
    randfile.open(argv[argc-1]);
    
    if(randfile.is_open()){
        randfile >> num_rands;
        
        int cur_num;
        for(int i = 0; i < num_rands; i++){
            randfile >> cur_num;
            rand_nums.push_back(cur_num);
        }
        
        randfile.close();
    } else {
        cout << "Error: Random number file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }
    
}

int
SimulationManager::get_next_rand(int burst)
{
    int idx = (rand_nums[rand_offset] % burst);
    rand_offset++;
    if(rand_offset >= num_rands)
        rand_offset = 0;
    return idx;
}
