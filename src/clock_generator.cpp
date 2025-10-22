#include "clock_generator.h"
#include <thread>
#include <stdexcept>

ClockGenerator::ClockGenerator(double simulationFrequencyHz, double displayFrequencyHz) {
    setSimulationFrequency(simulationFrequencyHz);
    setDisplayFrequency(displayFrequencyHz);
}

void ClockGenerator::setSimulationFrequency(double frequencyHz) {
    if (frequencyHz <= 0) throw std::invalid_argument("Frequency must be positive");
    auto periodUs = static_cast<int64_t>(1000000.0 / frequencyHz);
    simulationPeriod = std::chrono::microseconds(periodUs);
}

void ClockGenerator::setDisplayFrequency(double frequencyHz) {
    if (frequencyHz == 0) {
        displayMode = DisplayMode::EVERY_FRAME;
    } else if (frequencyHz > 0) {
        displayMode = DisplayMode::FIXED_FPS;
        auto periodUs = static_cast<int64_t>(1000000.0 / frequencyHz);
        displayPeriod = std::chrono::microseconds(periodUs);
    } else {
        throw std::invalid_argument("Frequency must be non-negative");
    }
}

void ClockGenerator::setShouldDisplay(std::chrono::steady_clock::time_point nowTime) {
    shouldDisplay = false;
    
    switch (displayMode) {
        case DisplayMode::EVERY_FRAME:
            shouldDisplay = true;
            break;
            
        case DisplayMode::FIXED_FPS: {
            auto displayElapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                nowTime - lastDisplayTick);
            shouldDisplay = (displayElapsed >= displayPeriod);
            break;
        }
            
        case DisplayMode::RESULT:
            shouldDisplay = false;
            break;
    }
}

void ClockGenerator::setDisplayMode(DisplayMode mode) {
    displayMode = mode;
}

void ClockGenerator::setSimulator(std::shared_ptr<Simulator> simulatorObj) {
    simulator = simulatorObj;
}

void ClockGenerator::setDisplayCallback(std::function<void()> callback) {
    displayCallback = callback;
}

void ClockGenerator::start() {
    if (!simulator) {
        throw std::runtime_error("No simulator set");
    }
    simulator->start();
    lastSimulationTick = std::chrono::steady_clock::now();
    lastDisplayTick = lastSimulationTick;
}

void ClockGenerator::stop() {
    if (simulator) {
        simulator->stop();
    }
}

bool ClockGenerator::tick() {
    if (!simulator || simulator->getState() != Simulator::State::RUNNING) {
        return false;
    }

    auto now = std::chrono::steady_clock::now();
    
    setShouldDisplay(now);
    
    if (shouldDisplay && displayCallback) {
        lastDisplayTick = now;
        displayCallback();
    }
    
    auto simulationElapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        now - lastSimulationTick);
    
    if (simulationElapsed >= simulationPeriod) {
        lastSimulationTick = now;
        return simulator->step();
    } else {
        auto simulationRemaining = simulationPeriod - simulationElapsed;
        if (simulationRemaining > std::chrono::microseconds(0)) {
            std::this_thread::sleep_for(simulationRemaining);
            lastSimulationTick = std::chrono::steady_clock::now();
            return simulator->step();
        }
    }
    
    return true;
}

void ClockGenerator::run() {
    start();
    
    while (tick()) {}
    
    if (displayCallback) {
        displayCallback();
    }
}

double ClockGenerator::getSimulationFrequency() const {
    return 1000000.0 / simulationPeriod.count();
}

double ClockGenerator::getDisplayFrequency() const {
    if (displayMode == DisplayMode::EVERY_FRAME) {
        return getSimulationFrequency();
    } else {
        return 1000000.0 / displayPeriod.count();
    }
}

ClockGenerator::DisplayMode ClockGenerator::getDisplayMode() const {
    return displayMode;
}

bool ClockGenerator::isRunning() const {
    return simulator && simulator->getState() == Simulator::State::RUNNING;
}

std::shared_ptr<Simulator> ClockGenerator::getSimulator() const {
    return simulator;
}