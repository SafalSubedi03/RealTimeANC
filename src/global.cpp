#include "../include/global.h"
bool isdisplayActive = true;
float* sharedSpace::sampleVal = new float[ResampledFrameSize];

float lpfParamters::wn[filterlength] = {};
float lpfParamters::ha[filterlength] = {};