//NOTE:
//This program is a quick and somewhat inaccurate simulation of voters viewing media since it relies on various assumptions.
//However, it servers as a fun mathematical simulation and generalisation of political polarisation  
//(It assumes media goes to the extremes over the center and doesn't truly simulate business activity)
//Each turn = day

#include <iostream> // Redundant import for small-scale testing
#include <fstream>  // Required for file writing
#include <vector>   // Import vector for dynamic allocation
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip>  //Custom Graphs
#include <random>   // Required header

#include <iostream> //Optional/testing header

#include "MediaBiasSimulation.hpp"

// helper function to read +ve integers without errors
int getSafeIntegerInput(const std::string& prompt, int min_val, int max_val) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min_val && value <= max_val) {
            return value;
        }
        // reask if wrong type is entered
        std::cout << "Invalid input. Please enter a number between " << min_val << " and " << max_val << ".\n";
        std::cin.clear(); // clear errors
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //any bad types are destroyed
    }
}

//Below are two same functions but one is for state media (no greed) and one is for private media (radicalise function triggers greed)
void Voter::watchMediaOutlet(float voter_tolerance, float learning_rate, MediaOutlet& outlet){
    if(outlet.bias>=(bias-voter_tolerance) && outlet.bias<=(bias+voter_tolerance)){
        float change = learning_rate*(outlet.bias-bias);//change in bias
        bias+=change;
        outlet.total_influence += 1.0f;
        outlet.turn_influence += 1.0f;
    }
}

void PrivateMediaOutlet::radicaliseOutlets(MediaBiasSimulator& sim){
    // Get Market Average
    float market_average = static_cast<float>(sim.voters.size()) / sim.outlets.size();

    // SURVIVAL: Business needs viewers
    if(turn_influence<sim.current_influence_threshold){
        consecutive_days_starving++;
        //calculate how desparate the media outlet needs influence (acting as profit)
        float influence_deficit = sim.current_influence_threshold - turn_influence;
        float desperation = influence_deficit / sim.current_influence_threshold;
        if (desperation > 1.0f) desperation = 1.0f;
        if (desperation < 0.0f) desperation = 0.0f;
        //calculate the rate of change of polarisation
        float bias_delta = desperation * sim.radicalisation_rate;
        if (bias == 0.0f) {
            //randomly choose direction
            std::uniform_int_distribution<int> coin_flip(0, 1);
            bias = (coin_flip(sim.random_number_generator) == 0) ? -0.01f : 0.01f;
        }
        else if(bias<=0.0){
            bias-=bias_delta; //go more to left
            if (bias < sim.voter_start_randomgen) { //error checker
                bias = sim.start_spectrum;
            }
        }
        else{
            bias += bias_delta; //go more to right
            if (bias > sim.end_spectrum) { //error checker
                bias = sim.end_spectrum;
            }
        }
    }
    //Can survive, wants profit?
    else if(turn_influence<market_average){
        consecutive_days_starving=0;
        //calculate how desparate the media outlet needs influence (acting as profit)
        float influence_deficit = market_average - turn_influence;
        float desperation = influence_deficit / market_average;
        if (desperation > 1.0f) desperation = 1.0f;
        if (desperation < 0.0f) desperation = 0.0f;
        //calculate the rate of change of polarisation
        float bias_delta = desperation * (sim.radicalisation_rate * 0.5f);
        //greedy moves at half the rate (0.5f)
        if (bias == 0.0f) {
            std::uniform_int_distribution<int> coin_flip(0, 1);
            bias = (coin_flip(sim.random_number_generator) == 0) ? -0.01f : 0.01f;
        }
        //move according to market need of average voter bias
        else if(bias<sim.average_voter_bias){
            bias+=bias_delta;
            if (bias > sim.average_voter_bias) {
                bias = sim.average_voter_bias; // check if overdone
            }
        }
        else{
            bias-=bias_delta;
            if (bias < sim.average_voter_bias) {
                bias = sim.average_voter_bias; // check if overdone
            }
        }
    }
    //Stability
    else{
        consecutive_days_starving=0;
    }

    turn_influence=0.0f;
}

