#pragma once

//Media Outlet Struct
struct MediaOutlet{
    int id = 0;
    float bias = 0.0;
    float total_influence = 0.0;

    void AddInfluence(float influence);

    void OutputData();
};

//Voter Struct
struct Voter{
    int id = 0;
    float bias = 0.0;
    //Function checking the bias a voter gains from a media outlet
    void watchMediaOutlet(float voter_tolerance, float learning_rate, MediaOutlet& outlet);

    void OutputData();
};

struct MediaBiasSimulator{
private:
    //Randomisation attributes
    std::mt19937 random_number_generator;

    //Start and end of political spectrum
    float start_spectrum = -1.0; //left wing/party A
    float end_spectrum = 1.0; //right wing/party B

    //Random Number Getter Function
    float getRandomSpectrum();
public:
    //Voter settings
    float voter_start_randomgen = -0.8;    //Bias of voter (starting value) 
    float voter_end_randomgen = 0.8;       //Bias of voter (ending value)  
    float voter_tolerance = 0.2;           //Voter media outlet (range of viewpoints) tolerance
    float learning_rate = 0.1;             //How fast the voter is affected by bias and picks up different viewpoints

    //Array of organisations and peoples
    std::vector<Voter> voters;
    std::vector<MediaOutlet> outlets;

    //Constructor/Startup Function
    MediaBiasSimulator(int voter_number, int outlet_number);
    
    //Terminal Output Function
    void outputVoterBias();
    //Terminal Output Function
    void outputOutletBias();
    //Main Function for Turns/FPS
    void tick();
};
