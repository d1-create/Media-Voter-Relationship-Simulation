#include "Media.hpp"
#include "Voter.hpp"

#include <iostream>

//Below are two same functions but one is for state media (no greed) and one is for private media (radicalise function triggers greed)
float Voter::watchMediaOutlet(float voter_tolerance, float learning_rate, MediaOutlet& outlet){
    if(outlet.bias>=(bias-voter_tolerance) && outlet.bias<=(bias+voter_tolerance)){
        float change = learning_rate*(outlet.bias-bias);//change in bias
        outlet.total_influence += 1.0f;
        outlet.turn_influence += 1.0f;
        return change;
    }
    return 0.0f;
}


//Redundant Functions (Use only if necessary or small scale)
void Voter::OutputData(){
    std::cout<< "Voter " << id << " has a bias of " << bias;
    std::cout<<"\n";
}
