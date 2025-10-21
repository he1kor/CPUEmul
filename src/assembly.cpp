#include "assembly.h"


std::string Assembly::toString() const{
    static const std::unordered_map<uint16_t, std::string_view> codeToMnemonic = {
            {Asm::NOP, "NOP"},
            {Asm::LOAD, "LOAD"},
            {Asm::STORE, "STORE"},
            {Asm::LOADI, "LOADI"},
            {Asm::ADD, "ADD"},
            {Asm::SUB, "SUB"},
            {Asm::INC, "INC"},
            {Asm::DEC, "DEC"},
            {Asm::AND, "AND"},
            {Asm::OR, "OR"},
            {Asm::XOR, "XOR"},
            {Asm::NOT, "NOT"},
            {Asm::SHL, "SHL"},
            {Asm::SHR, "SHR"},
            {Asm::JMP, "JMP"},
            {Asm::JZ, "JZ"},
            {Asm::JNZ, "JNZ"},
            {Asm::JC, "JC"},
            {Asm::JNC, "JNC"},
            {Asm::HLT, "HLT"}
        };

        std::string result;
        auto it = codeToMnemonic.find(instructionCode);
        if (it != codeToMnemonic.end()) {
            result = it->second;
        } else {
            result = "UNK(" + std::to_string(instructionCode) + ")";
        }


        if (instructionCode != Asm::NOP && instructionCode != Asm::HLT) {
            
            result += " ";
            if (isLiteral) {
                result += "*";
            }
            result += std::to_string(value);
        }

        return result;
}
