#include <iostream>
#include <windows.h>

#include "assembly.h"

#include "assembler.h"

#include "cpu.h"
#include <format>

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
        std::cout << "ACC " << cpu.getACC() << "\tPC ";
        std::cout << cpu.getPC() << "\n";
    } while ((cpu.step()));

    std::cout << "\n---FINSH---\n";
}