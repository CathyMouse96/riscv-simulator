//
//  MulticycleSimulator.hpp
//  RISCV-Simulator
//
//  Created by CMouse on 2017/11/15.
//  Copyright © 2017年 Peking University. All rights reserved.
//

#ifndef MulticycleSimulator_hpp
#define MulticycleSimulator_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX 400000000
unsigned char mem[MAX] = {0};

typedef unsigned long long REG;
REG reg[32] = {0};

#define MEM_NONE 0
#define MEM_BYTE 1
#define MEM_HALF 2
#define MEM_WORD 4
#define MEM_DOUBLE 8

void load_memory(const char * filename);
void simulate();
void print_reg();
void print_mem();
void set_breakpoint();
void check_result();

void do_fetch();
void do_decode();
void do_execute();
void do_memory();
void do_writeback();

// Get cnt bits starting from position s
unsigned int getbit(unsigned inst, int s, int cnt) {
    unsigned int mask = (1 << cnt) - 1;
    mask <<= s;
    return (inst & mask) >> s;
}

// Perform Sign Extension
unsigned long ext_signed(unsigned int src, unsigned char len) {
    unsigned int sign_bit = (1 << (len - 1)) & src;
    if (sign_bit == 0) {
        return src;
    }
    else {
        unsigned long mask = ~((1 << len) - 1);
        return mask | src;
    }
}

#endif /* MulticycleSimulator_hpp */
