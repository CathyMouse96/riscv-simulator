//
//  PipelineSimulator.cpp
//  RISCV-Simulator
//
//  Created by CMouse on 2017/11/17.
//  Copyright © 2017年 Peking University. All rights reserved.
//

#include "PipelineSimulator.hpp"
#include "PipelineRegisters.h"
#include "PipelineControl.h"
#include "Instructions.h"

#include <set>
#include <string>
#include <map>
using namespace std;

#define MAX_BREAKPOINTS 100

// .text segment
extern unsigned int tadr;
extern unsigned int tsize;
extern unsigned int tvadr;

// .data segment
extern unsigned int dadr;
extern unsigned int dsize;
extern unsigned int dvadr;

extern unsigned long entry_main;
extern unsigned long size_main;
extern unsigned long endpc_main;
extern FILE *elf;

extern unsigned long global_pointer;
extern unsigned long result;

extern bool open_file(const char * filename);
extern void read_elf(const char * filename);

extern map<string, unsigned long> symboltable;

#define isRType(op) (op == 0x33 || op == 0x3B)
#define isSType(op) (op == 0x23)
#define isSBType(op) (op == 0x63)

bool step_mode = false;
bool debug_mode = false;
unsigned int clock_cycles = 0;
unsigned int instruction_cnt = 0;
unsigned int data_cnt = 0;
unsigned int prediction_cnt = 0;
unsigned int jump_cnt = 0;

set<unsigned long> breakpoints;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: Simulator [options] <filename>\n");
        printf("Options:\n");
        printf("-d\tDebug mode.\n");
        return 1;
    }
    
    else if (argc == 2) {
        read_elf(argv[1]);
        load_memory(argv[1]);
    }
    else if (argc == 3) {
        if (strncmp(argv[1], "-d", 2) == 0) {
            debug_mode = true;
        }
        read_elf(argv[argc - 1]);
        load_memory(argv[argc - 1]);
    }
    
    // PC Initialization
    _Fetch.PC = entry_main;
    endpc_main = entry_main + size_main - 4; // exclude ret
    
    // Pipeline Registers Initialization
    _Decode.inst = 0;
    _Decode.PC = 0;
    _Execute.val_rs1 = 0;
    _Execute.val_rs2 = 0;
    _Execute.imm = 0;
    _Execute.rd = 0;
    
    
    // Register File Initialization
    for (int i = 0; i < 32; i++) {
        reg[i] = 0;
    }
    reg[2] = MAX / 2; // Stack pointer
    
    reg[3] = (REG)global_pointer; // Global pointer
    printf("\n");
    printf("Global pointer = %llx\n", reg[3]);
    printf("Endpc_main = %lx\n", endpc_main);
    
    printf("\n");
    printf("Start simulation.\n");
    printf("Please choose:\n");
    printf("[1] Run\n");
    printf("[2] Run by step\n");
    printf("[3] Exit\n");
    printf("[4] Set breakpoint\n");
    
    char choice;
    cin >> choice;
    while (choice != '1' && choice != '2' && choice != '3') {
        if (choice == '4') {
            set_breakpoint();
        }
        printf("\n");
        printf("Please choose:\n");
        printf("[1] Run\n");
        printf("[2] Run by step\n");
        printf("[3] Exit\n");
        printf("[4] Set breakpoint\n");
        cin >> choice;
    }
    
    if (choice == '3') {
        return 0;
    } else if (choice == '2') {
        step_mode = true;
    }
    
    simulate();
    
    printf("\n");
    printf("Simulation completed.\n");
    printf("Total number of instructions run: %u\n", instruction_cnt);
    printf("Total number of clock cycles: %u\n", clock_cycles);
    printf("Pipeline CPI = %lf\n", ((double)clock_cycles) / ((double)instruction_cnt));
    printf("\n");
    printf("Total number of data hazards: %u\n", data_cnt);
    printf("Total number of prediction hazards: %u\n", prediction_cnt);
    printf("Total number of jump hazards: %u\n", jump_cnt);
    printf("Please choose:\n");
    printf("[1] Check register\n");
    printf("[2] Check memory\n");
    printf("[3] Exit\n");
    printf("[4] Check result\n");
    
    cin >> choice;
    while (choice != '3') {
        if (choice == '1') {
            print_reg();
        }
        else if (choice == '2') {
            print_mem();
        }
        else if (choice == '4') {
            check_result();
        }
        printf("\n");
        printf("Please choose:\n");
        printf("[1] Check register\n");
        printf("[2] Check memory\n");
        printf("[3] Exit\n");
        printf("[4] Check result\n");
        cin >> choice;
    }
    
    return 0;
}

