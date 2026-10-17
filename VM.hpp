//
// Created by Aktr0s on 27.09.2026.
//

#ifndef PXL_8_VM_HPP
#define PXL_8_VM_HPP

#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "Instruction.hpp"

constexpr int FB_W = 100;
constexpr int FB_H = 100;
constexpr int MEM_SIZE = 256;

struct VM
{
    int regs[8] = {0};
    uint8_t memory[MEM_SIZE] = {0};
    uint8_t framebuffer[FB_W * FB_H] = {0};
    uint8_t keyState = 0; //bit0 up, 1 down, 2 left, 3 right, 4 space, 5 z, 6 x
    int pc = 0;
    bool flagZ = false;
    bool flagN = false;
    bool halted = false;
};

enum class StepResult {OK, WAIT, HALT};

int GetVal(VM& vm, const Operand& op);
void SetVal(VM& vm, const Operand& op, int value);
void SetFlags(VM& vm, int result);
StepResult Step(VM& vm, const std::vector<Instruction>& program);

extern VM vm;
extern std::vector<Instruction> program;
extern std::vector<std::string> codeLines;
extern bool running;
extern bool AssembleRequired;
extern bool firstBoot;
extern int lastExecutedLine;

void LoadCode(const std::vector<std::string>& lines);
void RunProgram();                          // (start; shows a box and stops on error
void AssembleProgram();                     // re)assemble codeLines and 
void StopProgram();
void AssembleRunButtonPass();
void StepProgram();                         // execute exactly one instruction
void ResetProgram();                        // clear VM state, stop running
void TickProgram(int instructionsPerFrame); // call once per raylib frame while running, unused switched for Instruction Per Second
void TickProgramAtRate(int ticksPerSecond);
int CurrentSourceLine();                    // editor line about to execute, for highlighting; -1 if none

#endif //PXL_8_VM_HPP
