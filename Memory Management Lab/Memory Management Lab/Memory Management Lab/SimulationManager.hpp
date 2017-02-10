//
//  SimulationManager.hpp
//  Memory Management Lab
//
//  Created by  Satya Nedunuri on 3/28/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef SimulationManager_hpp
#define SimulationManager_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <list>
#include "Pager.hpp"

using namespace std;

class Pager;

class SimulationManager {
    
public:
    SimulationManager(int argcount, char* argArr[]){
        argc = argcount;
        argv = argArr;
        rand_offset = 0;
        num_rands = 0;
        output_opt = 0;
        pagetable_opt = 0;
        frametable_opt = 0;
        summary_opt = 0;
        page_table_per_instr = 0;
        frame_table_per_instr = 0;
        aging_per_instr = 0;
        stats = {0,0,0,0,0};
        oflag = 0;
    }
    
    void run();
    
    //used for random algo
    int NUM_PHYSICAL_FRAMES;
    int get_next_rand(int burst);
    
    //bit struct for page table entry
    struct pte {
        unsigned int present:1;
        unsigned int modified:1;
        unsigned int referenced:1;
        unsigned int paged_out:1;
        unsigned int frame_index:6;
    };
    
    //needed for NRU algo
    static const int NUM_VIRTUAL_PAGES = 64;
    int rand_offset;
    vector<int> rand_nums;
    
    //page table is public for Second Chance algo
    vector<pte> page_table;
    int frame_table[NUM_VIRTUAL_PAGES]; //max size needed
    list<int> free_list;

    
    //current instruction being processed
    struct Instruction {
        int operation;
        int page_num;
    };
    
    Instruction cur_instr;
    list<Instruction> instr_list;
    
    struct stats {
        unsigned int instr_count;
        unsigned int unmaps;
        unsigned int maps;
        unsigned int ins;
        unsigned int outs;
        unsigned int zeros;
        unsigned long long int total_cost;
    };
    
    //stats struct
    stats stats;

private:
    
    //command line args and options
    int argc;
    int aflag;
    int oflag;
    int fflag;
    
    //random vals
    int num_rands;
    
    //option flags
    int output_opt;
    int pagetable_opt;
    int frametable_opt;
    int summary_opt;
    int page_table_per_instr;
    int frame_table_per_instr;
    int aging_per_instr;
    
    //constants for cycle count
    static const int MAP_UNMAP_CYCLES = 400;
    static const int PAGE_IN_OUT_CYCLES = 3000;
    static const int ZERO_CYCLES = 150;
    static const int READ_WRITE_CYCLES = 1;
    
    
    //option arguments
    char **argv;
    char *avalue;
    char *ovalue;
    char *fvalue;
      
    //algorithm used to allocate frames
    Pager *pager;
    
    //private helper functions
    int get_args();
    void decode_args();
    void read_inputfile();
    void read_randfile();
    
    void simulate();
    int get_frame();
    int allocate_frame_from_free_list();
    Instruction get_next_instruction();
    void page_in(int frame);
    void zero(int frame);
    void map(pte &entry, int frame);
    void update_pte(pte &entry);
    void print_instr_info(Instruction instr);
    void print_summary_stats();
    void print_page_table_state();
    void print_frame_table_state(int i);
    void print_all();
    
};


#endif /* SimulationManager_hpp */
