#include <iostream>
#include <portaudio.h>
#include <thread>
#include <atomic>

#include "../include/global.h"
#include "../include/displayAvailable.h"
#include "../include/controller.h"

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

    if (uData->cp.isStreamActive.load())
    {
        for (unsigned long i = 0; i < framesperbuffer; i++)
        {
            out[2 * i] = in ? 0.8f * in[2 * i] : 0.0f;
            out[2 * i + 1] = in ? 0.0f * in[2 * i + 1] : 0.0f;
        }
    }

    return paContinue;
}

static int stream2CallBack(
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
    userData2 *uData = (userData2 *)u;

    if (uData->cp.isStreamActive.load())
    {
        for (unsigned long i = 0; i < framesperbuffer; i++)
        {
            out[2 * i] = in ? 0.0f * in[2 * i] : 0.0f;
            out[2 * i + 1] = in ? 0.8f * in[2 * i + 1] : 0.0f;
        }
    }

    return paContinue;
}

int main()
{
    cout << "----START----" << endl;
    PaError err = Pa_Initialize();
    checkError(err);
    if (isdisplayActive)
    {
        printAvailableDevices();
        char w;
        cin>>w;
    }

   
    cout << "Mic 1: " << Pa_GetDeviceInfo(Mic1Index)->name << " Channel: " << Pa_GetDeviceInfo(Mic1Index)->maxInputChannels << endl;
    cout << "Out 1: " << Pa_GetDeviceInfo(Out1Index)->name << " Channel: " << Pa_GetDeviceInfo(Out1Index)->maxOutputChannels << endl;
    cout << "Mic 2: " << Pa_GetDeviceInfo(Mic2Index)->name << " Channel: " << Pa_GetDeviceInfo(Mic2Index)->maxInputChannels << endl;
    cout << "Out 2: " << Pa_GetDeviceInfo(Out2Index)->name << " Channel: " << Pa_GetDeviceInfo(Out2Index)->maxOutputChannels << endl;
    // int Mic1Index = Pa_GetDefaultInputDevice();
    // int OutIndex = Pa_GetDefaultOutputDevice();

    userData1 userD1;
    userData2 userD2;

    PaStreamParameters mic1Parameters;
    PaStreamParameters out1Parameters;
    PaStreamParameters mic2Parameters;
    PaStreamParameters out2Parameters;

    mic1Parameters.device = Mic1Index;
    mic1Parameters.channelCount = mic1Channel;
    mic1Parameters.hostApiSpecificStreamInfo = nullptr;
    mic1Parameters.sampleFormat = paFloat32;
    mic1Parameters.suggestedLatency = Pa_GetDeviceInfo(Mic1Index)->defaultLowInputLatency;

    out1Parameters.device = Out1Index;
    out1Parameters.channelCount = out1Channel;
    out1Parameters.hostApiSpecificStreamInfo = nullptr;
    out1Parameters.sampleFormat = paFloat32;
    out1Parameters.suggestedLatency = Pa_GetDeviceInfo(Out1Index)->defaultLowOutputLatency;

    mic1Parameters.device = Mic1Index;
    mic1Parameters.channelCount = mic1Channel;
    mic1Parameters.hostApiSpecificStreamInfo = nullptr;
    mic1Parameters.sampleFormat = paFloat32;
    mic1Parameters.suggestedLatency = Pa_GetDeviceInfo(Mic1Index)->defaultLowInputLatency;

    out2Parameters.device = Out2Index;
    out2Parameters.channelCount = out2Channel;
    out2Parameters.hostApiSpecificStreamInfo = nullptr;
    out2Parameters.sampleFormat = paFloat32;
    out2Parameters.suggestedLatency = Pa_GetDeviceInfo(Out2Index)->defaultLowOutputLatency;

    userD1.cp.isStreamActive.store(true);
    userD2.cp.isStreamActive.store(false);
    

    thread controlThread(controller,ref(userD1) , ref(userD2));
    PaStream *stream1;
    PaStream *stream2;
    err = Pa_OpenStream(&stream1,
                        &mic1Parameters,
                        &out1Parameters,
                        sampleRate1,
                        framePerBuffer,
                        paClipOff,
                        stream1CallBack,
                        &userD1);
    checkError(err);
    cout<<"hi";

    // err = Pa_OpenStream(&stream2,
    //                     &mic2Parameters,
    //                     &out2Parameters,
    //                     sampleRate2,
    //                     framePerBuffer,
    //                     paClipOff,
    //                     stream2CallBack,
    //                     &userD2);
    
    // checkError(err);
    cout<<"hi";
    
    Pa_StartStream(stream1);
    // Pa_StartStream(stream2);

    while (Pa_IsStreamActive(stream1) == 1)
        Pa_Sleep(100);

    Pa_CloseStream(stream1);
    // Pa_CloseStream(stream2);

    Pa_Terminate();

    return 0;
}
