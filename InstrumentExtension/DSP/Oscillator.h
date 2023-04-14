//
//  Oscillator.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#include "SynthParams.h"

class Oscillator{
public:
    Oscillator(double sampleRate = 44100.0, SynthParams *synthParams={}){
        mSampleRate = sampleRate;
        mSynthParams = synthParams;
    }
    
    void setFrequency(double frequency) {
        mDeltaOmega = frequency / mSampleRate;
    }
    
    // This function calculates the PolyBLEPs
    double poly_blep(double t)
    {
        double dt = mDeltaOmega;
        
        // t-t^2/2 +1/2
        // 0 < t <= 1
        // discontinuities between 0 & 1
        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0;
        }
        
        // t^2/2 +t +1/2
        // -1 <= t <= 0
        // discontinuities between -1 & 0
        else if (t > 1 - dt)
        {
            t = (t - 1.0) / dt;
            return t * t + t + t + 1.0;
        }
        
        // no discontinuities
        // 0 otherwise
        else return 0.0;
    }
    
    inline float ThisBlepSample(float t) {
        return 0.5f * t * t;
    }
    
    inline float NextBlepSample(float t) {
        t = 1.0f - t;
        return -0.5f * t * t;
    }
    
    // This class provides a band-limited oscillator
    double nextSample(double fm_sample = 0.0)
    {
        double value = 0.0; // Init output to avoid nasty surprises
        double t = mOmega; // Define half phase
        
        if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_SINE)
        {
            value = std::sin(mOmega * (std::numbers::pi_v<double> * 2.0));
        }
        
        else if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_SAW)
        {
            //            value = (2.0 * mOmega) - 1.0; // Render naive waveshape
            //            value -= poly_blep(t); // Layer output of Poly BLEP on top
            float tempPhase = mOmega;
            
            float this_sample = next_sample;
            next_sample = 0.0f;
            
            //            const float frequency = fm.Next();
            
            tempPhase += mDeltaOmega;
            
            if (tempPhase >= 1.0f) {
                tempPhase -= 1.0f;
                float t = tempPhase / mDeltaOmega;
                this_sample -= ThisBlepSample(t);
                next_sample -= NextBlepSample(t);
            }
            
            next_sample += tempPhase;
            value += (2.0f * this_sample - 1.0f) * 1.0;
            
        }
        else if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_SQUARE)
        {
            if (mOmega < 0.5)
            {
                value = 1.0; // Flip
            } else {
                value = -1.0; // Flop
            }
            value += poly_blep(t); // Layer output of Poly BLEP on top (flip)
            value -= poly_blep(fmod(t + 0.5, 1.0)); // Layer output of Poly BLEP on top (flop)
        }
        
        else if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_TRIANGLE)
        {
            if (mOmega < 0.5)
            {
                value = 1.0; // Flip
            } else {
                value = -1.0; // Flop
            }
            value += poly_blep(t); // Layer output of Poly BLEP on top (flip)
            value -= poly_blep(fmod(t + 0.5, 1.0)); // Layer output of Poly BLEP on top (flop)
            
            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
            value = mDeltaOmega * (std::numbers::pi_v<double> * 2.0) * value + (1 - mDeltaOmega * (std::numbers::pi_v<double> * 2.0)) * lastOutput;
            lastOutput = value;
        }
        else if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_FM)
        {
            value = std::sin((mOmega + fm_sample) * (std::numbers::pi_v<double> * 2.0));
        }

        return value; // Output
    }
    double process(double fm_input = 0.0) {
        double sample;
        
        sample = nextSample(fm_input);
        
        mOmega += mDeltaOmega;
        
        if (mOmega >= 1.0) { mOmega -= 1.0; }
        return sample;
    }
    
private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    double lastOutput = 0.0;
    float next_sample=0.0;
    SynthParams *mSynthParams;
};
