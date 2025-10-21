#pragma once

#include <iostream>

#include "assembly.h"

#include "cpu.h"
#include <format>
#include "string_view"
#include <iomanip>

#pragma once

#include <iostream>
#include "assembly.h"
#include "cpu.h"
#include <format>
#include "string_view"
#include <iomanip>

namespace coutCPU{
    static bool displaySimulationStep = false;

    void logLong(const CPU<>& cpu){
        const int col1 = 20;
        const int col2 = 15;
        
        if (displaySimulationStep) {
            std::cout << "Step: " << cpu.getStep() << std::endl;
        }
        
        std::cout << "==========================================" << std::endl;
        std::cout << "|                CPU STATE               |" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        std::cout << "| " << std::left << std::setw(col1) << "ACC" 
                << " | " << std::right << std::setw(col2) << cpu.getACC() << " |" << std::endl;
        
        std::cout << "| " << std::left << std::setw(col1) << "Program Counter" 
                << " | " << std::right << std::setw(col2) << cpu.getPC() << " |" << std::endl;
        
        if (displaySimulationStep) {
            std::cout << "| " << std::left << std::setw(col1) << "Simulation Step" 
                    << " | " << std::right << std::setw(col2) << cpu.getStep() << " |" << std::endl;
        }
        
        CPU<>::Instruction ir = cpu.getIR();
        Assembly line(ir);
        
        std::cout << "|----------------------------------------|" << std::endl;
        std::cout << "|               INSTRUCTION              |" << std::endl;
        std::cout << "|----------------------------------------|" << std::endl;
        
        std::cout << "| " << std::left << std::setw(col1) << "Assembly" 
                << " | " << std::right << std::setw(col2) << line.toString() << " |" << std::endl;
        
        std::stringstream hexStream;
        hexStream << "0x" << std::hex << std::setw(4) << std::setfill('0') << ir.raw;
        std::string hexStr = hexStream.str();
        std::cout << "| " << std::left << std::setw(col1) << "Hex" 
                << " | " << std::right << std::setw(col2) << hexStr << " |" << std::endl;
        
        std::cout << "| " << std::left << std::setw(col1) << "Decimal" 
                << " | " << std::right << std::setw(col2) << std::dec << ir.raw << " |" << std::endl;
        
        std::cout << "|----------------------------------------|" << std::endl;
        
        std::string zeroFlagStr = cpu.getZ() ? "SET" : "CLEAR";
        std::cout << "| " << std::left << std::setw(col1) << "Zero Flag" 
                << " | " << std::right << std::setw(col2) << zeroFlagStr << " |" << std::endl;
        
        std::string carryFlagStr = cpu.getC() ? "SET" : "CLEAR";
        std::cout << "| " << std::left << std::setw(col1) << "Carry Flag" 
                << " | " << std::right << std::setw(col2) << carryFlagStr << " |" << std::endl;
        
        std::cout << "==========================================" << std::endl;
    }

    void logShort(const CPU<>& cpu){
        CPU<>::Instruction ir = cpu.getIR();
        Assembly line(ir);
        
        std::string assemblyStr = line.toString();
        
        size_t spacePos = assemblyStr.find(' ');
        std::string mnemonic, operand;
        
        if (spacePos != std::string::npos) {
            mnemonic = assemblyStr.substr(0, spacePos);
            operand = assemblyStr.substr(spacePos + 1);
        } else {
            mnemonic = assemblyStr;
            operand = "";
        }
        
        if (displaySimulationStep) {
            std::cout << "[" << std::setw(3) << cpu.getStep() << "] ";
        }
        
        std::cout << "[" << std::setw(3) << cpu.getPC() << "] |   "
                << std::left << std::setw(6) << mnemonic
                << std::right << std::setw(5) << operand
                << "   |" << std::setw(10) << cpu.getACC()
                << "  " << (cpu.getZ() ? 'Z' : '-') << (cpu.getC() ? 'C' : '-')
                << std::endl;
}

    void logTableHeader(){
        if (displaySimulationStep) {
            std::cout << "+------+------+-------------+--------+-------+" << std::endl;
            std::cout << "| Step |  PC  | Instruction |  ACC   | Flags |" << std::endl;
            std::cout << "+------+------+-------------+--------+-------+" << std::endl;
        } else {
            std::cout << "+------+-------------+--------+-------+" << std::endl;
            std::cout << "|  PC  | Instruction |  ACC   | Flags |" << std::endl;
            std::cout << "+------+-------------+--------+-------+" << std::endl;
        }
    }

    void logTableRow(const CPU<>& cpu){
        CPU<>::Instruction ir = cpu.getIR();
        Assembly line(ir);
        
        std::string assemblyStr = line.toString();
        
        size_t spacePos = assemblyStr.find(' ');
        std::string mnemonic, operand;
        
        if (spacePos != std::string::npos) {
            mnemonic = assemblyStr.substr(0, spacePos);
            operand = assemblyStr.substr(spacePos + 1);
        } else {
            mnemonic = assemblyStr;
            operand = "";
        }
        
        if (displaySimulationStep) {
            std::cout << "| " << std::setw(4) << cpu.getStep() << " | "
                    << std::setw(4) << cpu.getPC() << " | "
                    << std::left << std::setw(6) << mnemonic
                    << std::right << std::setw(5) << operand
                    << " | " << std::setw(6) << cpu.getACC() << " |   "
                    << (cpu.getZ() ? 'Z' : ' ') << (cpu.getC() ? 'C' : ' ') << "  |"
                    << std::endl;
        } else {
            std::cout << "| " << std::setw(4) << cpu.getPC() << " | "
                    << std::left << std::setw(6) << mnemonic
                    << std::right << std::setw(5) << operand
                    << " | " << std::setw(6) << cpu.getACC() << " |   "
                    << (cpu.getZ() ? 'Z' : ' ') << (cpu.getC() ? 'C' : ' ') << "  |"
                    << std::endl;
        }
    }

    void logTableFooter(){
        if (displaySimulationStep) {
            std::cout << "+------+------+-------------+--------+-------+" << std::endl;
        } else {
            std::cout << "+------+-------------+--------+-------+" << std::endl;
        }
    }
}