void load_memory(const char * filename) {
    open_file(filename);
    // Load .text segment
    fseek(elf, tadr, SEEK_SET);
    fread(&mem[tvadr], 1, tsize, elf);
    // Load .data segment
    fseek(elf, dadr, SEEK_SET);
    fread(&mem[dvadr], 1, dsize, elf);
    fclose(elf);
}

void print_reg() {
    printf("Status of Register File:\n");
    for (int i = 0; i < 32; i+=2) {
        printf("x%i\t", i);
        printf("%017llx\t", reg[i]);
        printf("x%i\t", i + 1);
        printf("%017llx\n", reg[i + 1]);
    }
}

void print_mem() {
    printf("Enter Memory Address (hexadecimal):\n");
    unsigned int addr = 0;
    scanf("%x", &addr);
    if (addr >= MAX) {
        printf("Error: Out of range.\n");
        return;
    }
    printf("Status of Memory:\n");
    unsigned int print_range = 16;
    unsigned int addr_start = addr & (~(print_range - 1));
    for (unsigned int i = addr_start; i < addr_start + print_range; i++) {
        printf("%x\t", i);
        printf("%2x\n", mem[i]);
    }
}

void set_breakpoint() {
    printf("Enter breakpoint address (hexadecimal):\n");
    unsigned long addr = 0;
    scanf("%lx", &addr);
    if (addr >= MAX) {
        printf("Error: Out of range.\n");
        return;
    }
    breakpoints.insert(addr);
    printf("Breakpoint at *0x%lx.\n", addr);
}

void check_result() {
    printf("Enter global variable name:\n");
    string input;
    cin >> input;
    if (symboltable.count(input) > 0) {
        unsigned long addr = symboltable[input];
        cout << "Variable " << input << " is located at ";
        printf("%lx.\n", addr);
        printf("Word value = 0x%x.\n", *((unsigned int *) &mem[addr]));
    }
    else {
        printf("No such variable.\n");
    }
}

