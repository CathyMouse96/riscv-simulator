//
//  MulticycleFSM.h
//  RISCV-Simulator
//
//  Created by CMouse on 2017/11/17.
//  Copyright © 2017年 Peking University. All rights reserved.
//

#ifndef MulticycleFSM_h
#define MulticycleFSM_h

#include "Instructions.h"

struct ControlUnit {
    unsigned int opcode;
    unsigned int funct3;
    unsigned int funct7;
} _ControlUnit;

unsigned char getPCWrite(unsigned int cycle) {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    // Special cases
    else if (_ControlUnit.opcode == 0x33 && (_ControlUnit.funct3 == 0x0 || _ControlUnit.funct3 == 0x1) && _ControlUnit.funct7 == 0x01) { // 64-bit multiplication needs 6 cycles
        if (cycle == 5) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x3B && _ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x01) { // mulw rd, rs1, rs2
        if (cycle == 5) { // 64-bit multiplication needs 6 cycles
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x33 && (_ControlUnit.funct3 == 0x4 || _ControlUnit.funct3 == 0x6) && _ControlUnit.funct7 == 0x01) { // 64-bit division needs 40 cycles
        if (cycle == 39) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x3B && _ControlUnit.funct3 == 0x4 && _ControlUnit.funct7 == 0x01) { // divw rd, rs1, rs2
        if (cycle == 39) { // 64-bit division needs 40 cycles
            return 1;
        } else {
            return 0;
        }
    }
    // End special cases
    else if (_ControlUnit.opcode == 0x33 || _ControlUnit.opcode == 0x03 || _ControlUnit.opcode == 0x13 || _ControlUnit.opcode == 0x1B || _ControlUnit.opcode == 0x67) { // need five cycles
        if (cycle == 4) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x3B) { // wide register arithmetic
        if (cycle == 4) { // need five cycles
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x23 || _ControlUnit.opcode == 0x63) { // need four cycles
        if (cycle == 3) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x17 || _ControlUnit.opcode == 0x37 || _ControlUnit.opcode == 0x6f) { // need five cycles
        if (cycle == 4) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

unsigned char getALUSrc() { // return 1 for imm, 0 for val_rs2
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x03 || _ControlUnit.opcode == 0x13 || _ControlUnit.opcode == 0x1B || _ControlUnit.opcode == 0x67 || _ControlUnit.opcode == 0x23) {
        return 1;
    }
    return 0;
}

unsigned char getALUOp() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x03 || _ControlUnit.opcode == 0x23) {
        return ALU_ADD;
    }
    else if (_ControlUnit.opcode == 0x33) {
        if (_ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x00) { // add rd, rs1, rs2
            return ALU_ADD;
        } else if (_ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x01) { // mul rd, rs1, rs2
            return ALU_MUL;
        } else if (_ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x20) { // sub rd, rs1, rs2
            return ALU_SUB;
        } else if (_ControlUnit.funct3 == 0x1 && _ControlUnit.funct7 == 0x00) { // sll rd, rs1, rs2
            return ALU_SLL;
        } else if (_ControlUnit.funct3 == 0x1 && _ControlUnit.funct7 == 0x01) { //mulh rd, rs1, rs2
            return ALU_MUL;
        } else if (_ControlUnit.funct3 == 0x2 && _ControlUnit.funct7 == 0x00) { // slt rd, rs1, rs2
            return ALU_SLT;
        } else if (_ControlUnit.funct3 == 0x4 && _ControlUnit.funct7 == 0x00) { // xor rd, rs1, rs2
            return ALU_XOR;
        } else if (_ControlUnit.funct3 == 0x4 && _ControlUnit.funct7 == 0x01) { // div rd, rs1, rs2
            return ALU_DIV;
        } else if (_ControlUnit.funct3 == 0x5 && _ControlUnit.funct7 == 0x00) { // srl rd, rs1, rs2
            return ALU_SRL;
        } else if (_ControlUnit.funct3 == 0x5 && _ControlUnit.funct7 == 0x20) { // sra rd, rs1, rs2
            return ALU_SRA;
        } else if (_ControlUnit.funct3 == 0x6 && _ControlUnit.funct7 == 0x00) { // or rd, rs1, rs2
            return ALU_OR;
        } else if (_ControlUnit.funct3 == 0x6 && _ControlUnit.funct7 == 0x01) { // rem rd, rs1, rs2
            return ALU_REM;
        } else if (_ControlUnit.funct3 == 0x7 && _ControlUnit.funct7 == 0x00) { // and rd, rs1, rs2
            return ALU_AND;
        }
    }
    else if (_ControlUnit.opcode == 0x3B) { // wide register arithmetic
        if (_ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x0) { // addw rd, rs1, rs2
            return ALU_ADD;
        } else if (_ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x20) { // subw rd, rs1, rs2
            return ALU_SUB;
        } else if (_ControlUnit.funct3 == 0x1 && _ControlUnit.funct7 == 0x0) { // sllw rd, rs1, rs2
            return ALU_SLL;
        } else if (_ControlUnit.funct3 == 0x5 && _ControlUnit.funct7 == 0x0) { // srlw rd, rs1, rs2
            return ALU_SRL;
        } else if (_ControlUnit.funct3 == 0x5 && _ControlUnit.funct7 == 0x20) { // sraw rd, rs1, rs2
            return ALU_SRA;
        } else if (_ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x01) { // mulw rd, rs1, rs2
            return ALU_MUL;
        } else if (_ControlUnit.funct3 == 0x4 && _ControlUnit.funct7 == 0x01) { // divw rd, rs1, rs2
            return ALU_DIV;
        }
    }
    else if (_ControlUnit.opcode == 0x13) {
        if (_ControlUnit.funct3 == 0x0) { // addi rd, rs1, imm
            return ALU_ADD;
        } else if (_ControlUnit.funct3 == 0x1 && _ControlUnit.funct7 == 0x00) { // slli rd, rs1, imm
            return ALU_SLL;
        } else if (_ControlUnit.funct3 == 0x2) { // slti rd, rs1, imm
            return ALU_SLT;
        } else if (_ControlUnit.funct3 == 0x4) { // xori rd, rs1, imm
            return ALU_XOR;
        } else if (_ControlUnit.funct3 == 0x5 && _ControlUnit.funct7 == 0x00) { // srli rd, rs1, imm
            return ALU_SRL;
        }  else if (_ControlUnit.funct3 == 0x5 && _ControlUnit.funct7 == 0x10) { // srai rd, rs1, imm
            return ALU_SRA;
        } else if (_ControlUnit.funct3 == 0x6) { // ori rd, rs1, imm
            return ALU_OR;
        }  else if (_ControlUnit.funct3 == 0x7) { // andi rd, rs1, imm
            return ALU_AND;
        }
    }
    else if (_ControlUnit.opcode == 0x1B) {
        if (_ControlUnit.funct3 == 0x0) { // addiw rd, rs1, imm
            return ALU_ADD;
        } else if (_ControlUnit.funct3 == 0x1 && _ControlUnit.funct7 == 0x00) { // slliw rd, rs1, imm
            return ALU_SLL;
        }
    }
    else if (_ControlUnit.opcode == 0x67) {
        if (_ControlUnit.funct3 == 0x0) { // jalr rd, rsi, imm
            return ALU_ADD;
        }
    }
    else if (_ControlUnit.opcode == 0x63) {
        if (_ControlUnit.funct3 == 0x0) { // beq rs1, rs2, offset
            return ALU_SEQ;
        } else if (_ControlUnit.funct3 == 0x1) { // bne rs1, rs2, offset
            return ALU_SNE;
        } else if (_ControlUnit.funct3 == 0x4) { // blt rs1, rs2, offset
            return ALU_SLT;
        } else if (_ControlUnit.funct3 == 0x5) { // bge rs1, rs2, offset
            return ALU_SGE;
        }
    }
    return 0;
}

unsigned char getBranch() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x63) { // branch instructions
        return 1;
    }
    return 0;
}

