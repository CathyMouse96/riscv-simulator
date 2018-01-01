//
//  MulticycleRegisters.h
//  RISCV-Simulator
//
//  Created by CMouse on 2017/11/15.
//  Copyright © 2017年 Peking University. All rights reserved.
//

#ifndef MulticycleRegisters_h
#define MulticycleRegisters_h

struct Fetch {
    unsigned long PC;
    // Signals from Control Unit
    unsigned char PCWrite;
} _Fetch;

struct fetch {
    unsigned int inst;
} _fetch;

struct Decode {
    unsigned int inst;
    unsigned long PC;
} _Decode;

struct decode {
    REG val_rs1;
    REG val_rs2;
    unsigned long imm;
    unsigned short rd;
} _decode;

struct Execute {
    REG val_rs1;
    REG val_rs2;
    unsigned long imm;
    unsigned short rd;
    unsigned long PC;
    // Signals from Control Unit
    unsigned char ALUSrc;
    unsigned char ALUOp;
    
} _Execute;

struct execute {
    REG aluout;
} _execute;

struct Memory {
    REG aluout;
    REG val_rs2;
    unsigned long imm;
    unsigned short rd;
    unsigned long PC;
    // Signals from Control Unit
    unsigned char Branch;
    unsigned char Jump;
    unsigned char Return;
    unsigned char MemRead;
    unsigned char MemWrite;
    unsigned char MemoutSelect;
} _Memory;

struct memory {
    REG memout;
    unsigned long PC; // for Fetch
} _memory;

struct WriteBack {
    REG aluout;
    REG memout;
    unsigned short rd;
    // Signals from Control Unit
    unsigned char RegWrite;
    unsigned char MemtoReg;
} _WriteBack;

#endif /* MulticycleRegisters_h */
