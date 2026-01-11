#ifndef GLOBAL_H
#define GLOBAL_H

#include <atomic>

// Global Parameters
// Stream 1 Parameter
#define Mic1Index       0
#define mic1Channel     2
#define Out1Index       6
#define out1Channel     2
#define sampleRate1     44100
//Stream 2 Parameter
#define Mic2Index       21
#define mic2Channel     1
#define Out2Index       16
#define out2Channel     1
#define sampleRate2     16000
#define ResampledFrameSize 190

#define framePerBuffer 528
#define filterlength 21
#define pi 3.1415792

extern bool isdisplayActive;

struct CoreParameters
{
    std::atomic<bool> isStreamActive;

    std::atomic<float> gainL;
    std::atomic<float> gainR;

    int channelCount;
};

struct sharedSpace{
    static float* sampleVal;
};

struct lpfParamters{
    static float wn[filterlength];
    static float ha[filterlength];

};

struct userData1

{
    CoreParameters cp;
    
    
};
struct userData2
{
    CoreParameters cp;};

#endif