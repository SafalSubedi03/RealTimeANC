#include <iomanip>
#include "..\include\displayAvailable.h"
#include <portaudio.h>
#include <iostream>

using namespace std;

void printAvailableDevices()
{
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
    {
        cout << "ERROR: Pa_GetDeviceCount returned " << numDevices << endl;
        return;
    }

    cout << "\n================ Available Audio Devices ================\n";

    cout << left
         << setw(4)  << "Idx"
         << setw(60) << "Device Name"
         << setw(22) << "API"
         << setw(8)  << "In Ch"
         << setw(8)  << "Out Ch"
         << setw(12) << "Sample Rate"
         << endl;

    cout << string(84, '-') << endl;

    for (int i = 0; i < numDevices; i++)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        const PaHostApiInfo* apiInfo =
            Pa_GetHostApiInfo(deviceInfo->hostApi);

        cout << left
             << setw(4)  << i
             << setw(60) << deviceInfo->name
             << setw(22) << apiInfo->name
             << setw(8)  << deviceInfo->maxInputChannels
             << setw(8)  << deviceInfo->maxOutputChannels
             << setw(12) << deviceInfo->defaultSampleRate
             << endl;
    }
    cout << "=========================================================\n";
}
