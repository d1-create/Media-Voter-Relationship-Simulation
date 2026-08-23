#pragma once

#include <memory>

struct MediaBiasSimulator;

struct MediaOutlet{
    int id = 0;
    float bias = 0.0f;
    float total_influence = 0.0f;
    float turn_influence = 0.0f;

    void OutputData();
};

//State Media
struct StateMediaOutlet : MediaOutlet{
    // The State Media still tracks views for data analysis
    // But no radicalisation exists
    StateMediaOutlet(){
        id=-1; // Sentinel value as exception
    }
};

//Media Outlet Struct (Profit Driven)
struct PrivateMediaOutlet : MediaOutlet{
    //days the outlet has not been making a profit
    int consecutive_days_starving = 0; 
    //Change Bias If No Influence
    void radicaliseOutlets(MediaBiasSimulator& sim);
};

//Voter Struct (Bias and Interest Driven)
struct Voter{
    int id = 0;
    float bias = 0.0f;
    //Function checking the bias a voter gains from a media outlet
    void watchMediaOutlet(float voter_tolerance, float learning_rate, MediaOutlet& outlet);  

    void OutputData();
};

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
    float minimum_share_percentage = 0.10f; // Outlets expect at least 10% of a "fair share" of the market
    float current_influence_threshold = 0.0f;
    float radicalisation_rate = 0.02f;  // How fast they polarise

    //Array of organisations and peoples
    std::vector<Voter> voters;
    std::vector<std::unique_ptr<MediaOutlet>> outlets;
    //Data
    float average_voter_bias = 0.0f;
    //Settings
    bool state_media_exists = true;
    //Constructor/Startup Function
    MediaBiasSimulator(int voter_number, int outlet_number);
    
    //Terminal Output Function
    void outputVoterBias();
    //Terminal Output Function
    void outputOutletBias();
    //Main Function for Turns/FPS
    void tick();

    //Calculate Influence Needed
    void setDynamicInfluenceThreshold() {
        if (outlets.empty()) return;

        // 1. Calculate an even split of all voters among all outlets
        float fair_share = static_cast<float>(voters.size()) / outlets.size();
        // 2. Multiply by the minimum acceptable percentage (e.g., 0.10f for 10%)
        current_influence_threshold =  fair_share * minimum_share_percentage;
    }

    void setAvgVoterBias(){
        if(voters.empty()) return;
        float total_bias = 0.0f;
        for(Voter& v:voters){
            total_bias+=v.bias;
        }
        average_voter_bias = total_bias/voters.size();
    }

    //AI-Made ASCII Function (Tedious Graph Work)
    void generateReportAndGraph();


};
