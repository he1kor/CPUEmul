#include <iostream>

#include "assembly.h"

#include "assembler.h"

#include "cpu.h"
#include "clock_generator.h"
#include <format>

#include "cpu_state_out.h"
#include "file.h"
#include "data_reader.h"

#include "CLI11.hpp"

ClockGenerator::DisplayMode multiplexDisplayFlags(bool isFPS, bool isResultOnly, bool isEveryStep){
    if (isFPS)
        return ClockGenerator::DisplayMode::FIXED_FPS;
    if (isResultOnly)
        return ClockGenerator::DisplayMode::RESULT;
    if (isEveryStep)
        return ClockGenerator::DisplayMode::EVERY_FRAME;

    //default
    return ClockGenerator::DisplayMode::EVERY_FRAME;
}

int main(int argc, char** argv){

    CLI::App app{"CPU Emulator"};

    CLI::App* runCmd = app.add_subcommand("run", "Run assembly program");

    std::string assemblyPath;
    runCmd->add_option("assembly", assemblyPath, "Assembly program file")
        ->required()
        ->check(CLI::ExistingFile);

    std::optional<std::string> dataPath;
    runCmd->add_option("--datafile", dataPath, "Data file")
        ->check(CLI::ExistingFile);

    double hz = 100000;
    runCmd->add_option<double,unsigned int>("--hz", hz);

    double fps = 0;
    bool isResultOnly = false;
    bool isEveryStep = false;

    auto fps_option = runCmd->add_option<double,unsigned int>("--fps", fps);
    auto result_flag = runCmd->add_flag("--result", isResultOnly);
    auto every_step_flag = runCmd->add_flag("--every-step", isEveryStep);

    fps_option->excludes("--result");
    fps_option->excludes("--every-step");

    result_flag->excludes("--fps");
    result_flag->excludes("--every-step");
    
    every_step_flag->excludes("--fps");
    every_step_flag->excludes("--result");

    bool showStep = false;
    runCmd->add_flag("--show-step,--ss", showStep, "Show CPU simulation step number");
    

    CLI11_PARSE(app, argc, argv);

    if (!runCmd->parsed()) {
        std::cout << "Use 'cpuemul --help' for usage information\n";
        return 1;
    }
    
    bool isFPS = runCmd->count("--fps");

    constexpr uint32_t IMEM_SIZE = 1024;
    constexpr uint32_t DMEM_SIZE = 1024;

    Assembler assembler;

    auto expectedAssemblySource = file::read(assemblyPath);
    if (!expectedAssemblySource){
        std::cerr << file::toStr(expectedAssemblySource.error());
        return 1;
    }

    auto expectedAssembly = assembler.translate(*expectedAssemblySource);
    if (!expectedAssembly){
        std::cerr << Assembler::toStr(expectedAssembly.error());
        return 0;
    }
    
    std::array<uint32_t, DMEM_SIZE> data;
    if (dataPath){
        auto expectedDataSource = file::read(*dataPath);
        if (!expectedDataSource){
            std::cerr << file::toStr(expectedDataSource.error());
            return 1;
        }
        auto expectedData = DataReader::parseData<DMEM_SIZE>(*expectedDataSource);
        if (!expectedData){
            std::cerr << DataReader::toStr(expectedData.error().code);
            return 1;
        }
        data = *expectedData;
    } else{
        data.fill(0);
    }

    

    auto cpu = std::make_shared<CPU<1024, 1024>>();
    cpu->loadIMEM(flashAssembly<IMEM_SIZE, DMEM_SIZE>(*expectedAssembly));
    cpu->loadDMEM(data);
    ClockGenerator clock(hz, fps);
    clock.setDisplayMode(multiplexDisplayFlags(isFPS, isResultOnly, isEveryStep));

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