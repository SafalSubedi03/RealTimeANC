#ifndef GLOBAL_H
#define GLOBAL_H

#include <atomic>

// Global Parameters
// Stream 1 Parameter
#define Mic1Index       20
#define mic1Channel     2
#define Out1Index       17
#define out1Channel     2
#define sampleRate1     48000
//Stream 2 Parameter
#define Mic2Index       21
#define mic2Channel     1
#define Out2Index       16
#define out2Channel     1
#define sampleRate2     16000

#define framePerBuffer 528

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

struct userData1
{
    CoreParameters cp;
    
    
};
struct userData2
{
    CoreParameters cp;};

#endif