void simulate() {
    unsigned long end = endpc_main;
    while (_Fetch.PC != end) {
        // Five stages actually run in parallel
        if (debug_mode == true) {
            printf("\n");
            printf("Fetch stage: instruction at *0x%lx\n", _Fetch.PC);
        }
        do_fetch();
        if (debug_mode == true) {
            printf("\n");
            printf("Decode stage: instruction at *0x%lx\n", _Decode.PC);
        }
        do_decode();
        if (debug_mode == true) {
            printf("\n");
            printf("Execute stage: instruction at *0x%lx\n", _Execute.PC);
        }
        do_execute();
        if (debug_mode == true) {
            printf("\n");
            printf("Memory stage: instruction at *0x%lx\n", _Memory.PC);
        }
        do_memory();
        do_writeback();
        
        // Update pipeline registers at end of clock cycle
        _WriteBack.aluout = _Memory.aluout;
        _WriteBack.memout = _memory.memout;
        _WriteBack.rd = _Memory.rd;
        _WriteBack.WB_RegWrite = _Memory.WB_RegWrite;
        _WriteBack.WB_MemtoReg = _Memory.WB_MemtoReg;
        
        _Memory.aluout = _execute.aluout;
        _Memory.val_rs2 = _Execute.val_rs2;
        _Memory.imm = _Execute.imm;
        _Memory.rd = _Execute.rd;
        _Memory.PC = _Execute.PC;
        _Memory.MEM_Branch = _Execute.MEM_Branch;
        _Memory.MEM_Jump = _Execute.MEM_Jump;
        _Memory.MEM_Return = _Execute.MEM_Return;
        _Memory.MEM_MemRead = _Execute.MEM_MemRead;
        _Memory.MEM_MemWrite = _Execute.MEM_MemWrite;
        _Memory.MEM_MemoutSelect = _Execute.MEM_MemoutSelect;
        _Memory.WB_RegWrite = _Execute.WB_RegWrite;
        _Memory.WB_MemtoReg = _Execute.WB_MemtoReg;
        
        if (_Execute.stalled) { // Stall
            _Execute.stalled = 0;
        } else if (_Execute.bubbled) { // Bubble
            _Execute.val_rs1 = 0;
            _Execute.val_rs2 = 0;
            _Execute.imm = 0;
            _Execute.rd = 0;
            _Execute.PC = 0;
            _Execute.EX_ALUSrc = 0;
            _Execute.EX_ALUOp = 0;
            _Execute.MEM_Branch = 0;
            _Execute.MEM_Jump = 0;
            _Execute.MEM_Return = 0;
            _Execute.MEM_MemRead = 0;
            _Execute.MEM_MemWrite = 0;
            _Execute.MEM_MemoutSelect = 0;
            _Execute.WB_RegWrite = 0;
            _Execute.WB_MemtoReg = 0;
            _Execute.bubbled = 0;
        } else { // Normal
            _Execute.val_rs1 = _decode.val_rs1;
            _Execute.val_rs2 = _decode.val_rs2;
            _Execute.imm = _decode.imm;
            _Execute.rd = _decode.rd;
            _Execute.PC = _Decode.PC;
            _Execute.EX_ALUSrc = _decode.EX_ALUSrc;
            _Execute.EX_ALUOp = _decode.EX_ALUOp;
            _Execute.MEM_Branch = _decode.MEM_Branch;
            _Execute.MEM_Jump = _decode.MEM_Jump;
            _Execute.MEM_Return = _decode.MEM_Return;
            _Execute.MEM_MemRead = _decode.MEM_MemRead;
            _Execute.MEM_MemWrite = _decode.MEM_MemWrite;
            _Execute.MEM_MemoutSelect = _decode.MEM_MemoutSelect;
            _Execute.WB_RegWrite = _decode.WB_RegWrite;
            _Execute.WB_MemtoReg = _decode.WB_MemtoReg;
        }
        
        if (_Decode.stalled) { // Stall
            _Decode.stalled = 0;
        } else if (_Decode.bubbled) { // Bubble
            _Decode.inst = 0;
            _Decode.PC = 0;
            _Decode.bubbled = 0;
        } else { // Normal
            instruction_cnt++;
            _Decode.inst = _fetch.inst;
            _Decode.PC = _Fetch.PC;
        }
        
        if (_Fetch.stalled) {
            _Fetch.stalled = 0;
        } else {
            _Fetch.PC = _fetch.PC;
        }
        
        clock_cycles++;
        
        if (step_mode == true || breakpoints.count(_Fetch.PC) != 0) {
            printf("\n");
            printf("Please choose:\n");
            printf("[1] Check register\n");
            printf("[2] Check memory\n");
            printf("[3] Continue\n");
            printf("[4] Set breakpoint\n");
            printf("[5] Step\n");
            
            char choice;
            cin >> choice;
            while (choice != '3' && choice != '5') {
                if (choice == '1') {
                    print_reg();
                }
                else if (choice == '2') {
                    print_mem();
                }
                else if (choice == '4') {
                    set_breakpoint();
                }
                printf("\n");
                printf("Please choose:\n");
                printf("[1] Check register\n");
                printf("[2] Check memory\n");
                printf("[3] Continue\n");
                printf("[4] Set breakpoint\n");
                printf("[5] Step\n");
                cin >> choice;
            }
            if (choice == '3') {
                step_mode = false;
            } else if (choice == '5') {
                step_mode = true;
            }
        }
    }
}

