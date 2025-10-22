#pragma once

#include <memory>
#include <chrono>
#include <functional>
#include "simulator.h"

class ClockGenerator {
public:
    enum class DisplayMode {
        EVERY_FRAME,
        FIXED_FPS,
        RESULT
    };

    ClockGenerator(double simulationFrequencyHz, double displayFrequencyHz = 0);
    
    void setSimulationFrequency(double frequencyHz);
    void setDisplayFrequency(double frequencyHz);
    void setDisplayMode(DisplayMode mode);
    void setSimulator(std::shared_ptr<Simulator> simulatorObj);
    void setDisplayCallback(std::function<void()> callback);

    void start();
    void stop();
    bool tick();
    void run();

    double getSimulationFrequency() const;
    double getDisplayFrequency() const;
    DisplayMode getDisplayMode() const;
    bool isRunning() const;
    std::shared_ptr<Simulator> getSimulator() const;

private:
    void setShouldDisplay(std::chrono::steady_clock::time_point nowTime);

    std::chrono::microseconds simulationPeriod;
    std::chrono::microseconds displayPeriod;
    std::chrono::steady_clock::time_point lastSimulationTick;
    std::chrono::steady_clock::time_point lastDisplayTick;
    std::shared_ptr<Simulator> simulator;
    std::function<void()> displayCallback;
    DisplayMode displayMode = DisplayMode::EVERY_FRAME;
    bool shouldDisplay = false;
};