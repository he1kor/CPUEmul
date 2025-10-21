#include <iostream>

#include "assembly.h"

#include "assembler.h"

#include "cpu.h"
#include "clock_generator.h"
#include <format>

#include "cpu_state_out.h"

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

    auto cpu = std::make_shared<CPU<1024, 1024>>();
    cpu->loadIMEM(flashAssembly<IMEM_SIZE, DMEM_SIZE>(**fileResult));
    cpu->loadDMEM(DMEM);
    ClockGenerator clock(1.0, 10);

    clock.setSimulator(cpu);
    
    coutCPU::displaySimulationStep = true;
    coutCPU::logTableHeader();
    clock.setDisplayCallback([&cpu, &clock]() {
        coutCPU::logTableRow(*cpu);
        cpu->getStep();
    });
    
    clock.run();
    coutCPU::logTableFooter();
}