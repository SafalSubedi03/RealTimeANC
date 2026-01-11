#include "../include/controller.h"
#include <iostream>
#include "global.h"
using namespace std;

void controller(userData1 &uData1, userData2 &uData2)
{
    char keypressed = '*';
    float changeGainBy = 5.5;
    float newGain;
    while (keypressed != 'r')
    {
        cin >> keypressed;
        switch (keypressed)
        {
        case '1':
            uData1.cp.isStreamActive.store(!uData1.cp.isStreamActive.load());
            
            cout << "Stream 1: " << uData1.cp.isStreamActive.load() << endl;
            cout << "Stream 2: " << uData2.cp.isStreamActive.load() << endl;
            break;
        case '2':
            uData2.cp.isStreamActive.store(!uData2.cp.isStreamActive.load());
           
            cout << "Stream 1: " << uData1.cp.isStreamActive.load() << endl;
            cout << "Stream 2: " << uData2.cp.isStreamActive.load() << endl;
            break;

        case 'q':
            newGain = CoreParameters::gain.load()  + changeGainBy;
            CoreParameters::gain.store(newGain);
            cout<<"Gain: "<<CoreParameters::gain.load()<<endl;
            keypressed = '*';
            break;
       
        case 'a':
            newGain = CoreParameters::gain.load()  - changeGainBy;
            if(newGain > 0)
            CoreParameters::gain.store(newGain);
            cout<<"Gain: "<<CoreParameters::gain.load()<<endl;
            keypressed = '*';
            break;
        
        default:
            keypressed = '*';
            break;
        }
    }
}
