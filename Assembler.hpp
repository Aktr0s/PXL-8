//
// Created by Aktr0s on 27.09.2026.
//

#ifndef PXL_8_ASSEMBLER_HPP
#define PXL_8_ASSEMBLER_HPP

#pragma once
#include <vector>
#include <string>
#include "Instruction.hpp"

bool Assemble(const std::vector<std::string>& codeLines, std::vector<Instruction>& outProgram);

#endif //PXL_8_ASSEMBLER_HPP