void do_fetch() {
    // Read instruction memory
    unsigned long PC = _Fetch.PC;
    _fetch.inst = *((unsigned int *) &mem[PC]);
    // Calculate PC
    if (_Memory.MEM_Branch && _Memory.aluout) {
        _fetch.PC = _Memory.PC + _Memory.imm;
    } else if (_Memory.MEM_Jump) {
        _fetch.PC = _Memory.PC + _Memory.imm;
    } else if (_Memory.MEM_Return) {
        _fetch.PC = _Memory.aluout;
    } else {
        _fetch.PC = PC + 4;
    }
}

void do_decode() {
    // Decode instruction
    unsigned int inst = _Decode.inst;
    unsigned int OP = getbit(inst, 0, 7);
    unsigned int funct3 = getbit(inst, 12, 3);
    unsigned int funct7 = getbit(inst, 25, 7);
    
    // Set signals
    _decode.EX_ALUSrc = getALUSrc(OP, funct3, funct7);
    _decode.EX_ALUOp = getALUOp(OP, funct3, funct7);
    _decode.MEM_Branch = getBranch(OP, funct3, funct7);
    _decode.MEM_Jump = getJump(OP, funct3, funct7);
    _decode.MEM_Return = getReturn(OP, funct3, funct7);
    _decode.MEM_MemRead = getMemRead(OP, funct3, funct7);
    _decode.MEM_MemWrite = getMemWrite(OP, funct3, funct7);
    _decode.MEM_MemoutSelect = getMemoutSelect(OP, funct3, funct7);
    _decode.WB_RegWrite = getRegWrite(OP, funct3, funct7);
    _decode.WB_MemtoReg = getMemtoReg(OP, funct3, funct7);
    
    unsigned int rs1 = getbit(inst, 15, 5);
    unsigned int rs2 = getbit(inst, 20, 5);
    unsigned int rd = getbit(inst, 7, 5);
    
    // Test for data hazard
    if (_Execute.WB_RegWrite && _Execute.rd != 0) {
        if (_Execute.rd == rs1) {
            dataHazard();
        } else if (_Execute.rd == rs2 && (isRType(OP) || isSType(OP) || isSBType(OP))) {
            dataHazard();
        }
    }
    else if (_Memory.WB_RegWrite && _Memory.rd != 0) {
        if (_Memory.rd == rs1) {
            dataHazard();
        } else if (_Memory.rd == rs2 && (isRType(OP) || isSType(OP) || isSBType(OP))) {
            dataHazard();
        }
    }
    else if (_WriteBack.WB_RegWrite && _WriteBack.rd != 0) {
        if (_WriteBack.rd == rs1) {
            dataHazard();
        } else if (_WriteBack.rd == rs2 && (isRType(OP) || isSType(OP) || isSBType(OP))) {
            dataHazard();
        }
    }
    
    // Test for jump hazard
    if (_decode.MEM_Jump || _decode.MEM_Return) {
        jumpHazard();
    }
    
    _decode.val_rs1 = reg[rs1];
    _decode.val_rs2 = reg[rs2];
    if (OP == 0x03 || OP == 0x13 || OP == 0x1B || OP == 0x67 || OP == 0x73) { // I-TYPE INSTRUCTION
        unsigned int i_imm = getbit(inst, 20, 12);
        _decode.imm = ext_signed(i_imm, 12);
    }
    else if (OP == 0x23) { // S-TYPE INSTRUCTION
        unsigned int s_imm = getbit(inst, 25, 7) << 5;
        s_imm += getbit(inst, 7, 5);
        _decode.imm = ext_signed(s_imm, 12);
    }
    else if (OP == 0x63) { // SB-TYPE INSTRUCTION
        unsigned int sb_imm = getbit(inst, 31, 1) << 12;
        sb_imm += getbit(inst, 7, 1) << 11;
        sb_imm += getbit(inst, 25, 6) << 5;
        sb_imm += getbit(inst, 8, 4) << 1;
        _decode.imm = ext_signed(sb_imm, 13);
    }
    else if (OP == 0x17 || OP == 0x37) { // U-TYPE INSTRUCTION
        unsigned int u_imm = getbit(inst, 12, 20) << 12;
        _decode.imm = ext_signed(u_imm, 32);
    }
    else if (OP == 0x6f) { // UJ-TYPE INSTRUCTION
        unsigned int uj_imm = getbit(inst, 31, 1) << 20;
        uj_imm += getbit(inst, 12, 8) << 12;
        uj_imm += getbit(inst, 20, 1) << 11;
        uj_imm += getbit(inst, 21, 10) << 1;
        _decode.imm = ext_signed(uj_imm, 21);
    }
    else {
        _decode.imm = 0;
    }
    _decode.rd = rd;
    if (debug_mode) {
        printf("DEBUG: _decode.imm = %lx\n", _decode.imm);
        printf("DEBUG: _decode.MEM_MemRead = %x\n", _decode.MEM_MemRead);
        printf("DEBUG: _decode.MEM_MemWrite = %x\n", _decode.MEM_MemWrite);
        printf("DEBUG: _decode.MEM_MemoutSelect = %x\n", _decode.MEM_MemoutSelect);
        printf("DEBUG: _decode.WB_RegWrite = %x\n", _decode.WB_RegWrite);
        printf("DEBUG: _decode.WB_MemtoReg = %x\n", _decode.WB_MemtoReg);
    }
}

