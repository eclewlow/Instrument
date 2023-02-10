//
//  ResonantFilter.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>


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
//    typedef enum {
//        kOff = 0,
//        kAttack =1,
//        kDecay =2,
//        kSustain =3,
//        kRelease =4
//    } State;
//
//    typedef enum {
//        kLinearReleaseMode = 0,
//        kExponentialReleaseMode = 1
//    } ReleaseMode;
//
//    typedef enum {
//        kEnvelopeRetriggerMode = 0,
//        kLegatoMode = 1
//    } LegatoMode;
//    private enum State {
//        case attack
//        case sustain
//        case decay
//        case release
//    }
    BiquadFilter(double cutoff, double resonance, double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
        mCutoff = cutoff;
        mResonance = resonance;
        nyquist = 0.5 * sampleRate;
        inverseNyquist = 1.0 / nyquist;

//        mSustain = sustain;
//        mRelease = release;
//        mDeltaOmega = 1.0 / sampleRate;
//        mOmega = 0.0f;
//        mLevel = 0.0f;
//        mState = kOff;
    }
    
    
    double process(float input) {
//        double cutoff    = double(cutoffRamper.getAndStep());
//        double resonance = double(resonanceRamper.getAndStep());
        coeffs.calculateLopassParams(
                                     clamp(mCutoff * inverseNyquist, 0.0005444f, 0.9070295f),
                                     clamp(mResonance, -20.0f, 50.0f)
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
    
//    void noteOn() {
//        mState = kAttack;
//        if(mLegatoMode == kEnvelopeRetriggerMode) {
//            mResumeLevel = 0.0f;
//        }
//        else {
//            mResumeLevel = mLevel;
//        }
//        mOmega = 0.0f;
//    }
//
//    void noteOff() {
//        mState = kRelease;
//        mResumeLevel = mLevel;
//        mOmega = 0.0f;
//    }
    
    void setCutoffResonance(double cutoff, double resonance) {
        mCutoff = cutoff;
        mResonance = resonance;
    }
    
    void setCutoff(double cutoff) {
        mCutoff = cutoff;
    }

    void setResonance(double resonance) {
        mResonance = resonance;
    }

private:
    BiquadCoefficients coeffs;
    double mCutoff = { 0.0 };
    double mResonance = { 0.0 };
    double nyquist;
    double inverseNyquist;
    FilterState filterState;
//    State mState = kOff;
//    ReleaseMode mReleaseMode = kExponentialReleaseMode;
//    LegatoMode mLegatoMode = kLegatoMode;
//
//    double mLevel = {0.0};
//    double mResumeLevel = {0.0};
//    double mOmega = { 0.0 };
//    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
};
