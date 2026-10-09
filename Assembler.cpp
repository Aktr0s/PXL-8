//
// Created by Aktr0s on 27.09.2026.
//

#include "Assembler.hpp"

#include <functional>

#include "Operand.hpp"
#include "messageBox.h"
#include <sstream>
#include <unordered_map>

namespace
{
    struct RawLine
    {
        std::string label;
        std::string opcode;
        std::vector<std::string> operandStrs;
        int sourceLine = 0;
    };

    std::vector<std::string> SplitOperands(const std::string& rest)
    {
        std::vector<std::string> out;
        std::stringstream ss(rest);
        std::string piece;
        while (std::getline(ss, piece, ',')) {
            auto a = piece.find_first_not_of(" \t");
            auto b = piece.find_last_not_of(" \t");
            if (a != std::string::npos) out.push_back(piece.substr(a, b - a + 1));
        }
        return out;
    }

    RawLine TokenizeLine(std::string text, int lineNo) {
        RawLine r;
        r.sourceLine = lineNo;

        auto semi = text.find(';');
        if (semi != std::string::npos) text = text.substr(0, semi);

        auto start = text.find_first_not_of(" \t");
        if (start == std::string::npos) return r; // blank / comment-only line

        text = text.substr(start);

        auto colon = text.find(':');
        if (colon != std::string::npos) {
            r.label = text.substr(0, colon);
            text = text.substr(colon + 1);
            auto s2 = text.find_first_not_of(" \t");
            if (s2 == std::string::npos) return r; // label alone on its own line
            text = text.substr(s2);
        }

        std::istringstream iss(text);
        iss >> r.opcode;
        std::string rest;
        std::getline(iss, rest);
        r.operandStrs = SplitOperands(rest);
        return r;
    }

    std::string AtLine(int line, const std::string& msg) {
        return "line " + std::to_string(line) + ": " + msg;
    }
}

bool Assemble(const std::vector<std::string>& codeLines, std::vector<Instruction>& outProgram)
{
    if (codeLines.empty())
    {
        invokeMessageBox("No assembly code to run, make sure the file is properly loaded");
        return false;
    }
    outProgram.clear();
    std::vector<RawLine> raw;
    std::unordered_map<std::string, int> labels;

    for (size_t i = 0; i < codeLines.size(); i++) {
        RawLine rl = TokenizeLine(codeLines[i], (int)i + 1);
        if (!rl.label.empty()) {
            if (labels.count(rl.label)) {
                invokeMessageBox(AtLine(rl.sourceLine, "Duplicate label \"" + rl.label + "\"").c_str());
                return false;
            }
            labels[rl.label] = (int)raw.size();
        }
        if (!rl.opcode.empty()) raw.push_back(rl);
    }

    // PASS 2 — resolve opcodes, operands, jump targets. Stop at first problem.
    const auto& table = OpCodeTable();
    for (auto& rl : raw) {
        auto opIt = table.find(rl.opcode);
        if (opIt == table.end()) {
            invokeMessageBox(AtLine(rl.sourceLine, "Unknown instruction \"" + rl.opcode + "\"").c_str());
            return false;
        }

        Instruction ins;
        ins.op = opIt->second;
        ins.sourceLine = rl.sourceLine;

        if (IsJump(ins.op)) {
            if (rl.operandStrs.size() != 1) {
                invokeMessageBox(AtLine(rl.sourceLine, rl.opcode + " expects exactly one label").c_str());
                return false;
            }
            auto labelIt = labels.find(rl.operandStrs[0]);
            if (labelIt == labels.end()) {
                invokeMessageBox(AtLine(rl.sourceLine, "Unknown label \"" + rl.operandStrs[0] + "\"").c_str());
                return false;
            }
            ins.target = labelIt->second;
        } else {
            int expected = OperandCount(ins.op);
            if ((int)rl.operandStrs.size() != expected) {
                invokeMessageBox(AtLine(rl.sourceLine, rl.opcode + " expects " + std::to_string(expected) +
                                  " operand(s), got " + std::to_string(rl.operandStrs.size())).c_str());
                return false;
            }
            for (auto& s : rl.operandStrs) {
                Operand op = ParseOperand(s); // shows its own message box on failure
                if (op.type == OperandType::INVALID) return false;
                ins.ops.push_back(op);
            }
        }

        outProgram.push_back(ins);
    }

    return true;
}