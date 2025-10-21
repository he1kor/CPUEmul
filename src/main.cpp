#include <iostream>
#include <windows.h>

#include "assembly.h"

#include "assembler.h"

#include "cpu.h"
#include <format>

template<uint16_t IMEM_SIZE, uint16_t DMEM_SIZE>
void coutCPUState(const CPU<IMEM_SIZE, DMEM_SIZE>& cpu){
    const int col1 = 20;
    const int col2 = 15;
    
    std::cout << "==========================================" << std::endl;
    std::cout << "|                CPU STATE               |" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    std::cout << "| " << std::left << std::setw(col1) << "ACC" 
              << " | " << std::right << std::setw(col2) << cpu.getACC() << " |" << std::endl;
    
    std::cout << "| " << std::left << std::setw(col1) << "Program Counter" 
              << " | " << std::right << std::setw(col2) << cpu.getPC() << " |" << std::endl;
    
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

int main(){

    constexpr uint32_t IMEM_SIZE = 1024;
    constexpr uint32_t DMEM_SIZE = 1024;

    Assembler assembler;

    auto fileResult = assembler.translateFile("test.txt");
    if (!fileResult){
        std::cerr << Assembler::toStr(fileResult.error());
        return 0;
    }
    if (!(*fileResult)){
        std::cerr << Assembler::toStr(fileResult->error());
        return 0;
    }
    
    std::array<uint32_t, DMEM_SIZE> DMEM;

    DMEM[100] = 7;

    DMEM[101] = 10;
    DMEM[102] = 20;
    DMEM[103] = 100;
    DMEM[104] = 5;
    DMEM[105] = 1;
    DMEM[106] = 5;
    DMEM[107] = 5;

    
    CPU cpu;
    cpu.loadIMEM(flashAssembly<IMEM_SIZE, DMEM_SIZE>(**fileResult));
    cpu.loadDMEM(DMEM);


    cpu.start();
    do {
        coutCPUState(cpu);
    } while ((cpu.step()));

    std::cout << "\n---FINSH---\n";
}