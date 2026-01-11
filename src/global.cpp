#include "../include/global.h"
bool isdisplayActive = true;
float sharedSpace::sampleVal[ResampledFrameSize] = {};

float lpfParamters::wn[filterlength] = {};
float lpfParamters::ha[filterlength] = {};


std::atomic<float> CoreParameters::gain = 10;