void do_execute() {
    if (debug_mode) {
        printf("DEBUG: _Execute.ALUSrc = %x\n", _Execute.EX_ALUSrc);
        printf("DEBUG: _Execute.ALUOp = %x\n", _Execute.EX_ALUOp);
    }
    // NOTE: May have precision errors
    REG alu_val1 = _Execute.val_rs1;
    REG alu_val2 = _Execute.EX_ALUSrc? _Execute.imm:_Execute.val_rs2;
    if (debug_mode) {
        printf("DEBUG: alu_val1 = %llx\n", alu_val1);
        printf("DEBUG: alu_val2 = %llx\n", alu_val2);
    }
    switch (_Execute.EX_ALUOp) {
        case ALU_ADD:
            _execute.aluout = alu_val1 + alu_val2;
            break;
        case ALU_SUB:
            _execute.aluout = alu_val1 - alu_val2;
            break;
        case ALU_MUL:
            _execute.aluout = alu_val1 * alu_val2;
            break;
        case ALU_DIV:
            _execute.aluout = alu_val1 / alu_val2;
            break;
        case ALU_REM:
            _execute.aluout = alu_val1 % alu_val2;
            break;
        case ALU_AND:
            _execute.aluout = alu_val1 & alu_val2;
            break;
        case ALU_OR:
            _execute.aluout = alu_val1 | alu_val2;
            break;
        case ALU_XOR:
            _execute.aluout = alu_val1 ^ alu_val2;
            break;
        case ALU_SLL:
            _execute.aluout = alu_val1 << alu_val2;
            break;
        case ALU_SRL:
            _execute.aluout = alu_val1 >> alu_val2;
            break;
        case ALU_SRA:
            _execute.aluout = ((signed long long )alu_val1) >> alu_val2;
            break;
        case ALU_SEQ:
            _execute.aluout = (alu_val1 == alu_val2)? 1:0;
            break;
        case ALU_SNE:
            _execute.aluout = (alu_val1 != alu_val2)? 1:0;
            break;
        case ALU_SLT:
            _execute.aluout = (alu_val1 < alu_val2)? 1:0;
            break;
        case ALU_SGE:
            _execute.aluout = (alu_val1 >= alu_val2)? 1:0;
            break;
        default:
            break;
    }
    if (debug_mode) {
        printf("DEBUG: aluout = %llx\n", _execute.aluout);
    }
    // Test for prediction hazard
    if (_Execute.MEM_Branch && _execute.aluout){
        predictionHazard();
    }
    // Test for jump hazard
    if (_Execute.MEM_Jump || _Execute.MEM_Return) {
        jumpHazard();
    }
}

