#include <iostream>
#include <portaudio.h>
#include <thread>
#include <atomic>

#include "../include/global.h"
#include "../include/displayAvailable.h"
#include "../include/controller.h"

using namespace std;

// Helper

static void checkError(PaError err)
{
    if (err != paNoError)
    {
        cout << "PortAudio Error: " << Pa_GetErrorText(err) << endl;
        Pa_Terminate();
        exit(EXIT_FAILURE);
    }
}

// ------------------------------------------------------------
// Stream 1 callback: Laptop mic -> Laptop speakers
// Mic: mono (1 ch), Output: stereo (2 ch)
// ------------------------------------------------------------
static int stream1CallBack(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags,
    void *u)
{
    const float *in = (const float *)inputBuffer; // mono
               // stereo
    userData1 *uData = (userData1 *)u;

    if (!uData->cp.isStreamActive.load())
    {
        for (unsigned long i = 0; i < framePerBuffer; i++)
           return paContinue;
    }

    for (unsigned long i = 0; i < framesPerBuffer; i++)
    {
        // float sample = in ? in[i] * 0.8f : 0.0f;
        sharedSpace::sampleVal[2*i]  =    in ? uData->cp.gainL * in[2 * i] : 0.0f;         // Left
        sharedSpace::sampleVal[2*i+1]   = in ? uData->cp.gainR * in[2 * i + 1] : 0.0f; // Right
    }
    return paContinue;
}

// Stream 2 callback: JBL Hands-Free mic -> JBL Hands-Free speaker
// Mic: mono (1 ch), Output: mono (1 ch)

static int stream2CallBack(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags,
    void *u)
{
    const float *in = (const float *)inputBuffer; // mono
    float *out = (float *)outputBuffer;           // mono
    userData2 *uData = (userData2 *)u;

    if (!uData->cp.isStreamActive.load())
    {
        for (unsigned long i = 0; i < framePerBuffer; i++)
        {
            out[2 * i] = 0.0f;     // Left
            out[2 * i + 1] = 0.0f; // Right
        }
        return paContinue;
    }

    for (unsigned long i = 0; i < framesPerBuffer; i++)
        out[i] = sharedSpace::sampleVal[2*i];

    return paContinue;
}

// Main

int main()
{
    cout << "---- START ----" << endl;

    checkError(Pa_Initialize());

    if (isdisplayActive)
    {
        printAvailableDevices();
        cout << "Press any key to continue..." << endl;
        char w;
        cin >> w;
    }

    // Print selected devices (sanity check)
    cout << "Stream 1 Mic : " << Pa_GetDeviceInfo(Mic1Index)->name << endl;
    cout << "Stream 1 Out : " << Pa_GetDeviceInfo(Out1Index)->name << endl;
    cout << "Stream 2 Mic : " << Pa_GetDeviceInfo(Mic2Index)->name << endl;
    cout << "Stream 2 Out : " << Pa_GetDeviceInfo(Out2Index)->name << endl;

    // User data
    userData1 userD1;
    userData2 userD2;
    userD1.cp.isStreamActive.store(true);
    userD1.cp.gainL.store(10);

    userD2.cp.isStreamActive.store(false);
    userD2.cp.gainR.store(10);
    thread controlT(controller, ref(userD1), ref(userD2));
    // Stream parameters

    PaStreamParameters mic1{}, out1{}, mic2{}, out2{};

    // ---- Stream 1 (Laptop) ----
    mic1.device = Mic1Index;
    mic1.channelCount = mic1Channel; // 1
    mic1.sampleFormat = paFloat32;
    mic1.suggestedLatency = Pa_GetDeviceInfo(mic1.device)->defaultLowInputLatency;

    out1.device = Out1Index;
    out1.channelCount = out1Channel; // 2
    out1.sampleFormat = paFloat32;
    out1.suggestedLatency = Pa_GetDeviceInfo(out1.device)->defaultLowOutputLatency;

    // ---- Stream 2 (JBL Hands-Free) ----
    mic2.device = Mic2Index;
    mic2.channelCount = mic2Channel; // 1
    mic2.sampleFormat = paFloat32;
    mic2.suggestedLatency = Pa_GetDeviceInfo(mic2.device)->defaultLowInputLatency;

    out2.device = Out2Index;
    out2.channelCount = out2Channel; // 1
    out2.sampleFormat = paFloat32;
    out2.suggestedLatency =
        Pa_GetDeviceInfo(out2.device)->defaultLowOutputLatency;

    // Format support checks (MANDATORY)

    checkError(Pa_IsFormatSupported(&mic1, &out1, sampleRate1));
    checkError(Pa_IsFormatSupported(&mic2, &out2, sampleRate2));

    // Open streams

    PaStream *stream1 = nullptr;
    PaStream *stream2 = nullptr;

    checkError(Pa_OpenStream(&stream1,
                             &mic1,
                             nullptr,
                             sampleRate1,
                             framePerBuffer,
                             paClipOff,
                             stream1CallBack,
                             &userD1));

    checkError(Pa_OpenStream(&stream2,
                             &mic2,
                             &out2,
                             sampleRate2,
                             framePerBuffer,
                             paClipOff,
                             stream2CallBack,
                             &userD2));

    // Start streams
    checkError(Pa_StartStream(stream1));
    checkError(Pa_StartStream(stream2));

    cout << "Stream 1 Active: " << Pa_IsStreamActive(stream1) << endl;
    cout << "Stream 2 Active: " << Pa_IsStreamActive(stream2) << endl;

    // Keep running
    while (Pa_IsStreamActive(stream1) == 1 || Pa_IsStreamActive(stream2) == 1)
    {
        Pa_Sleep(100);
    }

    // Cleanup
    Pa_CloseStream(stream1);
    Pa_CloseStream(stream2);
    Pa_Terminate();

    cout << "---- END ----" << endl;

    delete[] sharedSpace::sampleVal;
    return 0;
}