unsigned char getJump() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x6f) { // jal rd, imm
        return 1;
    }
    return 0;
}

unsigned char getReturn() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x67 && _ControlUnit.funct3 == 0x0) { // jalr rd, rs1, imm
        return 1;
    }
    return 0;
}

unsigned char getMemRead() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x03) { // load instructions
        return 1;
    }
    return 0;
}

unsigned char getMemWrite(unsigned int cycle) {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x23) { // store instructions
        if (cycle == 3) {
            return 1;
        }
        return 0;
    }
    return 0;
}

unsigned char getMemoutSelect() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x67 && _ControlUnit.funct3 == 0) { // jalr rd, rs1, imm
        return 1; // PC
    }
    else if (_ControlUnit.opcode == 0x17) { // auipc rd, offset
        return 1; // PC
    }
    else if (_ControlUnit.opcode == 0x37) { // lui rd, offset
        return 2; // imm
    }
    else if (_ControlUnit.opcode == 0x6f) { // jal rd, imm
        return 1; // PC
    }
    return 0;
}

unsigned char getRegWrite(unsigned int cycle) {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x23 || _ControlUnit.opcode == 0x63) {
        return 0;
    }
    // Special cases
    else if (_ControlUnit.opcode == 0x33 && (_ControlUnit.funct3 == 0x0 || _ControlUnit.funct3 == 0x1) && _ControlUnit.funct7 == 0x01) { // 64-bit multiplication needs 6 cycles
        if (cycle == 5) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x3B && _ControlUnit.funct3 == 0x0 && _ControlUnit.funct7 == 0x01) { // mulw rd, rs1, rs2
        if (cycle == 5) { // 64-bit multiplication needs 6 cycles
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x33 && (_ControlUnit.funct3 == 0x4 || _ControlUnit.funct3 == 0x6) && _ControlUnit.funct7 == 0x01) { // 64-bit division needs 40 cycles
        if (cycle == 39) {
            return 1;
        } else {
            return 0;
        }
    }
    else if (_ControlUnit.opcode == 0x3B && _ControlUnit.funct3 == 0x4 && _ControlUnit.funct7 == 0x01) { // divw rd, rs1, rs2
        if (cycle == 39) { // 64-bit division needs 40 cycles
            return 1;
        } else {
            return 0;
        }
    }
    // End special cases
    else if (cycle == 4) {
        return 1;
    }
    return 0;
}

unsigned char getMemtoReg() {
    if (_ControlUnit.opcode == 0) { // nop instruction
        return 0;
    }
    else if (_ControlUnit.opcode == 0x03) { // load instructions
        return 1;
    }
    else if (_ControlUnit.opcode == 0x67 && _ControlUnit.funct3 == 0) { // jalr rd, rs1, imm
        return 1; // PC
    }
    else if (_ControlUnit.opcode == 0x17) { // auipc rd, offset
        return 1; // PC
    }
    else if (_ControlUnit.opcode == 0x37) { // lui rd, offset
        return 1; // imm
    }
    else if (_ControlUnit.opcode == 0x6f) { // jal rd, imm
        return 1; // PC
    }
    return 0;
}

#endif /* MulticycleFSM_h */
