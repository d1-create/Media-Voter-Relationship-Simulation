struct MediaOutlet;

//Voter Struct (Bias and Interest Driven)
struct Voter{
    int id = 0;
    float bias = 0.0f;
    //Function Returning the bias a voter gains from a media outlet
    float watchMediaOutlet(float voter_tolerance, float learning_rate, MediaOutlet& outlet);  

    void OutputData();
};
