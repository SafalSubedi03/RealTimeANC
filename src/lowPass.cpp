#include "../include/lowPass.h"
#include "../include/global.h"
#include <cmath>

void computelphamming()
{
    
    for (int n = 0; n < filterlength; n++)
    {
        lpfParamters::wn[n] =
            0.54f - 0.46f * cos((2 * pi * n) / (filterlength - 1));
    }
}

void computelpfImpuseResponse()
{
    int M = filterlength;
    int centeridx = (M - 1) / 2;

    computelphamming();        

        float fc = 8000; //in hz
        float wc = 2.0f * pi * fc / sampleRate1;

        for (int n = 0; n < M; n++)
        {
            int k = n - centeridx;
            if (k == 0)
                lpfParamters::ha[n] = wc / pi;
            else
                lpfParamters::ha[n] = sin(wc * k) / (pi * k);

            lpfParamters::ha[n] *= lpfParamters::wn[n];
        }

        float sum = 0.0f;
        for (int n = 0; n < M; n++) sum += lpfParamters::ha[n];
        for (int n = 0; n < M; n++) lpfParamters::ha[n] /= sum;        
    
}