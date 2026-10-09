//
// Created by Aktr0s on 27.09.2026.
//

#ifndef PXL_8_OPERAND_HPP
#define PXL_8_OPERAND_HPP

#pragma once
#include <string>
#include <stdexcept>

enum class OperandType {REG, IMM, MEM, INVALID};

struct Operand
{
    OperandType type = OperandType::INVALID;
    int value;
};

Operand ParseOperand(const std::string& token);

#endif //PXL_8_OPERAND_HPP
