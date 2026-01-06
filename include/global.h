#ifndef GLOBAL_H
#define GLOBAL_H

#include<atomic>


//Global Parameters
#define mic1Channel 2
#define outChannel 2
#define sampleRate 44100
#define framePerBuffer 528

extern bool isdisplayActive;

struct CoreParameters{
    std::atomic <bool> isStreamActive;
};

struct userData1
{
    CoreParameters cp;
};

#endif