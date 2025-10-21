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
    auto result = assembler.translate(
     R"(
        LOAD 0 #Sum
        STORE 0

        LOAD 100 #Ptr + i
        STORE 1
        
            LOAD *100
            JZ 15
            DEC
            STORE 100

            LOAD *1
            INC
            STORE 1
            
            LOADI *1
            ADD *0 #Add loaded *(PTR+i) to sum
            STORE 0
            JMP 4
        
        LOAD *0
        HLT
    )");
    if (!result)
        std::cout << Assembler::toStr(result.error());

    
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
    cpu.loadIMEM(flashAssembly<IMEM_SIZE, DMEM_SIZE>(*result));
    cpu.loadDMEM(DMEM);


    cpu.start();
    do {
        std::cout << "ACC " << cpu.getACC() << "\tPC ";
        std::cout << cpu.getPC() << "\n";
    } while ((cpu.step()));

    std::cout << "\n---FINSH---\n";
}