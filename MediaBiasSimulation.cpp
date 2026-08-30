//NOTE:
//This program is a quick and somewhat inaccurate simulation of voters viewing media since it relies on various assumptions.
//However, it servers as a fun mathematical simulation and generalisation of political polarisation  
//(It assumes media goes to the extremes over the center and doesn't truly simulate business activity)
//Each turn = day

#include <iostream> 
#include <fstream>  
#include <vector>   
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip>
#include <random>

#include "MediaBiasSimulation.hpp"
#include "Media.hpp"
#include "Voter.hpp"
#include "TerminalInput.hpp"

//Add Windows Compatibility Library
#ifdef _WIN32
#include <windows.h>
#endif

//Startup Functions with Constructor
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
}
void MediaBiasSimulator::prepareSimulator(){
    if(state_media_exists){
        auto s_m = std::make_unique<StateMediaOutlet>();
        outlets.push_back(std::move(s_m));
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
//Basic Setters
void MediaBiasSimulator::setDynamicInfluenceThreshold() {
    if (outlets.empty()) return;
    // 1. Calculate an even split of all voters among all outlets
    float fair_share = static_cast<float>(voters.size()) / outlets.size();
    // 2. Multiply by the minimum acceptable percentage (e.g., 0.10f for 10%)
    current_influence_threshold =  fair_share * minimum_share_percentage;
}
void MediaBiasSimulator::setAvgVoterBias(){
    if(voters.empty()) return;
    float total_bias = 0.0f;
    for(Voter& v:voters){
        total_bias+=v.bias;
    }
    average_voter_bias = total_bias/voters.size();
}
//Get Data (Data-Visualisation Function)
void MediaBiasSimulator::generateReportAndGraph(int num_steps, int chart_height) {
    if (num_steps <= 0 || chart_height <= 0) return;

    // Setup Data Arrays
    std::vector<int> bins(num_steps, 0);
    std::vector<std::vector<std::string>> outlet_markers(num_steps);
    int max_bin_count = 0;

    // lambda to cleanly convert any voter bias on the gradient to a column bin index
    auto get_bin = [num_steps](float bias) {
        int idx = static_cast<int>(((bias + 1.0f) / 2.0f) * num_steps);
        return std::clamp(idx, 0, num_steps - 1);
    };

    // Add columns
    for (const auto& v : voters) {
        int idx = get_bin(v.bias);
        bins[idx]++;
        max_bin_count = std::max(max_bin_count, bins[idx]);
    }
    int max_scale = std::max(1000, max_bin_count);

    // Add Media Outlet Names
    for (const auto& o : outlets) {
        std::string label = (o->id == -1) ? "State" : "O" + std::to_string(o->id);
        outlet_markers[get_bin(o->bias)].push_back(label);
    }

    // Append to Files
    if (time_days % 20 != 0) return; // Only run logs on specific intervals
    
    std::ofstream file("simulation_log.txt", std::ios::app);
    auto print = [&](const std::string& text) {
        std::cout << text;
        if (file.is_open()) file << text;
    };

    // Draw the Bar Chart Matrix Rows
    print("\n================= VOTER DISTRIBUTION (Day " + std::to_string(time_days) + ") =================\n\n");
    for (int row = chart_height; row >= 1; --row) {
        print(std::string(4 - std::to_string((max_scale * row) / chart_height).length(), ' ') + std::to_string((max_scale * row) / chart_height) + " | ");
        for (int i = 0; i < num_steps; ++i) {
            print(((bins[i] * chart_height) / max_scale >= row) ? " █  " : "    ");
        }
        print("\n");
    }

    // Draw the Bottom Axis, Labels, and Labels Scale
    print("       +" + std::string(num_steps * 4, '-') + " (Max: " + std::to_string(max_scale) + ")\n bias: ");
    for (int i = 0; i < num_steps; ++i) {
        float center = -1.0f + ((i + 0.5f) * (2.0f / num_steps));
        print(std::to_string(center).substr(0, 4) + " "); // Simple substring formatting trick
    }
    print("\n");

    // Add Media Markers
    bool items_left = true;
    for (size_t depth = 0; items_left; depth++) {
        items_left = false;
        print(" media:");
        for (int i = 0; i < num_steps; ++i) {
            if (depth < outlet_markers[i].size()) {
                print(" " + outlet_markers[i][depth] + std::string(3 - outlet_markers[i][depth].length(), ' '));
                if (depth + 1 < outlet_markers[i].size()) items_left = true;
            } else {
                print("    ");
            }
        }
        print("\n");
    }
    print("\n========================================================================\n"); // separator line? 
    
    if (file.is_open()){ file.close(); }
    renderStandardDeviationBar();
}
//Std Calculator Function
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

    //add up the biases for each voter to ensure that you dont get biased by a network then another is out of range
    for(Voter& voter : voters){
        float curr_total_bias = 0.0f;
        for(auto& mediaoutlet : outlets){
            curr_total_bias += voter.watchMediaOutlet(voter_tolerance, learning_rate, *mediaoutlet);
        }
        voter.bias += curr_total_bias;
    }
    // Stage Two : Media Outlets analyse data
    for(auto& mediaoutlet : outlets){
        if(mediaoutlet->id!=-1){
            PrivateMediaOutlet* private_outlet = static_cast<PrivateMediaOutlet*>(mediaoutlet.get());
            private_outlet->radicaliseOutlets(*this);
        }
    }
    
    // Stage Three : Generate Graphs
    generateReportAndGraph(25,15);

    time_days++;
}

int main() {
    //Use UTF-8 so generateReportAndGraph function works on windows
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    //Terminal Input Structs
    TerminalInput terminal_input;
    SimulationPresets selected_mode = SimulationPresets::UNITED_STATES; 

    //Get Input
    terminal_input.outputStartupMessage();
    int total_voters = terminal_input.getSafeIntegerInput("Enter citizen population size (e.g., 500 - 50000): ", 500, 50000);
    int total_outlets = terminal_input.getSafeIntegerInput("Enter number of private market networks (e.g., 2 - 50): ", 2, 50);
    int total_days = terminal_input.getSafeIntegerInput("Enter simulation timeline lifespan in turns/days (e.g., 10 - 750): ", 10, 750);
    MediaBiasSimulator sim(total_voters,total_outlets);

    terminal_input.parameterInput(sim,selected_mode);

    sim.prepareSimulator();// Set state media etc before loading
    // Usual ratio of voters to outlets is 100:1 or (10000+:1 in reality)
    for(int i=0;i<=total_days;i++){
        sim.tick();
    }

    std::cout << "\n========================================================================\n";
    std::cout << "SIMULATION FINISHED\n";
    std::cout << "========================================================================\n";
    std::cout << "-> Days Processed: " << total_days << " Days.\n";
    std::cout << "-> Final population data exported successfully.\n";
    std::cout << "-> Open 'simulation_log.txt' to find day-to-day graphs.\n";
    std::cout << "========================================================================\n";

    return 0;
}
