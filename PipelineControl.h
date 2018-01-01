//
//  PipelineControl.h
//  RISCV-Simulator
//
//  Created by CMouse on 2017/11/17.
//  Copyright © 2017年 Peking University. All rights reserved.
//

#ifndef PipelineControl_h
#define PipelineControl_h

#include "PipelineRegisters.h"
#include "Instructions.h"

extern unsigned int instruction_cnt;
extern unsigned int data_cnt;
extern unsigned int prediction_cnt;
extern unsigned int jump_cnt;

void stallFetch() {
    _Fetch.stalled = 1;
}

void stallDecode() {
    _Decode.stalled = 1;
}

void bubbleDecode() {
    _Decode.bubbled = 1;
}

void stallExecute() {
    _Execute.stalled = 1;
}

void bubbleExecute() {
    _Execute.bubbled = 1;
}

void dataHazard() {
    data_cnt++;
    stallFetch();
    stallDecode();
    bubbleExecute();
}

void predictionHazard() {
    prediction_cnt++;
    instruction_cnt--;
    stallFetch();
    bubbleDecode();
    bubbleExecute();
}

void predictionHazard_normalFetch() {
    prediction_cnt++;
    bubbleDecode();
    bubbleExecute();
}

void jumpHazard() {
    jump_cnt++;
    stallFetch();
    bubbleDecode();
}

void jumpHazard_normalFetch() {
    jump_cnt++;
    bubbleDecode();
}

unsigned char getALUSrc(unsigned int opcode, unsigned int funct3, unsigned int funct7) { // return 1 for imm, 0 for val_rs2
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x03 || opcode == 0x13 || opcode == 0x1B || opcode == 0x67 || opcode == 0x23) {
        return 1;
    }
    return 0;
}

unsigned char getALUOp(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x03 || opcode == 0x23) {
        return ALU_ADD;
    }
    else if (opcode == 0x33) {
        if (funct3 == 0x0 && funct7 == 0x00) { // add rd, rs1, rs2
            return ALU_ADD;
        } else if (funct3 == 0x0 && funct7 == 0x01) { // mul rd, rs1, rs2
            return ALU_MUL;
        } else if (funct3 == 0x0 && funct7 == 0x20) { // sub rd, rs1, rs2
            return ALU_SUB;
        } else if (funct3 == 0x1 && funct7 == 0x00) { // sll rd, rs1, rs2
            return ALU_SLL;
        } else if (funct3 == 0x1 && funct7 == 0x01) { //mulh rd, rs1, rs2
            return ALU_MUL;
        } else if (funct3 == 0x2 && funct7 == 0x00) { // slt rd, rs1, rs2
            return ALU_SLT;
        } else if (funct3 == 0x4 && funct7 == 0x00) { // xor rd, rs1, rs2
            return ALU_XOR;
        } else if (funct3 == 0x4 && funct7 == 0x01) { // div rd, rs1, rs2
            return ALU_DIV;
        } else if (funct3 == 0x5 && funct7 == 0x00) { // srl rd, rs1, rs2
            return ALU_SRL;
        } else if (funct3 == 0x5 && funct7 == 0x20) { // sra rd, rs1, rs2
            return ALU_SRA;
        } else if (funct3 == 0x6 && funct7 == 0x00) { // or rd, rs1, rs2
            return ALU_OR;
        } else if (funct3 == 0x6 && funct7 == 0x01) { // rem rd, rs1, rs2
            return ALU_REM;
        } else if (funct3 == 0x7 && funct7 == 0x00) { // and rd, rs1, rs2
            return ALU_AND;
        }
    }
    else if (opcode == 0x3B) { // wide register arithmetic
        if (funct3 == 0x0 && funct7 == 0x0) { // addw rd, rs1, rs2
            return ALU_ADD;
        } else if (funct3 == 0x0 && funct7 == 0x20) { // subw rd, rs1, rs2
            return ALU_SUB;
        } else if (funct3 == 0x1 && funct7 == 0x0) { // sllw rd, rs1, rs2
            return ALU_SLL;
        } else if (funct3 == 0x5 && funct7 == 0x0) { // srlw rd, rs1, rs2
            return ALU_SRL;
        } else if (funct3 == 0x5 && funct7 == 0x20) { // sraw rd, rs1, rs2
            return ALU_SRA;
        } else if (funct3 == 0x0 && funct7 == 0x01) { // mulw rd, rs1, rs2
            return ALU_MUL;
        } else if (funct3 == 0x4 && funct7 == 0x01) { // divw rd, rs1, rs2
            return ALU_DIV;
        }
    }
    else if (opcode == 0x13) {
        if (funct3 == 0x0) { // addi rd, rs1, imm
            return ALU_ADD;
        } else if (funct3 == 0x1 && funct7 == 0x00) { // slli rd, rs1, imm
            return ALU_SLL;
        } else if (funct3 == 0x2) { // slti rd, rs1, imm
            return ALU_SLT;
        } else if (funct3 == 0x4) { // xori rd, rs1, imm
            return ALU_XOR;
        } else if (funct3 == 0x5 && funct7 == 0x00) { // srli rd, rs1, imm
            return ALU_SRL;
        }  else if (funct3 == 0x5 && funct7 == 0x10) { // srai rd, rs1, imm
            return ALU_SRA;
        } else if (funct3 == 0x6) { // ori rd, rs1, imm
            return ALU_OR;
        }  else if (funct3 == 0x7) { // andi rd, rs1, imm
            return ALU_AND;
        }
    }
    else if (opcode == 0x1B) {
        if (funct3 == 0x0) { // addiw rd, rs1, imm
            return ALU_ADD;
        } else if (funct3 == 0x1 && funct7 == 0x00) { // slliw rd, rs1, imm
            return ALU_SLL;
        }
    }
    else if (opcode == 0x67) {
        if (funct3 == 0x0) { // jalr rd, rsi, imm
            return ALU_ADD;
        }
    }
    else if (opcode == 0x63) {
        if (funct3 == 0x0) { // beq rs1, rs2, offset
            return ALU_SEQ;
        } else if (funct3 == 0x1) { // bne rs1, rs2, offset
            return ALU_SNE;
        } else if (funct3 == 0x4) { // blt rs1, rs2, offset
            return ALU_SLT;
        } else if (funct3 == 0x5) { // bge rs1, rs2, offset
            return ALU_SGE;
        }
    }
    return 0;
}

