#include <string>
#include <iostream>
#include <vector>

#include "TerminalInput.hpp"
#include "MediaBiasSimulation.hpp"

int TerminalInput::getSafeIntegerInput(const std::string& prompt, int min_val, int max_val) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min_val && value <= max_val) {
            return value;
        }
        // reask if wrong type is entered
        std::cout << "Invalid input. Please enter a number between " << min_val << " and " << max_val << ".\n";
        std::cin.clear(); // clear errors
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // any erroneous types are destroyed
    }
}

void TerminalInput::outputStartupMessage(){
    std::cout << "========================================================================\n";
    std::cout << "                     AGENCY-BASED MEDIA SIMULATION                      \n";          
    std::cout << "========================================================================\n\n";    
}

void TerminalInput::parameterInput(MediaBiasSimulator& sim,SimulationPresets& preset){
    std::cout << "Options:\n";
    std::cout << "1. United States  [Polarised Free Market]\n";
    std::cout << "2. United Kingdom [Regulated Media]\n";
    std::cout << "3. Germany        [High Institutional Trust]\n";
    std::cout << "4. Russia         [State Control with State Propaganda]\n";
    std::cout << "5. India          [Hyper-Competitive Media]\n";
    int nation_chosen = getSafeIntegerInput("Enter the nation you would like to simulate the voter-media relationship of (1-5): ", 1, 5);

    switch(nation_chosen){
        case 1:
            preset = SimulationPresets::UNITED_STATES;
            sim.voter_start_randomgen = -0.85f;    // Highly dispersed initial public
            sim.voter_end_randomgen = 0.85f;
            sim.voter_tolerance = 0.20f;           // Tight echo chambers / high confirmation bias
            sim.learning_rate = 0.015f;            // Rapid individual mind shifts
            sim.minimum_share_percentage = 0.28f;  // Ruthless market survival bar
            sim.radicalisation_rate = 0.04f;       // Aggressive, fast corporate polarization
            sim.state_media_exists = false;        // Pure commercial landscape
            break;
        case 2:
            preset = SimulationPresets::UNITED_KINGDOM;
            sim.voter_start_randomgen = -0.65f;    // Moderate initial distribution
            sim.voter_end_randomgen = 0.65f;
            sim.voter_tolerance = 0.38f;           // Open-minded consumers / broad viewing habits
            sim.learning_rate = 0.006f;            // Measured individual ideological drift
            sim.minimum_share_percentage = 0.12f;  // Safe, regulated corporate survival bar
            sim.radicalisation_rate = 0.018f;      // Gradual editorial positioning
            sim.state_media_exists = true;         // Subsidized central anchor active
            break;
        case 3:
            preset = SimulationPresets::GERMANY;
            sim.voter_start_randomgen = -0.50f;    // Heavily centralized initial public
            sim.voter_end_randomgen = 0.50f;
            sim.voter_tolerance = 0.45f;           // Maximum broad-mindedness / massive public trust
            sim.learning_rate = 0.003f;            // Extremely steady, analytical voter base
            sim.minimum_share_percentage = 0.08f;  // Highly protective/supportive market floor
            sim.radicalisation_rate = 0.010f;      // Very stable, slow-moving corporate stances
            sim.state_media_exists = true;         // Dominant centrist state broadcasters
            break;
        case 4:
            preset = SimulationPresets::RUSSIA;
            sim.voter_start_randomgen = 0.10f;     // Populace pre-skewed/restricted heavily to the state narrative
            sim.voter_end_randomgen = 0.85f;
            sim.voter_tolerance = 0.22f;           // Restricted informational variance
            sim.learning_rate = 0.025f;            // High susceptibility due to media saturation
            sim.minimum_share_percentage = 0.35f;  // Private outlets face massive existential pressure
            sim.radicalisation_rate = 0.050f;      // Outlets must aggressively align or polarize to survive
            sim.state_media_exists = true;         // State media exists as an active propaganda arm
            break;
        case 5:
            preset = SimulationPresets::INDIA;
            sim.voter_start_randomgen = -0.70f;    
            sim.voter_end_randomgen = 0.70f;
            sim.voter_tolerance = 0.25f;           // High polarization among TV news audiences
            sim.learning_rate = 0.012f;            // Fast-paced media consumption habits
            sim.minimum_share_percentage = 0.32f;  // Intense market crowding requires a high audience share
            sim.radicalisation_rate = 0.045f;      // High-turnover, sensationalist polarization speed
            sim.state_media_exists = false;        // Public broadcaster exists but is market-bypassed
            break;
    }
}
