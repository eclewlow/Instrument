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
    
    typedef enum {
        OSCILLATOR_MODE_SINE = 0,
        OSCILLATOR_MODE_SAW = 1,
        OSCILLATOR_MODE_SQUARE = 2
    } OscillatorMode;


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
    // This class provides a band-limited oscillator
    double nextSample()
    {
        double value = 0.0; // Init output to avoid nasty surprises
        double t = mOmega; // Define half phase
        
        if (mOscillatorMode == OSCILLATOR_MODE_SINE)
        {
            value = std::sin(mOmega * (std::numbers::pi_v<double> * 2.0));
        }
        
        else if (mOscillatorMode == OSCILLATOR_MODE_SAW)
        {
          value = (2.0 * mOmega) - 1.0; // Render naive waveshape
          value -= poly_blep(t); // Layer output of Poly BLEP on top
        }
        
        else if (mOscillatorMode == OSCILLATOR_MODE_SQUARE)
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
        
        return value; // Output
    }
    double process() {
        const double sample = nextSample();
            
        mOmega += mDeltaOmega;

        if (mOmega >= 1.0) { mOmega -= 1.0; }
        return sample;
    }

private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    OscillatorMode mOscillatorMode = OSCILLATOR_MODE_SAW;
    SynthParams *mSynthParams;
};
