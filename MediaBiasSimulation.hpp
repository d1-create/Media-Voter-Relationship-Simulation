#pragma once

#include <memory>
#include <vector>
#include <random>

struct Voter;
struct MediaOutlet;

//Full Media Simulation
struct MediaBiasSimulator{
private:
    //Random Number Getter Function
    float getRandomSpectrum();
public:
    int time_days = 0;
    //Start and end of political spectrum
    const float start_spectrum = -1.0f; //left wing/party A
    const float end_spectrum = 1.0f; //right wing/party B

    //Randomisation attributes
    std::mt19937 random_number_generator;

    //Voter settings
    float voter_start_randomgen = -0.75f;   //Bias of voter (starting value) 
    float voter_end_randomgen = 0.75f;      //Bias of voter (ending value)  
    float voter_tolerance = 0.4f;           //Voter media outlet (range of viewpoints) tolerance
    float learning_rate = 0.005f;           //How fast the voter is affected by bias and picks up different viewpoints

    //Outlet settings
    float minimum_share_percentage = 0.10f; // Outlets expect at least 10% of a fair market share to sustain themselves 
    float current_influence_threshold = 0.0f;
    float radicalisation_rate = 0.02f;  // How fast they polarise

    //Array of organisations and peoples
    std::vector<Voter> voters;
    std::vector<std::unique_ptr<MediaOutlet>> outlets;
    //Data
    float average_voter_bias = 0.0f;
    //Settings
    bool state_media_exists = true;

    // ### FUNCTIONS ###
    //Constructor/Startup Function
    MediaBiasSimulator(int voter_number, int outlet_number);
    void prepareSimulator();
    //Terminal Output Function
    void outputVoterBias();
    //Terminal Output Function
    void outputOutletBias();
    //Main Function for Turns/FPS
    void tick();

    //Calculate Influence Needed
    void setDynamicInfluenceThreshold();
    void setAvgVoterBias();

    //AI-Made ASCII Function (Tedious Graph Work)
    void generateReportAndGraph(int num_steps, int chart_height);
    //Polarisation Bar
    void renderStandardDeviationBar();

};
