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

MediaBiasSimulator::MediaBiasSimulator(int voter_number, int outlet_number){
    //wipe save data
    std::ofstream wipe_old_log_file("simulation_log.txt", std::ios::trunc);
    wipe_old_log_file.close(); 

    //generate random number device
    std::random_device rd;
    random_number_generator.seed(rd());
    //generate voter data and outlet data randomly
    voters.reserve(voter_number);
    for(int i=0;i<voter_number;i++){
        Voter v;
        v.bias = getRandomSpectrum();
        v.id = i;
        voters.push_back(v);
    }
    outlets.reserve(outlet_number+1); //state media so +1
    for(int i = 0; i < outlet_number; i++){
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
    // Instantiate the class once
    // Usual ratio of voters to outlets is 100:1 or (1000+:1 in reality)
    MediaBiasSimulator sim(4000,4);
    for(int i=0;i<=100;i++){
        sim.tick();
    }
    return 0;
}