//Redundant Functions (Use only if necessary or small scale)
void Voter::OutputData(){
    std::cout<< "Voter " << id << " has a bias of " << bias;
    std::cout<<"\n";
}
void MediaOutlet::OutputData(){ 
    std::cout<< "Media outlet " << id << " has a bias of " << bias << " and have accumulated " << total_influence << " influence";
    std::cout<<"\n";
}

float MediaBiasSimulator::getRandomSpectrum() {
    //get random number
    std::uniform_real_distribution<float> distr(voter_start_randomgen, voter_end_randomgen);
    return distr(random_number_generator);
}

MediaBiasSimulator::MediaBiasSimulator(int total_voters, int total_outlets){
    //wipe save data
    std::ofstream wipe_old_log_file("simulation_log.txt", std::ios::trunc);
    wipe_old_log_file.close(); 

    //generate random number device
    std::random_device rd;
    random_number_generator.seed(rd());

    //generate voter data and outlet data randomly
    voters.reserve(total_voters);
    for(int i=0;i<total_voters;i++){
        Voter v;
        v.bias = getRandomSpectrum();
        v.id = i;
        voters.push_back(v);
    }
    outlets.reserve(total_outlets+1); //state media so +1
    for(int i = 0; i < total_outlets; i++){
        auto m = std::make_unique<PrivateMediaOutlet>(); 
        m->bias = getRandomSpectrum();
        m->id = i;
        outlets.push_back(std::move(m)); // Move ownership directly into the vector array
    }
    if(state_media_exists){
        auto s = std::make_unique<StateMediaOutlet>();
        outlets.push_back(std::move(s));
    }
}

//Redundant Functions (Use only if necessary or small scale)
void MediaBiasSimulator::outputVoterBias(){
    for(Voter& v: voters){
        v.OutputData();
    }
}
void MediaBiasSimulator::outputOutletBias(){
    for(auto& m: outlets){
        m->OutputData();
    }
}

//Get Data
void MediaBiasSimulator::generateReportAndGraph() {
    const int NUM_BINS = 10;
    std::vector<int> bins(NUM_BINS, 0);
    int max_bin_count = 0;

    // 1. Map and count voters into visual array bins
    for (const auto& voter : voters) {
        int bin_idx = static_cast<int>(((voter.bias + 1.0f) / 2.0f) * NUM_BINS);
        bin_idx = std::clamp(bin_idx, 0, NUM_BINS - 1);
        
        bins[bin_idx]++;
        max_bin_count = std::max(max_bin_count, bins[bin_idx]);
    }

    // 2. Identify display routing parameters
    bool should_print_to_screen = (time_days % 20 == 0);
    std::ofstream file("simulation_log.txt", std::ios::app);

    // Lambda helper to funnel text to both active streams simultaneously
    auto log_output = [&](const std::string& text) {
        if (should_print_to_screen) std::cout << text;
        if (file.is_open()) file << text;
    };

    // 3. Render Header
    log_output("\n================= VOTER DISTRIBUTION & MEDIA LOCATIONS (Day " + std::to_string(time_days) + ") =================\n\n");

    // 4. Step through rows and render spectrum lines
    for (int i = 0; i < NUM_BINS; ++i) {
        float bin_center = -1.0f + (i * 0.2f) + 0.1f;
        
        // Format the political slice bracket
        std::stringstream line;
        line << "[" << std::fixed << std::setw(4) << std::setprecision(1) << bin_center << "]: ";
        
        // Generate population bars instantly using structural fill constructors
        if (max_bin_count > 0 && bins[i] > 0) {
            if (max_bin_count > 0 && bins[i] > 0) {
                int bar_length = (bins[i] * 40) / max_bin_count;
                for (int b = 0; b < bar_length; ++b) {
                    line << "█"; // Safely passes the multibyte UTF-8 block token
                }
            }
        }
        
        // Match smart-pointer outlet keys to row bins
        for (const auto& outlet : outlets) {
            int outlet_bin = static_cast<int>(((outlet->bias + 1.0f) / 2.0f) * NUM_BINS);
            outlet_bin = std::clamp(outlet_bin, 0, NUM_BINS - 1);

            if (outlet_bin == i) {
                // If it's your sentinel state media component, name it explicitly
                std::string label = (outlet->id == -1) ? "State Media" : "Outlet " + std::to_string(outlet->id);
                line << "  <-- [" << label << " (Bias: " << std::fixed << std::setprecision(2) << outlet->bias << ")]";
            }
        }
        
        line << "\n";
        log_output(line.str());
    }

    log_output("\n========================================================================\n");
    if (file.is_open()) file.close();

    //render polarisation bar
    renderStandardDeviationBar();
}
void MediaBiasSimulator::renderStandardDeviationBar(){
    if (voters.empty()) return;
    float sum_squared_diff = 0.0f;
    for (const auto& v : voters) {
        float diff = v.bias-average_voter_bias;
        sum_squared_diff +=diff * diff;
    }
    float variance = sum_squared_diff/voters.size();
    float standard_deviation = std::sqrt(variance);
    // make a 20 character long progress bar
    const int BAR_MAX_WIDTH = 20;
    int filled_length = static_cast<int>(standard_deviation * BAR_MAX_WIDTH);

    //logic error check
    if (filled_length > BAR_MAX_WIDTH) filled_length = BAR_MAX_WIDTH;
    if (filled_length < 0) filled_length = 0;
    std::string visual_bar = "[";
    for (int i = 0; i < BAR_MAX_WIDTH; ++i) {
        if (i < filled_length) {
            visual_bar += "█"; // filled part
        } else {
            visual_bar += "░"; // empty part
        }
    }
    visual_bar += "]";

    // Open file in append mode to duplicate terminal logs to disk
    std::ofstream file("simulation_log.txt", std::ios::app);
    
    // 4. Output the metrics
    bool should_print_to_screen = (time_days % 20 == 0);
    
    auto log_stream = [&](const std::string& msg) {
        if (should_print_to_screen) std::cout << msg;
        if (file.is_open()) file << msg;
    };

    std::stringstream ss;
    ss << "Polarisation (Std Deviation): " << visual_bar << " " 
       << std::fixed << std::setprecision(3) << standard_deviation << "\n";
    
    log_stream(ss.str());
    if (file.is_open()) file.close();

}

