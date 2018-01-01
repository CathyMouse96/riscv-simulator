//
//  PipelineRegisters.h
//  RISCV-Simulator
//
//  Created by CMouse on 2017/11/17.
//  Copyright © 2017年 Peking University. All rights reserved.
//

#ifndef PipelineRegisters_h
#define PipelineRegisters_h

struct Fetch {
    unsigned long PC;
    // Signal for pipeline control
    unsigned char stalled;
} _Fetch;

struct fetch {
    unsigned int inst;
    unsigned long PC;
} _fetch;

struct Decode {
    unsigned int inst;
    unsigned long PC;
    // Signals for pipeline control
    unsigned char stalled;
    unsigned char bubbled;
} _Decode;

struct decode {
    REG val_rs1;
    REG val_rs2;
    unsigned long imm;
    unsigned short rd;
    // Signals for execute stage
    unsigned char EX_ALUSrc;
    unsigned char EX_ALUOp;
    // Signals for memory stage
    unsigned char MEM_Branch;
    unsigned char MEM_Jump;
    unsigned char MEM_Return;
    unsigned char MEM_MemRead;
    unsigned char MEM_MemWrite;
    unsigned char MEM_MemoutSelect;
    // Signals for writeback stage
    unsigned char WB_RegWrite;
    unsigned char WB_MemtoReg;
} _decode;

struct Execute {
    REG val_rs1;
    REG val_rs2;
    unsigned long imm;
    unsigned short rd;
    unsigned long PC;
    // Signals for execute stage
    unsigned char EX_ALUSrc;
    unsigned char EX_ALUOp;
    // Signals for memory stage
    unsigned char MEM_Branch;
    unsigned char MEM_Jump;
    unsigned char MEM_Return;
    unsigned char MEM_MemRead;
    unsigned char MEM_MemWrite;
    unsigned char MEM_MemoutSelect;
    // Signals for writeback stage
    unsigned char WB_RegWrite;
    unsigned char WB_MemtoReg;
    // Signals for pipeline control
    unsigned char stalled;
    unsigned char bubbled;
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
    // Signals for memory stage
    unsigned char MEM_Branch;
    unsigned char MEM_Jump;
    unsigned char MEM_Return;
    unsigned char MEM_MemRead;
    unsigned char MEM_MemWrite;
    unsigned char MEM_MemoutSelect;
    // Signals for writeback stage
    unsigned char WB_RegWrite;
    unsigned char WB_MemtoReg;
} _Memory;

struct memory {
    REG memout;
    unsigned long PC;
} _memory;

struct WriteBack {
    REG aluout;
    REG memout;
    unsigned short rd;
    // Signals for writeback stage
    unsigned char WB_RegWrite;
    unsigned char WB_MemtoReg;
} _WriteBack;

#endif /* PipelineRegisters_h */
