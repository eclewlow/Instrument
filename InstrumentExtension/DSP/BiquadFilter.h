//
//  ResonantFilter.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#include "SynthParams.h"


static inline double clamp(double input, double low, double high) {
    return std::min(std::max(input, low), high);
}

static inline float convertBadValuesToZero(float x) {
    /*
     Eliminate denormals, not-a-numbers, and infinities.
     Denormals fails the first test (absx > 1e-15), infinities fails
     the second test (absx < 1e15), and NaNs fails both tests. Zero will
     also fail both tests, but because the system sets it to zero, that's OK.
     */

    float absx = fabs(x);

    if (absx > 1e-15 && absx < 1e15) {
        return x;
    }

    return 0.0;
}


enum {
    FilterParamCutoff = 0,
    FilterParamResonance = 1
};

static inline double squared(double x) {
    return x * x;
}

class BiquadFilter {
public:
    // MARK: Types
    struct FilterState {
        float x1 = 0.0;
        float x2 = 0.0;
        float y1 = 0.0;
        float y2 = 0.0;

        void clear() {
            x1 = 0.0;
            x2 = 0.0;
            y1 = 0.0;
            y2 = 0.0;
        }

        void convertBadStateValuesToZero() {
            /*
             These filters work by feedback. If an infinity or NaN needs to come
             into the filter input, the feedback variables can become infinity
             or NaN, which causes the filter to stop operating. This function
             clears out any bad numbers in the feedback variables.
             */
            x1 = convertBadValuesToZero(x1);
            x2 = convertBadValuesToZero(x2);
            y1 = convertBadValuesToZero(y1);
            y2 = convertBadValuesToZero(y2);
        }
    };

    struct BiquadCoefficients {
        float a1 = 0.0;
        float a2 = 0.0;
        float b0 = 0.0;
        float b1 = 0.0;
        float b2 = 0.0;

        void calculateLopassParams(double frequency, double resonance) {
            /*
             It's possible to replace the transcendental function calls here with
             interpolated table lookups or other approximations.
             
             frequency is Hz / nyquist-freq
             resonance is -20.00 to 20.00 db
             */
            
            // Convert from decibels to linear.
            double r = pow(10.0, 0.05 * -resonance);

            double k  = 0.5 * r * sin(M_PI * frequency);
            double c1 = (1.0 - k) / (1.0 + k);
            double c2 = (1.0 + c1) * cos(M_PI * frequency);
            double c3 = (1.0 + c1 - c2) * 0.25;

            b0 = float(c3);
            b1 = float(2.0 * c3);
            b2 = float(c3);
            a1 = float(-c2);
            a2 = float(c1);
        }

        // Arguments in hertz.
        double magnitudeForFrequency( double inFreq) {
            // Cast to double.
            double _b0 = double(b0);
            double _b1 = double(b1);
            double _b2 = double(b2);
            double _a1 = double(a1);
            double _a2 = double(a2);

            // The frequency on the unit circle in z-plane.
            double zReal      = cos(M_PI * inFreq);
            double zImaginary = sin(M_PI * inFreq);

            // The zeros response.
            double numeratorReal = (_b0 * (squared(zReal) - squared(zImaginary))) + (_b1 * zReal) + _b2;
            double numeratorImaginary = (2.0 * _b0 * zReal * zImaginary) + (_b1 * zImaginary);

            double numeratorMagnitude = sqrt(squared(numeratorReal) + squared(numeratorImaginary));

            // The poles response.
            double denominatorReal = squared(zReal) - squared(zImaginary) + (_a1 * zReal) + _a2;
            double denominatorImaginary = (2.0 * zReal * zImaginary) + (_a1 * zImaginary);

            double denominatorMagnitude = sqrt(squared(denominatorReal) + squared(denominatorImaginary));

            // The total response.
            double response = numeratorMagnitude / denominatorMagnitude;

            return response;
        }
    };

    BiquadFilter(double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
        nyquist = 0.5 * sampleRate;
        inverseNyquist = 1.0 / nyquist;
    }
    
    /*
     input - the input signal to be filtered
     controlVoltage - the CV from a function generator to control added cutoff frequency (values between 0.0 ... 1.0)
        a 0 value results in no cutoff frequency increase, and a 1.0 value results in the maximum cutoff frequency (8500 hz).
        Due to keyboard tracking - it is still possible to have a calculated cutoff (> 8500 hz)
     */
    double process(float input, float controlVoltage = 0.0, float keyboardTrackingNote = 36.0) {
//        double cutoff    = double(cutoffRamper.getAndStep());
//        double resonance = double(resonanceRamper.getAndStep());
        float cutoffGain = pow(2, (keyboardTrackingNote - 36.0) / 12.0);
        
        // if voice note == C0 (12) then cutoff for voice C0 is cutoff_param * 0.5
        // if voice note == C1 (24) then cutoff for voice C1 is cutoff_param * 1
        // if voice note == C2 (36), then cutoff for voice C2 is cutoff_param * 2
        // if voice note == C3 (48), then cutoff for voice C3 is cutoff_param * 2*2
        // if keyboard tracking is 0.0, then cutoff gain is 1.0 always
        // if keyboard tracking is 1.0, then cutoff gain is cutoffgain
        // if keyboardtracking is 0.5
        // (1.0-keytracking) + keytracking * cutoffGain
        // (1.0-0.5) + 0.5 * cutoffgain
        cutoffGain = (1.0-synthParams.vcf_keyboard_tracking_amount) + synthParams.vcf_keyboard_tracking_amount*cutoffGain;
        
        // so cutoff gain should range from 1.0 to cutoffgain
        double calulatedCutoff = synthParams.cutoff*cutoffGain + controlVoltage * (8500.0 - synthParams.cutoff);
        coeffs.calculateLopassParams(
                                     clamp(calulatedCutoff * inverseNyquist, 0.0005444f, 0.9070295f),
                                     clamp(synthParams.resonance, -8.0f, 20.0f)
                                     );
        
//        int frameOffset = int(frameIndex + bufferOffset);
        FilterState& state = filterState;
//        for (int channel = 0; channel < channelCount; ++channel) {
            
//            float* in  = (float*)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
//            float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;

        float output = 0.0;
        
            float x0 = input;
            float y0 = (coeffs.b0 * x0) + (coeffs.b1 * state.x1) + (coeffs.b2 * state.x2) - (coeffs.a1 * state.y1) - (coeffs.a2 * state.y2);
            output = y0;

            state.x2 = state.x1;
            state.x1 = x0;
            state.y2 = state.y1;
            state.y1 = y0;
    
        filterState.convertBadStateValuesToZero();
        
        return output;
    }

private:
    BiquadCoefficients coeffs;
    double nyquist;
    double inverseNyquist;
    FilterState filterState;
    double mSampleRate = { 0.0 };
};