//Main function for executing turns
void MediaBiasSimulator::tick(){
    //Get Market Data (Same for all companies)
    setDynamicInfluenceThreshold();
    setAvgVoterBias();

    // Stage One : Voters/Viewers watch media
    for(Voter& voter : voters){
        for(auto& mediaoutlet : outlets){
            voter.watchMediaOutlet(voter_tolerance, learning_rate, *mediaoutlet);
        }
    }
    // Stage Two : Media Outlets analyse data
    for(auto& mediaoutlet : outlets){
        if(mediaoutlet->id!=-1){
            PrivateMediaOutlet* private_outlet = static_cast<PrivateMediaOutlet*>(mediaoutlet.get());
            private_outlet->radicaliseOutlets(*this);
        }
    }
    
    // Stage Three : Generate Graphs
    generateReportAndGraph();

    time_days++;
}

int main() {
    std::cout << "========================================================================\n";
    std::cout << "                     AGENCY-BASED MEDIA SIMULATION                      \n";          
    std::cout << "========================================================================\n\n";

    // 1. Collect environment scale bounds with safe filtering
    std::cout << "Note: The most optimal setup is 1000 voters for each media network\n";
    int total_voters = getSafeIntegerInput("Enter citizen population size (e.g., 500 - 50000): ", 500, 50000);
    int total_outlets = getSafeIntegerInput("Enter number of private market networks (e.g., 2 - 50): ", 2, 50);
    int total_days = getSafeIntegerInput("Enter simulation timeline lifespan in turns/days (e.g., 10 - 750): ", 10, 750);

    std::cout << "Options:\n";
    std::cout << "1. United States  [Free-Market Commercialization, High Polarisation, No State Anchor]\n";
    std::cout << "2. United Kingdom [Regulated Marketplace, Public Anchor (BBC)]\n";
    std::cout << "3. Germany        [High Institutional Trust, Dual-System Cohesion ]\n";
    std::cout << "4. Russia         [Totalitarian State Control, Hardcoded Regime Propaganda Core]\n";
    std::cout << "5. India          [Hyper-Competitive Market Saturation, Visual Sensationalism]\n";

    int nation_chosen = getSafeIntegerInput("Enter the nation you would like to simulate the voter-media relationship of (1-5): ", 1, 5);
    SimulationPresets selected_mode = SimulationPresets::UNITED_STATES;
    std::string profile_name = "";
    MediaBiasSimulator sim(total_voters,total_outlets);

    switch (nation_chosen) {
        case 1:
            selected_mode = SimulationPresets::UNITED_STATES;
            profile_name = "USA (Free Market)";
            sim.voter_start_randomgen = -0.85f;    // Highly dispersed initial public
            sim.voter_end_randomgen = 0.85f;
            sim.voter_tolerance = 0.20f;           // Tight echo chambers / high confirmation bias
            sim.learning_rate = 0.015f;            // Rapid individual mind shifts
            sim.minimum_share_percentage = 0.28f;  // Ruthless market survival bar
            sim.radicalisation_rate = 0.04f;       // Aggressive, fast corporate polarization
            sim.state_media_exists = false;        // Pure commercial landscape
            break;
        case 2:
            selected_mode = SimulationPresets::UNITED_KINGDOM;
            profile_name = "UK (Balanced Media)";
            sim.voter_start_randomgen = -0.65f;    // Moderate initial distribution
            sim.voter_end_randomgen = 0.65f;
            sim.voter_tolerance = 0.38f;           // Open-minded consumers / broad viewing habits
            sim.learning_rate = 0.006f;            // Measured individual ideological drift
            sim.minimum_share_percentage = 0.12f;  // Safe, regulated corporate survival bar
            sim.radicalisation_rate = 0.018f;      // Gradual editorial positioning
            sim.state_media_exists = true;         // Subsidized central anchor active
            break;
        case 3:
            selected_mode = SimulationPresets::GERMANY;
            profile_name = "Germany (Dual-Media System)";
            sim.voter_start_randomgen = -0.50f;    // Heavily centralized initial public
            sim.voter_end_randomgen = 0.50f;
            sim.voter_tolerance = 0.45f;           // Maximum broad-mindedness / massive public trust
            sim.learning_rate = 0.003f;            // Extremely steady, analytical voter base
            sim.minimum_share_percentage = 0.08f;  // Highly protective/supportive market floor
            sim.radicalisation_rate = 0.010f;      // Very stable, slow-moving corporate stances
            sim.state_media_exists = true;         // Dominant centrist state broadcasters
            break;
        case 4:
            selected_mode = SimulationPresets::RUSSIA;
            profile_name = "Russia (Totalitarian Propaganda)";
            sim.voter_start_randomgen = 0.10f;     // Populace pre-skewed/restricted heavily to the state narrative
            sim.voter_end_randomgen = 0.85f;
            sim.voter_tolerance = 0.22f;           // Restricted informational variance
            sim.learning_rate = 0.025f;            // High susceptibility due to media saturation
            sim.minimum_share_percentage = 0.35f;  // Private outlets face massive existential pressure
            sim.radicalisation_rate = 0.050f;      // Outlets must aggressively align or polarize to survive
            sim.state_media_exists = true;         // State media exists as an active propaganda arm
            break;
        case 5:
            selected_mode = SimulationPresets::INDIA;
            profile_name = "India (Hyper-Competitive)";
            sim.voter_start_randomgen = -0.70f;    
            sim.voter_end_randomgen = 0.70f;
            sim.voter_tolerance = 0.25f;           // High polarization among TV news audiences
            sim.learning_rate = 0.012f;            // Fast-paced media consumption habits
            sim.minimum_share_percentage = 0.32f;  // Intense market crowding requires a high audience share
            sim.radicalisation_rate = 0.045f;      // High-turnover, sensationalist polarization speed
            sim.state_media_exists = false;        // Public broadcaster exists but is market-bypassed
            break;
    }
    // Instantiate the class once
    // Usual ratio of voters to outlets is 100:1 or (1000+:1 in reality)
    for(int i=0;i<=total_days;i++){
        sim.tick();
    }

    std::cout << "\n========================================================================\n";
    std::cout << "SIMULATION CYCLE FINALIZED SUCCESSFULLY\n";
    std::cout << "========================================================================\n";
    std::cout << "-> Preset: " << profile_name;
    std::cout << "-> Operational Turns Processed: " << total_days << " Days.\n";
    std::cout << "-> Finalised population profiles exported cleanly to hard disk.\n";
    std::cout << "-> Open 'simulation_log.txt' to scroll your chronological graphs.\n";
    std::cout << "========================================================================\n";

    return 0;
}
