#ifndef GLOBAL_H
#define GLOBAL_H

using namespace std;
#include<atomic>

// Global Parameters
static bool isDisplayActive = true;
#define numberOfChannels 1

#define SampleRate 44100
#define framesPerBuffer 528

struct coreParameters
{
    atomic<bool> isStreamActive;
    
};

// Global Structures
struct CallBackUserData1
{
    coreParameters cp;
};

struct CallBackUserData2
{
    coreParameters cp;
};

// Global Class

#endif