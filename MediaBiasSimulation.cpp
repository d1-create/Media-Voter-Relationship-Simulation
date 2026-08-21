//NOTE:
//This program is a quick and somewhat inaccurate simulation of voters viewing media since it relies on various assumptions.
//However, it servers as a fun mathematical simulation and generalisation of political polarisation  

#include <random> // Required header
#include <vector> // Required header 2

#include <iostream> //Optional/testing header

#include "MediaBiasSimulation.hpp"


void Voter::watchMediaOutlet(float voter_tolerance, float learning_rate, MediaOutlet& outlet){
    if(outlet.bias>=(bias-voter_tolerance) && outlet.bias<=(bias+voter_tolerance)){
        float change = learning_rate*(outlet.bias-bias);//change in bias
        bias+=change;
        std::cout << "work";
        outlet.AddInfluence(change);
    }
}

void Voter::OutputData(){
    std::cout<< "Voter " << id << " has a bias of " << bias;
    std::cout<<"\n";
}

void MediaOutlet::AddInfluence(float influence){
    total_influence +=influence;
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
    outlets.reserve(outlet_number);
    for(int i=0;i<outlet_number;i++){
        MediaOutlet m;
        m.bias = getRandomSpectrum();
        m.id = i;
        outlets.push_back(m);
    }
}

void MediaBiasSimulator::outputVoterBias(){
    for(Voter v: voters){
        v.OutputData();
    }
}

void MediaBiasSimulator::outputOutletBias(){
    for(MediaOutlet m: outlets){
        m.OutputData();
    }
}

//Main function for executing turns
void MediaBiasSimulator::tick(){
    //change 
    for(MediaOutlet m: outlets){
        for(int i=0;i<voters.size();i++){
            voters.at(i).watchMediaOutlet(voter_tolerance,learning_rate,m);
        }
    }
}


int main() {
    // Instantiate the class once
    MediaBiasSimulator sim(10,3);
    for(int i=0;i<=3;i++){
        sim.outputVoterBias();
        sim.outputOutletBias();
        sim.tick();
    }
    return 0;
}