void do_memory() {
    // Read from memory
    if (_Memory.MEM_MemRead == MEM_BYTE) {
        _memory.memout = (REG) *((unsigned char *) &mem[_Memory.aluout]);
    }
    else if (_Memory.MEM_MemRead == MEM_HALF) {
        _memory.memout = (REG) *((unsigned short *) &mem[_Memory.aluout]);
    }
    else if (_Memory.MEM_MemRead == MEM_WORD) {
        _memory.memout = (REG) *((unsigned int *) &mem[_Memory.aluout]);
    }
    else if (_Memory.MEM_MemRead == MEM_DOUBLE) {
        _memory.memout = *((unsigned long long *) &mem[_Memory.aluout]);
    }
    // Write to memory
    else if (_Memory.MEM_MemWrite == MEM_BYTE) {
        *((unsigned char *) &mem[_Memory.aluout]) = (_Memory.val_rs2 & 0xFF);
    }
    else if (_Memory.MEM_MemWrite == MEM_HALF) {
        *((unsigned short *) &mem[_Memory.aluout]) = (_Memory.val_rs2 & 0xFFFF);
    }
    else if (_Memory.MEM_MemWrite == MEM_WORD) {
        *((unsigned int *) &mem[_Memory.aluout]) = (_Memory.val_rs2 & 0xFFFFFFFF);
    }
    else if (_Memory.MEM_MemWrite == MEM_DOUBLE) {
        *((unsigned long long *) &mem[_Memory.aluout]) = _Memory.val_rs2;
    }
    // Set memout
    if (_Memory.MEM_MemoutSelect == 1) {
        if (_Memory.MEM_Jump || _Memory.MEM_Return) {
            _memory.memout = _Memory.PC + 4;
        } else {
            _memory.memout = _Memory.PC + _Memory.imm;
        }
    } else if (_Memory.MEM_MemoutSelect == 2) {
        _memory.memout = _Memory.imm;
    }
    
    if (debug_mode) {
        printf("DEBUG: _Memory.Branch = %x\n", _Memory.MEM_Branch);
        printf("DEBUG: _Memory.MemoutSelect = %x\n", _Memory.MEM_MemoutSelect);
        printf("DEBUG: _Memory.aluout = %llx\n", _Memory.aluout);
        printf("DEBUG: _Memory.imm = %lx\n", _Memory.imm);
        printf("DEBUG: _memory.memout = %llx\n", _memory.memout);
    }
    
    // Test for prediction hazard
    if (_Memory.MEM_Branch && _Memory.aluout){
        predictionHazard_normalFetch();
    }
    // Test for jump hazard
    if (_Memory.MEM_Jump || _Memory.MEM_Return) {
        jumpHazard_normalFetch();
    }
}

void do_writeback() {
    if (debug_mode) {
        printf("DEBUG: _WriteBack.RegWrite = %x\n", _WriteBack.WB_RegWrite);
        printf("DEBUG: _WriteBack.rd = %i\n", _WriteBack.rd);
    }
    if (!_WriteBack.WB_RegWrite || (_WriteBack.rd == 0)) {
        return;
    }
    if (_WriteBack.WB_MemtoReg) {
        reg[_WriteBack.rd] = _WriteBack.memout;
    } else {
        reg[_WriteBack.rd] = _WriteBack.aluout;
    }
}
