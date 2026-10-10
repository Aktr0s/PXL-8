//
// Created by Aktr0s on 27.09.2026.
//

#include "VM.hpp"
#include "Assembler.hpp"
#include "messageBox.h"
#include "raylib.h"

VM vm;
std::vector<Instruction> program;
std::vector<std::string> codeLines;
bool running = false;
int lastExecutedLine = -1;

int GetVal(VM& v, const Operand& op) {
    switch (op.type) {
        case OperandType::IMM: return op.value;
        case OperandType::REG: return v.regs[op.value];
        case OperandType::MEM: return v.memory[op.value & 0xFF];
        default: return 0; // INVALID never reaches here — Assemble() should reject it first
    }
}

void SetVal(VM& v, const Operand& op, int value) {
    if (op.type == OperandType::REG) v.regs[op.value] = value;
    else if (op.type == OperandType::MEM) v.memory[op.value & 0xFF] = (uint8_t)value;
}

void SetFlags(VM& v, int result) {
    v.flagZ = (result == 0);
    v.flagN = (result < 0);
}

static void RuntimeFail(VM& v, const std::string& msg) {
    v.halted = true;
    running = false;
    invokeMessageBox(msg.c_str(), MessageBoxType::Error);
}

StepResult Step(VM& v, const std::vector<Instruction>& prog) {
    if (v.halted) return StepResult::HALT;
    if (v.pc < 0 || v.pc >= (int)prog.size()) { v.halted = true; return StepResult::HALT; }

    const Instruction& ins = prog[v.pc];
    lastExecutedLine = ins.sourceLine;
    v.pc++; // advance first — jumps overwrite this below
    const auto& a = ins.ops;

    switch (ins.op) {
        case Op::NOP: break;

        case Op::MOV:
            if (a[0].type == OperandType::MEM || a[1].type == OperandType::MEM) {
                RuntimeFail(v, "line " + std::to_string(ins.sourceLine) + ": MOV cannot use [mem], use LOAD/STORE");
                return StepResult::HALT;
            }
            SetVal(v, a[0], GetVal(v, a[1]));
            break;

        case Op::LOAD:  SetVal(v, a[0], GetVal(v, a[1])); break;
        case Op::STORE: SetVal(v, a[0], GetVal(v, a[1])); break;

        case Op::ADD: { int r = GetVal(v,a[0]) + GetVal(v,a[1]); SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::SUB: { int r = GetVal(v,a[0]) - GetVal(v,a[1]); SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::MUL: { int r = GetVal(v,a[0]) * GetVal(v,a[1]); SetVal(v,a[0],r); SetFlags(v,r); break; }

        case Op::DIV: {
            int d = GetVal(v,a[1]);
            if (d == 0) { RuntimeFail(v, "line " + std::to_string(ins.sourceLine) + ": division by zero"); return StepResult::HALT; }
            int r = GetVal(v,a[0]) / d; SetVal(v,a[0],r); SetFlags(v,r); break;
        }
        case Op::MOD: {
            int d = GetVal(v,a[1]);
            if (d == 0) { RuntimeFail(v, "line " + std::to_string(ins.sourceLine) + ": division by zero"); return StepResult::HALT; }
            int r = GetVal(v,a[0]) % d; SetVal(v,a[0],r); SetFlags(v,r); break;
        }

        case Op::AND: { int r = GetVal(v,a[0]) & GetVal(v,a[1]); SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::OR:  { int r = GetVal(v,a[0]) | GetVal(v,a[1]); SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::XOR: { int r = GetVal(v,a[0]) ^ GetVal(v,a[1]); SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::NOT: { int r = ~GetVal(v,a[0]); SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::INC: { int r = GetVal(v,a[0]) + 1; SetVal(v,a[0],r); SetFlags(v,r); break; }
        case Op::DEC: { int r = GetVal(v,a[0]) - 1; SetVal(v,a[0],r); SetFlags(v,r); break; }

        case Op::CMP: { int r = GetVal(v,a[0]) - GetVal(v,a[1]); SetFlags(v,r); break; }

        case Op::JMP: v.pc = ins.target; break;
        case Op::JZ:  if (v.flagZ) v.pc = ins.target; break;
        case Op::JNZ: if (!v.flagZ) v.pc = ins.target; break;
        case Op::JG:  if (!v.flagZ && !v.flagN) v.pc = ins.target; break;
        case Op::JL:  if (v.flagN) v.pc = ins.target; break;
        case Op::JGE: if (!v.flagN) v.pc = ins.target; break;
        case Op::JLE: if (v.flagN || v.flagZ) v.pc = ins.target; break;

        case Op::PIX: {
            int x = GetVal(v,a[0]), y = GetVal(v,a[1]), c = GetVal(v,a[2]);
            if (x >= 0 && x < FB_W && y >= 0 && y < FB_H)
                v.framebuffer[y * FB_W + x] = (uint8_t)c;
            break;
        }
        case Op::CLS: {
            uint8_t c = (uint8_t)GetVal(v, a[0]);
            for (auto& px : v.framebuffer) px = c;
            break;
        }
        case Op::KEY: SetVal(v, a[0], v.keyState); break;

        case Op::WAIT: return StepResult::WAIT;
        case Op::HALT: v.halted = true; return StepResult::HALT;
    }

    return StepResult::OK;
}

void LoadCode(const std::vector<std::string>& lines) {
    codeLines = lines;
}

void RunProgram() {
    std::vector<Instruction> newProgram;
    if (!Assemble(codeLines, newProgram)) {
        // Assemble() already showed the message box describing what's wrong.
        ResetProgram();
        return;
    }
    program = std::move(newProgram);
    vm = VM();
    running = true;
}

void StopProgram() { running = false; }

void StepProgram() {
    if (program.empty()) RunProgram();
    if (!program.empty()) Step(vm, program);
}

void ResetProgram() {
    vm = VM();
    running = false;
    lastExecutedLine = -1;
    program.clear();
}

void TickProgram(int instructionsPerFrame) {
    if (!running) return;
    for (int i = 0; i < instructionsPerFrame; i++) {
        StepResult r = Step(vm, program);
        if (r == StepResult::WAIT) break;
        if (r == StepResult::HALT) { running = false; break; }
    }
}

static float tickAccumulator = 0.0f;

void TickProgramAtRate(int ticksPerSecond)
{
    if (!running) {tickAccumulator = 0.0f; return;}
    if (ticksPerSecond < 1) ticksPerSecond = 1;

    float tickInterval = 1.0f / static_cast<float>(ticksPerSecond);
    tickAccumulator += GetFrameTime();

    while (tickAccumulator >= tickInterval) {
        StepResult r = Step(vm, program);
        tickAccumulator -= tickInterval;
        if (r == StepResult::WAIT) break;
        if (r == StepResult::HALT) { running = false; tickAccumulator = 0.0f; break; }
    }
}

int CurrentSourceLine() {
    if (program.empty() || vm.pc < 0 || vm.pc >= (int)program.size())
        return -1;
    return program[vm.pc].sourceLine;
}