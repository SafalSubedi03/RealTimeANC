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
            uData2.cp.isStreamActive.store(false);
            cout << "Stream 1: " << uData1.cp.isStreamActive.load() << endl;
            cout << "Stream 2: " << uData2.cp.isStreamActive.load() << endl;
            break;
        case '2':
            uData2.cp.isStreamActive.store(!uData2.cp.isStreamActive.load());
            uData1.cp.isStreamActive.store(false);
            cout << "Stream 1: " << uData1.cp.isStreamActive.load() << endl;
            cout << "Stream 2: " << uData2.cp.isStreamActive.load() << endl;
            break;

        case 'q':
            if (uData1.cp.isStreamActive.load())
            {
                newGain = uData1.cp.gainL.load() + changeGainBy;
                uData1.cp.gainL.store(newGain);
                cout << "GainL: " << newGain << endl;
            }
            else if (uData2.cp.isStreamActive.load())
            {
                newGain = uData2.cp.gainL.load() + changeGainBy;
                uData2.cp.gainL.store(newGain);
                cout << "GainL: " << newGain << endl;
            }
            break;
        case 'w':
            if (uData1.cp.isStreamActive.load())
            {
                newGain = uData1.cp.gainR.load() + changeGainBy;
                uData1.cp.gainR.store(newGain);
                cout << "GainR: " << newGain << endl;
            }
            else if (uData2.cp.isStreamActive.load())
            {
                newGain = uData2.cp.gainR.load() + changeGainBy;
                uData2.cp.gainR.store(newGain);
                cout << "GainR: " << newGain << endl;
            }
            break;
        case 'a':
            if (uData1.cp.isStreamActive.load())
            {
                newGain = uData1.cp.gainL.load() - changeGainBy;
                if (newGain > 0)
                    uData1.cp.gainL.store(newGain);

                cout << "GainL: " << uData1.cp.gainL.load() << endl;
            }
            else if (uData2.cp.isStreamActive.load())
            {
                newGain = uData2.cp.gainL.load() - changeGainBy;
                if (newGain > 0)
                    uData2.cp.gainL.store(newGain);
                cout << "GainL: " << uData2.cp.gainL.load() << endl;
            }
            break;
        case 's':
            if (uData1.cp.isStreamActive.load())
            {
                newGain = uData1.cp.gainR.load() - changeGainBy;
                if (newGain > 0)
                    uData1.cp.gainR.store(newGain);
                cout << "GainR: " << uData1.cp.gainR.load() << endl;
            }
            else if (uData2.cp.isStreamActive.load())
            {
                newGain = uData2.cp.gainR.load() - changeGainBy;
                if (newGain > 0)
                    uData2.cp.gainR.store(newGain);
                cout << "GainR: " << uData2.cp.gainR.load() << endl;
            }
            break;
        default:
            keypressed = '*';
            break;
        }
    }
}
