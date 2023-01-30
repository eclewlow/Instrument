//
//  SawtoothOscillator.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>

class SawtoothOscillator{
public:
    SawtoothOscillator(double sampleRate = 44100.0){
        mSampleRate = sampleRate;
    }

    void setFrequency(double frequency) {
        mDeltaOmega = frequency / mSampleRate;
    }

    double getSampleForOmega(double omega) {
        const double sample = 1.0 - 2.0*(omega * 1.0 / (std::numbers::pi_v<double> * 2.0));
        
        // we have to update omega in the calling object. basically increment omega by frequency/samplerate
        return sample;
    }
    
    double process() {
//        return 1.0 - 2.0 * (phase * (1.0 / M_TWOPI));
        const double sample = 1.0 - 2.0*(mOmega * 1.0 / (std::numbers::pi_v<double> * 2.0));
        mOmega += mDeltaOmega;

        if (mOmega >= 1.0) { mOmega -= 1.0; }
        return sample;
    }

private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
};
