#include <iostream>
#include <portaudio.h>
#include <thread>
#include <atomic>

#include "../include/global.h"
#include "../include/displayAvailable.h"
#include "../include/controller.h"
#include "../include/lowPass.h"
#include "../include/pa_ringbuffer.h"

using namespace std;

// ----- ADD THIS -----
// Global instance of sharedSpace
sharedSpace sharedMem;

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

    static float delayL[filterlength]{};
    static float filteredInput[framePerBuffer]{};

    if (!uData->cp.isStreamActive.load())
    {
        return paContinue;
    }

    for (unsigned long i = 0; i < framesPerBuffer; i++)
    {
        float xL = in ? in[2 * i] : 0.0f;
        float filteredImpulseL = 0.0f;
        for (int k = filterlength - 1; k > 0; k--)
        {
            delayL[k] = delayL[k - 1];
        }
        delayL[0] = xL;
        for (int k = 0; k < filterlength; k++)
        {
            filteredImpulseL += lpfParamters::ha[k] * delayL[k];
        }
        filteredInput[i] = filteredImpulseL; // Left
    }

    for (int m = 0; m < ResampledFrameSize; m++)
    {
        int integer = m * 2.7625;
        float fraction = float(m * 2.7625) - float(integer);
        float val = (1 - fraction) * filteredInput[integer] + fraction * filteredInput[integer + 1];
        // write to ring buffer
        PaUtil_WriteRingBuffer(&sharedMem.ringBuffer, &val, 1);
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
        for (unsigned long i = 0; i < framesPerBuffer; i++)
        {
            float val = 0.0f;
            if (PaUtil_ReadRingBuffer(&sharedMem.ringBuffer, &val, 1) == 0)
                val = 0.0f; // buffer empty, output silence
            out[i] = CoreParameters::gain.load() * val;
        }
    }
    else
    {

        for (unsigned long i = 0; i < framesPerBuffer; i++)
        {

            out[i] = CoreParameters::gain.load() * in[i];
        }
    }

    return paContinue;
}

// Main
int main()
{
    cout << "---- START ----" << endl;

    checkError(Pa_Initialize());

    // ----- FIXED RING BUFFER INITIALIZATION -----
    if (PaUtil_InitializeRingBuffer(&sharedMem.ringBuffer, sizeof(float), 1024, sharedMem.ringBufferStorage) < 0)
    {
        cout << "Failed to initialize ring buffer!" << endl;
        return -1;
    }

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

    userD2.cp.isStreamActive.store(true);

    computelpfImpuseResponse();
    cout << "Impulse Response" << endl;
    for (int k = 0; k < filterlength; k++)
    {
        cout << k << " : " << lpfParamters::ha[k] << endl;
    }
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
                             ResampledFrameSize,
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

    return 0;
}
