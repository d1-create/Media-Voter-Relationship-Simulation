#pragma once

#include <string>

struct MediaBiasSimulator;

//Preset Nations
enum class SimulationPresets { 
    UNITED_STATES, 
    UNITED_KINGDOM, 
    GERMANY,
    RUSSIA,
    INDIA
};

struct TerminalInput{
    // helper function to read integers without errors
    int getSafeIntegerInput(const std::string& prompt, int min_val, int max_val);
    //basic startup for simulator
    void outputStartupMessage();
    //get paramters based on SimulationPresets
    void parameterInput(MediaBiasSimulator& sim,SimulationPresets& preset);
};

