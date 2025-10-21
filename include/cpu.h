#pragma once

#include <cstdint>
#include <array>
#include <functional>

#include "simulator.h"  

namespace Asm {
    constexpr uint16_t NOP = 0x00;

    constexpr uint16_t LOAD = 0x01;
    constexpr uint16_t STORE = 0x02;
    constexpr uint16_t LOADI = 0x03;
    
    constexpr uint16_t ADD = 0x04;
    constexpr uint16_t SUB = 0x05;
    constexpr uint16_t INC = 0x06;
    constexpr uint16_t DEC = 0x07;

    constexpr uint16_t AND = 0x08;
    constexpr uint16_t OR = 0x09;
    constexpr uint16_t XOR = 0x0A;
    constexpr uint16_t NOT = 0x0B;
    constexpr uint16_t SHL = 0x0C;
    constexpr uint16_t SHR = 0x0D;

    constexpr uint16_t JMP = 0x0E;
    constexpr uint16_t JZ = 0x0F;
    constexpr uint16_t JNZ = 0x10;
    constexpr uint16_t JC = 0x11;
    constexpr uint16_t JNC = 0x12;

    constexpr uint16_t HLT = 0x13;

    constexpr bool n = 0;
    constexpr bool l = true;
    constexpr bool p = false;
}

template<uint16_t IMEM_SIZE = 1024, uint16_t DMEM_SIZE = 1024>
class CPU : public Simulator{

public:
    union Instruction {
        struct {
            uint16_t code : 5;
            uint16_t isLiteral : 1;
            uint16_t value : 10;
        } fields;
        
        uint16_t raw;
    };

    CPU(){};

    uint32_t getPC() const{return PC;};
    uint32_t getACC() const{return ACC;};
    bool getZ() const{return Z;};
    bool getC() const{return C;};
    Instruction getIR() const{return IR;};

    void loadDMEM(const std::array<uint32_t, IMEM_SIZE>& DMEM){
        if (getState() != State::STOPPED)
            throw std::runtime_error("The CPU is already running");
        this->DMEM = DMEM;
    }
    void loadIMEM(const std::array<Instruction, DMEM_SIZE>& IMEM){
        if (getState() != State::STOPPED)
            throw std::runtime_error("The CPU is already running");
        this->IMEM = IMEM;
    }
    
private:

    std::array<Instruction, IMEM_SIZE> IMEM{0};
    std::array<uint32_t, DMEM_SIZE> DMEM{0};

    uint32_t PC = 0;
    uint32_t ACC = 0;

    bool Z = 0;
    bool C = 0;

    Instruction IR = {0};




    void onStart(){};
    void onStep() override{
        IR = IMEM[PC];
        execute();
        ++PC;
    };
    void onStop(){};

    void execute(){
        instructions[IR.fields.code]();
    }

    uint32_t getOperand(){
        return IR.fields.isLiteral ? IR.fields.value : DMEM[IR.fields.value];
    }

    void setAcc(uint32_t ACC){
        this->ACC = ACC;
        Z = (ACC == 0);
    }
    

    void NOP(){
    }


    void LOAD() {
        setAcc(getOperand());
    }
    void STORE() {
        DMEM[getOperand()] = ACC;
    }
    void LOADI(){
        setAcc(DMEM[getOperand()]);
    }


    void ADD() {
        uint32_t result = ACC + getOperand();
        C = (result < ACC);
        setAcc(result);
    }
    void SUB() {
        uint32_t result = ACC - getOperand();
        C = (result > ACC);
        setAcc(result);
    }
    void INC(){
        uint32_t result = ACC + 1;
        C = (result < ACC);
        setAcc(result);
    }
    void DEC(){
        uint32_t result = ACC - 1;
        C = (result > ACC);
        setAcc(result);
    }


    void AND(){
        setAcc(ACC & getOperand());
    }
    void OR(){
        setAcc(ACC | getOperand());
    }
    void XOR(){
        setAcc(ACC ^ getOperand());
    }
    void NOT(){
        setAcc(!ACC);
    }
    void SHL(){
        uint8_t shift_count = getOperand() & 0b00011111;
        if (shift_count > 0) {
            C = (ACC >> (32 - shift_count)) & 1;
        } else {
            C = 0;
        }
        setAcc(ACC << shift_count);
    }
    void SHR(){
        uint8_t shift_count = getOperand() & 0b00011111;
        if (shift_count > 0) {
            C = (ACC >> (shift_count - 1)) & 1;
        } else {
            C = 0;
        }
        setAcc(ACC >> shift_count);
    }


    void JMP(){
        PC = getOperand() - 1;
    }
    void JZ(){
        if (Z){
            JMP();
        }
    }
    void JNZ(){
        if (!Z){
            JMP();
        }
    }

    void JC(){
        if (!C){
            JMP();
        }
    }
    void JNC(){
        if (!C){
            JMP();
        }
    }

    void HLT(){
        stop();
    }

    const std::function<void()> instructions[20] {
        [Asm::NOP] = std::bind(&CPU::NOP, this),
        [Asm::LOAD] = std::bind(&CPU::LOAD, this),
        [Asm::STORE] = std::bind(&CPU::STORE, this),
        [Asm::LOADI] = std::bind(&CPU::LOADI, this),

        [Asm::ADD] = std::bind(&CPU::ADD, this),
        [Asm::SUB] = std::bind(&CPU::SUB, this),
        [Asm::INC] = std::bind(&CPU::INC, this),
        [Asm::DEC] = std::bind(&CPU::DEC, this),

        [Asm::AND] = std::bind(&CPU::AND, this),
        [Asm::OR] = std::bind(&CPU::OR, this),
        [Asm::XOR] = std::bind(&CPU::XOR, this),
        [Asm::NOT] = std::bind(&CPU::NOT, this),
        
        [Asm::SHL] = std::bind(&CPU::SHL, this),
        [Asm::SHR] = std::bind(&CPU::SHR, this),

        [Asm::JMP] = std::bind(&CPU::JMP, this),
        [Asm::JZ] = std::bind(&CPU::JZ, this),
        [Asm::JNZ] = std::bind(&CPU::JNZ, this),
        [Asm::JC] = std::bind(&CPU::JC, this),
        [Asm::JNC] = std::bind(&CPU::JNC, this),

        [Asm::HLT] = std::bind(&CPU::HLT, this),
    };
};