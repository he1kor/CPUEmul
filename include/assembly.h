#pragma once

#include <cstdint>
#include <array>

#include <format>

#include "cpu.h"

struct Assembly{
    static constexpr uint16_t INSTRUCTIONS_COUNT = 22;
    static constexpr uint16_t VALUE_BITS_COUNT = 10;
    


    uint16_t instructionCode;
    bool isLiteral = false;
    uint16_t value;
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