unsigned char getBranch(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x63) { // branch instructions
        return 1;
    }
    return 0;
}

unsigned char getJump(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x6f) { // jal rd, imm
        return 1;
    }
    return 0;
}

unsigned char getReturn(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x67 && funct3 == 0x0) { // jalr rd, rs1, imm
        return 1;
    }
    return 0;
}

unsigned char getMemRead(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x03) { // load instructions
        return 1;
    }
    return 0;
}

unsigned char getMemWrite(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x23) { // store instructions
        return 1;
    }
    return 0;
}

unsigned char getMemoutSelect(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x67 && funct3 == 0) { // jalr rd, rs1, imm
        return 1; // PC
    }
    else if (opcode == 0x17) { // auipc rd, offset
        return 1; // PC
    }
    else if (opcode == 0x37) { // lui rd, offset
        return 2; // imm
    }
    else if (opcode == 0x6f) { // jal rd, imm
        return 1; // PC
    }
    return 0;
}

unsigned char getRegWrite(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x23 || opcode == 0x63) {
        return 0;
    }
    // Special cases
    else if (opcode == 0x33 && (funct3 == 0x0 || funct3 == 0x1) && funct7 == 0x01) { // 64-bit multiplication needs 6 cycles
        return 1;
    }
    else if (opcode == 0x3B && funct3 == 0x0 && funct7 == 0x01) { // mulw rd, rs1, rs2
        return 1;
    }
    else if (opcode == 0x33 && (funct3 == 0x4 || funct3 == 0x6) && funct7 == 0x01) { // 64-bit division needs 40 cycles
        return 1;
    }
    else if (opcode == 0x3B && funct3 == 0x4 && funct7 == 0x01) { // divw rd, rs1, rs2
        return 1;
    }
    // End special cases
    return 1;
}

unsigned char getMemtoReg(unsigned int opcode, unsigned int funct3, unsigned int funct7) {
    if (opcode == 0) { // nop instruction
        return 0;
    }
    else if (opcode == 0x03) { // load instructions
        return 1;
    }
    else if (opcode == 0x67 && funct3 == 0) { // jalr rd, rs1, imm
        return 1; // PC
    }
    else if (opcode == 0x17) { // auipc rd, offset
        return 1; // PC
    }
    else if (opcode == 0x37) { // lui rd, offset
        return 1; // imm
    }
    else if (opcode == 0x6f) { // jal rd, imm
        return 1; // PC
    }
    return 0;
}

#endif /* PipelineControl_h */
