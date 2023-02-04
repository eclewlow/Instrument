//
//  FIRFilter.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>



/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 44100 Hz

* 20 Hz - 10000 Hz
  gain = 1
  desired ripple = 0.1 dB
  actual ripple = 0.041061797730060086 dB

* 18000 Hz - 22050 Hz
  gain = 0
  desired attenuation = -72 dB
  actual attenuation = -77.0843677290094 dB

*/

#define FILTER_TAP_NUM 19

static double filter_taps[FILTER_TAP_NUM] = {
  -0.0025578140340492194,
  0.0030474868447096094,
  0.00916864988061756,
  -0.017627431971416634,
  -0.0060858866669883105,
  0.052654869522672273,
  -0.04114353446302967,
  -0.09419378388974067,
  0.2911746139096847,
  0.6134896646112482,
  0.2911746139096847,
  -0.09419378388974067,
  -0.04114353446302967,
  0.052654869522672273,
  -0.0060858866669883105,
  -0.017627431971416634,
  0.00916864988061756,
  0.0030474868447096094,
  -0.0025578140340492194
};


typedef struct
{
    float buf[FILTER_TAP_NUM];
    unsigned int bufIndex;
    
    float out;
} FIRFilter;

void FIRFilter_Init(FIRFilter * fir) {
    for (unsigned int n = 0; n < FILTER_TAP_NUM; n++) {
        fir->buf[n] = 0.0;
    }
    
    fir->bufIndex = 0;
    
    fir->out = 0;
};

float FIRFilter_Update(FIRFilter * fir, float inp) {
    
    fir->buf[fir->bufIndex] = inp;

    fir->bufIndex++;

    if(fir->bufIndex == FILTER_TAP_NUM) {
        fir->bufIndex = 0;
    }

    fir->out = 0.0f;

    unsigned int sumIndex = fir->bufIndex;

    for (unsigned int n = 0; n < FILTER_TAP_NUM; n++) {
        if(sumIndex > 0) {
            sumIndex--;
        } else {
            sumIndex =FILTER_TAP_NUM-1;
        }

        fir->out += filter_taps[n] * fir->buf[sumIndex];
    }
    return fir->out;
}
