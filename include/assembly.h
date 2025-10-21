#pragma once

#include <cstdint>
#include <array>

#include <format>
#include <string>

#include "cpu.h"

struct Assembly{
    static constexpr uint16_t INSTRUCTIONS_COUNT = 22;
    static constexpr uint16_t VALUE_BITS_COUNT = 10;

    uint16_t instructionCode = 0;
    bool isLiteral = false;
    uint16_t value = 0;

    Assembly() = default;
    Assembly(const CPU<>::Instruction& instr) {
        instructionCode = instr.fields.code;
        isLiteral = instr.fields.isLiteral;
        value = instr.fields.value;
    }
    std::string toString() const;
};

template <uint16_t IMEM_SIZE, uint16_t DMEM_SIZE>
std::array<CPU<>::Instruction, IMEM_SIZE> flashAssembly(const std::vector<Assembly>& assembly){
    std::array<CPU<>::Instruction, IMEM_SIZE> result;

    size_t i = 0;
    for (const Assembly& instruction : assembly){
        if (instruction.instructionCode > Assembly::INSTRUCTIONS_COUNT)
            throw std::runtime_error(std::format("Bad instruction given: {}", instruction.instructionCode));
        if (instruction.value >> Assembly::VALUE_BITS_COUNT != 0)
            throw std::runtime_error(std::format("Bad value given: {}", instruction.value));

        result[i].fields.isLiteral = instruction.isLiteral;
        result[i].fields.value = instruction.value;
        result[i].fields.code = instruction.instructionCode;
        ++i;
    }
    return result;
};