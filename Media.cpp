#include "Voter.hpp"
#include "Media.hpp"
#include "MediaBiasSimulation.hpp"

#include <random>
#include <iostream>

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
            if (bias < sim.start_spectrum) { //error checker
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

void MediaOutlet::OutputData(){ 
    std::cout<< "Media outlet " << id << " has a bias of " << bias << " and have accumulated " << total_influence << " influence";
    std::cout<<"\n";
}
