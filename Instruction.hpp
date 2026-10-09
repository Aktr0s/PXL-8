//
// Created by Aktr0s on 27.09.2026.
//

#ifndef PXL_8_INSTRUCTION_HPP
#define PXL_8_INSTRUCTION_HPP

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Operand.hpp"

enum class Op
{
    NOP, MOV, LOAD, STORE, //No operation, Move, Load (from memory), Store (in memory)
    ADD, SUB, MUL, DIV, MOD, //Addition, Subtraction, Multiplication, Division, Modulo
    AND, OR, XOR, NOT, INC, DEC, //Logic Gates, Increment, Decrement
    CMP, JMP, JZ, JNZ, JG, JL, JGE, JLE, //Compare, Jump (Undonditional), Jump (If zero), Jump (If not zero), Jump (if greater than), Jump (if less than), Jump (If greater or equal), Jump (if less or equal)
    PIX, CLS, KEY, //Set a individual pixel, fill the whole pane, key (for using key inputs currently TODO)
    WAIT, HALT //Wait for one frame, Stop the program
};

inline const std::unordered_map<std::string, Op>& OpCodeTable()
{
    static const std::unordered_map<std::string, Op> table = {
        {"NOP",Op::NOP}, {"MOV",Op::MOV}, {"LOAD",Op::LOAD}, {"STORE",Op::STORE},
        {"ADD",Op::ADD}, {"SUB",Op::SUB}, {"MUL",Op::MUL}, {"DIV",Op::DIV}, {"MOD",Op::MOD},
        {"AND",Op::AND}, {"OR",Op::OR}, {"XOR",Op::XOR}, {"NOT",Op::NOT}, {"INC",Op::INC}, {"DEC",Op::DEC},
        {"CMP",Op::CMP}, {"JMP",Op::JMP}, {"JZ",Op::JZ}, {"JNZ",Op::JNZ},
        {"JG",Op::JG}, {"JL",Op::JL}, {"JGE",Op::JGE}, {"JLE",Op::JLE},
        {"PIX",Op::PIX}, {"CLS",Op::CLS}, {"KEY",Op::KEY},
        {"WAIT",Op::WAIT}, {"HALT",Op::HALT}
    };
    return table;
}

inline int OperandCount(Op op)
{
    switch (op) {
    case Op::NOP: case Op::HALT: case Op::WAIT: return 0;
    case Op::NOT: case Op::INC: case Op::DEC: case Op::KEY: case Op::CLS: return 1;
    case Op::MOV: case Op::LOAD: case Op::STORE:
    case Op::ADD: case Op::SUB: case Op::MUL: case Op::DIV: case Op::MOD:
    case Op::AND: case Op::OR: case Op::XOR: case Op::CMP: return 2;
    case Op::PIX: return 3;
    default: return -1;
    }
}

inline bool IsJump(Op op)
{
    switch (op) {
    case Op::JMP: case Op::JZ: case Op::JNZ:
    case Op::JG: case Op::JL: case Op::JGE: case Op::JLE:
        return true;
    default: return false;
    }
}

struct Instruction
{
    Op op = Op::NOP;
    std::vector<Operand> ops;
    int target = -1;
    int sourceLine = 0;
};

#endif //PXL_8_INSTRUCTION_HPP
