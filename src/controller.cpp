#include "../include/controller.h"
#include <iostream>
#include "global.h"
using namespace std;

void controller(userData1 &uData1, userData2 &uData2)
{
    char keypressed = '*';
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
        default:
            break;
        }
    }
}
