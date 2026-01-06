#include <iostream>
#include <portaudio.h>

#include "../include/global.h"
#include "../include/displayAvailable.h"

using namespace std;

// helper
static void checkError(PaError err)
{
    if (err != paNoError)
    {
        cout << "Error: " << Pa_GetErrorText(err) << endl;
        exit(EXIT_FAILURE);
    }
    
}

static int stream1CallBack(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesperbuffer,
    const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags,
    void *u

)
{
    const float *in = inputBuffer ? (const float *)inputBuffer : nullptr;
    float *out = (float *)outputBuffer;
    userData1 *uData = (userData1 *)u;
    
    for (unsigned long i = 0; i < framesperbuffer; i++)
    {
        out[2 * i] = in ? 0.8f *in[2 * i] : 0.0f;
        out[2 * i + 1] = in ? 0.8f * in[2 * i + 1] : 0.0f;
    }

    return paContinue;
}

int main()
{
    cout << "----START----" << endl;
    PaError err = Pa_Initialize();
    checkError(err);
    if(isdisplayActive){
        printAvailableDevices();
        return 0;
    }

    int Mic1Index = 0;
    int OutIndex = 4;

    cout << "Mic: " << Pa_GetDeviceInfo(Mic1Index)->name << endl;
    cout << "Channel: " << Pa_GetDeviceInfo(OutIndex)->maxOutputChannels << endl;
    cout << "Out: " << Pa_GetDeviceInfo(OutIndex)->name << endl;
    cout << "Channel: " << Pa_GetDeviceInfo(OutIndex)->maxOutputChannels << endl;


    // int Mic1Index = Pa_GetDefaultInputDevice();
    // int OutIndex = Pa_GetDefaultOutputDevice();

    userData1 userD1;

    PaStreamParameters mic1Parameters;
    PaStreamParameters outparameters;



    mic1Parameters.device = Mic1Index;
    mic1Parameters.channelCount = mic1Channel;
    mic1Parameters.hostApiSpecificStreamInfo = nullptr;
    mic1Parameters.sampleFormat = paFloat32;
    mic1Parameters.suggestedLatency = Pa_GetDeviceInfo(Mic1Index)->defaultLowInputLatency;

    outparameters.device = OutIndex;
    outparameters.channelCount = outChannel;
    outparameters.hostApiSpecificStreamInfo = nullptr;
    outparameters.sampleFormat = paFloat32;
    outparameters.suggestedLatency = Pa_GetDeviceInfo(OutIndex)->defaultLowOutputLatency;

    PaStream *stream;
    err = Pa_OpenStream(&stream,
                        &mic1Parameters,
                        &outparameters,
                        sampleRate,
                        framePerBuffer,
                        paClipOff,
                        stream1CallBack,
                        &userD1);
    checkError(err);
    
    Pa_StartStream(stream);   
   

    while(Pa_IsStreamActive(stream) == 1)
        Pa_Sleep(100);


    Pa_CloseStream(stream);
    
    Pa_Terminate();

    return 0;
}
