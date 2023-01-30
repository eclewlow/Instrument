//
//  ADSREnvelope.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#include <iostream>

using namespace std;

class ADSREnvelope {
public:
    typedef enum {
        kOff = 0,
        kAttack =1,
        kDecay =2,
        kSustain =3,
        kRelease =4
    } State;
    
    typedef enum {
        kLinearReleaseMode = 0,
        kExponentialReleaseMode = 1
    } ReleaseMode;

    typedef enum {
        kEnvelopeRetriggerMode = 0,
        kLegatoMode = 1
    } LegatoMode;
//    private enum State {
//        case attack
//        case sustain
//        case decay
//        case release
//    }
    ADSREnvelope(double attack, double decay, double sustain, double release, double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
        mAttack = attack;
        mDecay = decay;
        mSustain = sustain;
        mRelease = release;
        mDeltaOmega = 1.0 / sampleRate;
        mOmega = 0.0f;
        mLevel = 0.0f;
        mState = kOff;
    }
    
    double getSampleForOmega(double omega) {
        return 0.0f;
    }

    double process() {
        if(mState == kOff) {
            return 0.0f;
        }
        else if(mState == kAttack)
        {
            // attack: y = mx+b
            double m, sample;
            
            if(mAttack == 0.0f) {
                sample = 1.0f;
            }
            else{
                m = (1.0 - mResumeLevel) / (mAttack / 1000.0f);
                sample = m * mOmega + mResumeLevel;
            }

            mLevel = sample;

            if(sample >= 1.0) {
                sample = 1.0;
                mLevel = sample;
                mState = kDecay;
                mOmega = 0.0f;
                return sample;
            } else {
                mOmega += mDeltaOmega;
                return sample;
            }
            
        } else if(mState == kDecay) {
            // attack: y = -mx+b
            double m, sample;
            
            if(mDecay == 0.0f) {
                sample = mSustain;
            }
            else {
                m = (mSustain - 1.0) / (mDecay / 1000.0f);
                sample = m * mOmega + 1.0f;
            }
            
            mLevel = sample;
            
            if(sample <= mSustain) {
                sample = mSustain;
                mLevel = sample;
                mState = kSustain;
                mOmega = 0.0f;
                return sample;
            } else {
                mOmega += mDeltaOmega;
                return sample;
            }
        } else if(mState == kSustain) {
            // attack: y = sustain
            double sample = mSustain;
            mLevel = sample;
            mOmega += mDeltaOmega;
            return sample;
        } else if(mState == kRelease) {
            // attack: y = -mx+b
            double m, sample=0.0f;
            
            if(mRelease == 0.0f) {
                sample = 0.0f;
            }
            else {
                /* exp release mode */
                if(mReleaseMode == kExponentialReleaseMode){
                    sample = (mResumeLevel / pow(mRelease/1000.0f, 2.0f))*pow((mOmega - mRelease/1000.0f), 2);

                }
                /* linear release mode */
                else if(mReleaseMode == kLinearReleaseMode) {
                    m = (0.0 - mResumeLevel) / (mRelease / 1000.0f);
                    sample = m * mOmega + mResumeLevel;
                }
            }
            
            mLevel = sample;
            
            if(sample >= 1.0f) {
                sample = 1.0f;
                mLevel = sample;
            }
            
            if(sample <= 0.0f || mOmega >= mRelease/1000.0f) {
                sample = 0.0f;
                mLevel = sample;
                mState = kOff;
                mOmega = 0.0f;
                return sample;
            } else {
                mOmega += mDeltaOmega;
                return sample;
            }
        }
        
        return 0.0f;
    }
    
    void noteOn() {
        mState = kAttack;
        if(mLegatoMode == kEnvelopeRetriggerMode) {
            mResumeLevel = 0.0f;
        }
        else {
            mResumeLevel = mLevel;
        }
        mOmega = 0.0f;
    }
    
    void noteOff() {
        mState = kRelease;
        mResumeLevel = mLevel;
        mOmega = 0.0f;
    }
    
    State getEnvelopeState() const {
        return mState;
    }
    
    void setEnvelope(double attack, double decay, double sustain, double release) {
        mAttack = attack;
        mDecay = decay;
        mSustain = sustain;
        mRelease = release;
    }
    
    void setAttack(double attack) {
        mAttack = attack;
    }

    void setDecay(double decay) {
        mDecay = decay;
    }

    void setSustain(double sustain) {
        mSustain = sustain;
    }

    void setRelease(double release) {
        mRelease = release;
    }

private:
    double mAttack = { 1000.0 };
    double mDecay = { 0.0 };
    double mSustain = { 1.0 };
    double mRelease = { 1000.0 };
    
    State mState = kOff;
    ReleaseMode mReleaseMode = kExponentialReleaseMode;
    LegatoMode mLegatoMode = kLegatoMode;
    
    double mLevel = {0.0};
    double mResumeLevel = {0.0};
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
};
