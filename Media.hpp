#pragma once

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
