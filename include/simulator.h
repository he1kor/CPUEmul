#pragma once

#include <stdexcept>

class Simulator{
protected:
    void virtual onStep() = 0;
    void virtual onStart() = 0;
    void virtual onStop() = 0;
public:

    enum class State{
        STOPPED,
        RUNNING
    };

    Simulator(){};
    
    size_t getStep() const{return currentStep;};
    State getState() const{return state;};

    void start(){
        if (state == State::RUNNING)
            throw std::runtime_error("The simulation is already running.");
        state = State::RUNNING;
        currentStep = 0;
        onStart();
    }
    void stop(){
        if (state == State::STOPPED)
            throw std::runtime_error("The simulation is already stopped");
        onStop();
        state = State::STOPPED;
    }
    bool step(){
        if (state != State::RUNNING)
            throw std::runtime_error("The simulation is not running");
        onStep();
        if (state == State::STOPPED)
            return false;
        currentStep++;
        return true;
    }
private:
    size_t currentStep = 0;
    State state = State::STOPPED;
};