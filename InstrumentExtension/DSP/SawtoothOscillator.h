//
//  SawtoothOscillator.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#import "FIRFilter.h"

class SawtoothOscillator{
public:
    SawtoothOscillator(double sampleRate = 96000.0){
        mSampleRate = sampleRate;
        FIRFilter_Init(&mFilter);
    }
    
    typedef enum {
        OSCILLATOR_MODE_SINE = 0,
        OSCILLATOR_MODE_SAW = 1,
        OSCILLATOR_MODE_SQUARE = 2
    } OscillatorMode;


    void setFrequency(double frequency) {
        mDeltaOmega = frequency / mSampleRate;
    }

    double getSampleForOmega(double omega) {
        // ramp down wave
//        const double sample = 1.0 - 2.0*(omega * 1.0 / (std::numbers::pi_v<double> * 2.0));

        // ramp up wave
        const double sample = (2.0 * (omega * (1.0 / (M_PI * 2.0)))) - 1.0;

        // we have to update omega in the calling object. basically increment omega by frequency/samplerate
        return sample;
    }
    // This function calculates the PolyBLEPs
    
//    double t = 0.; // 0 <= t < 1
//    double dt = freq / sample_rate;
//
//    ...
//
//    double poly_blep(double t, double dt)
//    {
//      // 0 <= t < 1
//      if (t < dt)
//      {
//        t /= dt;
//        // 2 * (t - t^2/2 - 0.5)
//        return t+t - t*t - 1.;
//      }
//
//      // -1 < t < 0
//      else if (t > 1. - dt)
//      {
//        t = (t - 1.) / dt;
//        // 2 * (t^2/2 + t + 0.5)
//        return t*t + t+t + 1.;
//      }
//
//      // 0 otherwise
//      else
//      {
//        return 0.;
//      }
//    }
//
//    double poly_saw(double t, double dt)
//    {
//      // Correct phase, so it would be in line with sin(2.*M_PI * t)
//      t += 0.5;
//      if (t >= 1.) t -= 1.;
//
//      double naive_saw = 2.*t - 1.;
//      return naive_saw - poly_blep(t, dt);
//    }
//
//    ...
//
//    for (int i = 0; i < nFrames; ++i)
//    {
//      output[i] = poly_saw(t, dt);
//      t += dt;
//    }
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
//          value = sin(mOmega); // No harmonics in sine so no aliasing!! No Poly BLEPs needed!
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
        // ramp down wave
//        const double sample = 1.0 - 2.0*(mOmega * 1.0 / (std::numbers::pi_v<double> * 2.0));
        // ramp up wave
//        const double sample = (2.0 * (mOmega)) - 1.0;
        const double sample = nextSample();
//        double omega = mOmega;
//        if(mOmega >= 0.25 && mOmega < 0.50 || mOmega< 1.0 && mOmega >= 0.75) {
            
//        }
//        double sample = std::abs( std::sin(mOmega * (std::numbers::pi_v<double> * 2.0)));
//        if(mOmega >= 0.25 && mOmega < 0.50 || mOmega< 1.0 && mOmega >= 0.75)
            
//        FIRFilter_Update(&mFilter, sample);
        mOmega += mDeltaOmega;

        if (mOmega >= 1.0) { mOmega -= 1.0; }
        return sample;
    }

private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    OscillatorMode mOscillatorMode = OSCILLATOR_MODE_SAW;
    FIRFilter mFilter;
};
