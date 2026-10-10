//
// Created by Aktr0s on 27.09.2026.
//

#include "Operand.hpp"
#include "messageBox.h"
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstdint>

static bool IsNumberLiteral(const std::string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-') ? 1 : 0;
    if (i >= s.size()) return false;
    if (s.size() > i + 1 && s[i] == '0' && (s[i+1] == 'x' || s[i+1] == 'X')) {
        for (size_t j = i + 2; j < s.size(); j++)
            if (!isxdigit((unsigned char)s[j])) return false;
        return s.size() > i + 2;
    }
    for (size_t j = i; j < s.size(); j++)
        if (!isdigit((unsigned char)s[j])) return false;
    return true;
}

static bool ParseNumberLiteral(const std::string& s, int& out) {
    errno = 0;
    char* end = nullptr;
    long value = std::strtol(s.c_str(), &end, 0);
    if (end == s.c_str() || *end != '\0') return false;
    if (errno == ERANGE || value > INT32_MAX || value < INT32_MIN) return false;
    out = (int)value;
    return true;
}

Operand ParseOperand(const std::string& token) {
    if (!token.empty() && token[0] == '#') {
        std::string num = token.substr(1);
        int value;
        if (!IsNumberLiteral(num) || !ParseNumberLiteral(num, value)) {
            invokeMessageBox(("Bad immediate \"" + token + "\"").c_str(),MessageBoxType::Debug);
            return { OperandType::INVALID };
        }
        return { OperandType::IMM, value };
    }

    if (token.size() >= 2 && token.front() == '[' && token.back() == ']') {
        std::string inner = token.substr(1, token.size() - 2);
        int value;
        if (!IsNumberLiteral(inner) || !ParseNumberLiteral(inner, value)) {
            invokeMessageBox(("Bad memory address \"" + token + "\"").c_str(),MessageBoxType::Debug);
            return { OperandType::INVALID };
        }
        return { OperandType::MEM, value };
    }

    if (token.size() == 2 && (token[0] == 'r' || token[0] == 'R') && isdigit((unsigned char)token[1])) {
        int idx = token[1] - '0';
        if (idx < 0 || idx > 7) {
            invokeMessageBox(("Unknown register \"" + token + "\"").c_str(), MessageBoxType::Debug);
            return { OperandType::INVALID };
        }
        return { OperandType::REG, idx };
    }

    int value;
    if (IsNumberLiteral(token) && ParseNumberLiteral(token, value))
        return { OperandType::IMM, value };

    invokeMessageBox(("Cannot parse operand \"" + token + "\"").c_str(), MessageBoxType::Debug);
    return { OperandType::INVALID };
}