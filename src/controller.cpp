// Use to change Configuration Parameters in Real Time
#include<iostream>
#include "../include/global.h"
using namespace std;


void control(CallBackUserData1 &cd1, CallBackUserData2 &cd2){

    char* keyPressed = "*";
    while(true){
        cin >> keyPressed;
        switch(*keyPressed){
            case '1':
                cd1.cp.isStreamActive.store(!cd1.cp.isStreamActive.load());
                break;

            case '2':
                cd2.cp.isStreamActive.store(!cd2.cp.isStreamActive.load());
                break;

            default:
            break; 

        }
    }

}