#ifndef GLOBAL_H
#define GLOBAL_H

#include <atomic>

// Global Parameters
// Stream 1 Parameter
#define Mic1Index       22
#define mic1Channel     1
#define Out1Index       21
#define out1Channel     1
#define sampleRate1     8000

// Stream 2 Parameter
#define Mic2Index       8
#define mic2Channel     2
#define Out2Index       12
#define out2Channel     2
#define sampleRate2     44100

#define framePerBuffer 528


extern bool isdisplayActive;

struct CoreParameters
{
    std::atomic<bool> isStreamActive;

    std::atomic<float> gainL;
    std::atomic<float> gainR;

    int channelCount;
};

struct userData1
{
    CoreParameters cp;
};

struct userData2
{
    CoreParameters cp;
};

#endif