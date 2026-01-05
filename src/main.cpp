// Include Section
#include <iostream>
#include <portaudio.h>
#include <thread>
#include <atomic>


#include "../include/global.h"
#include "../include/displayAvailable.h"
#include "../include/controller.h"

using namespace std;

// audioCallBack
static int audioCallBack1(const void *inputBuffer,
                          void *outputBuffer,
                          unsigned long framesperbuffer,
                          const PaStreamCallbackTimeInfo *,
                          PaStreamCallbackFlags, void *u)
{
    float* in = (float*)inputBuffer; 
    float* out = (float*)outputBuffer;
    CallBackUserData1 *userdata = (CallBackUserData1*) u;
    
    if(userdata->cp.isStreamActive.load()){
    for(int i = 0; i<framesperbuffer; i++){
       out[i] = in[i];
    }}
    return 0;
}

static int audioCallBack2(const void *inputBuffer,
                          void *outputBuffer,
                          unsigned long framesperbuffer,
                          const PaStreamCallbackTimeInfo *,
                          PaStreamCallbackFlags, void *u)
{
    float* in = (float*)inputBuffer; 
    float* out = (float*)outputBuffer;
    CallBackUserData2 *userdata = (CallBackUserData2*) u;
    
    if(userdata->cp.isStreamActive.load()){
    for(int i = 0; i<framesperbuffer; i++){
       out[i] = in[i];
    }}
    return 0;
}


// Helper Function to display error
static void checkerror(PaError err)
{
    if (err != paNoError)
    {
        cout << "Error" << Pa_GetErrorText(err) << endl;
        exit(EXIT_FAILURE);
    }
}

int main()
{
    cout << "--Start--" << endl;
    // PortAudio initialize
    PaError err = Pa_Initialize();
    checkerror(err);

    if (isDisplayActive)
    {
        printAvailableDevices();
        return 0; // Terminate the program after displaying the list of availabel devies.
    }


    // PortAudio I/O Parameters
    PaStreamParameters mic1Parameters;
    PaStreamParameters mic2Parameters;
    PaStreamParameters out1Parameters;

    int mic1Index = 0;
    int mic2Index = 0;
    int out1Index = 4;

    // Configure Parameters
    // Mic1
    mic1Parameters.device = mic1Index;
    mic1Parameters.channelCount = numberOfChannels;
    mic1Parameters.hostApiSpecificStreamInfo = nullptr;
    mic1Parameters.sampleFormat = SampleRate;
    mic1Parameters.sampleFormat = paFloat32;
    mic1Parameters.suggestedLatency = Pa_GetDeviceInfo(mic1Index)->defaultLowInputLatency;

    // Mic2
    mic2Parameters.device = mic2Index;
    mic2Parameters.channelCount = numberOfChannels;
    mic2Parameters.hostApiSpecificStreamInfo = nullptr;
    mic2Parameters.sampleFormat = SampleRate;
    mic2Parameters.sampleFormat = paFloat32;
    mic2Parameters.suggestedLatency = Pa_GetDeviceInfo(mic2Index)->defaultLowInputLatency;

    // Out
    out1Parameters.device = out1Index;
    out1Parameters.channelCount = numberOfChannels;
    out1Parameters.hostApiSpecificStreamInfo = nullptr;
    out1Parameters.sampleFormat = SampleRate;
    out1Parameters.sampleFormat = paFloat32;
    out1Parameters.suggestedLatency = Pa_GetDeviceInfo(out1Index)->defaultHighOutputLatency;


    //UserData 
    CallBackUserData1 userD1;
    CallBackUserData2 userD2;

    //Create Thread
    thread ControllerThread(control,ref(userD1),ref(userD2));

    //Initialize the CallBackUserData
    userD1.cp.isStreamActive.store(false);
    userD2.cp.isStreamActive.store(false);
    

    // Initialize Stream
    //first Stream
    PaStream *inStream1;
    
    err = Pa_OpenStream(&inStream1,
                        &mic1Parameters,
                        &out1Parameters,
                        SampleRate,
                        framesPerBuffer,
                        paClipOff,
                        audioCallBack1,
                        &userD1

    );
    checkerror(err);
    
    PaStream *inStream2;

    //Second Stream
     err = Pa_OpenStream(&inStream2,
                        &mic2Parameters,
                        &out1Parameters,
                        SampleRate,
                        framesPerBuffer,
                        paClipOff,
                        audioCallBack1,
                        &userD2
    );
    checkerror(err);

    //Start Stream 
    Pa_StartStream(inStream1);
    Pa_StartStream(inStream2);
    while(Pa_IsStreamActive(inStream1) || Pa_IsStreamActive(inStream2)){
        Pa_Sleep(100);
    }

    Pa_CloseStream(inStream1);
    Pa_CloseStream(inStream2);   

    Pa_Terminate();
    return 0;
}