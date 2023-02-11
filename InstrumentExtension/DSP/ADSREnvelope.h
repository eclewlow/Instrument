//
//  ADSREnvelope.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#include "SynthParams.h"

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
        ADSR_TYPE_VCA = 0,
        ADSR_TYPE_VCF = 1,
    } ADSRType;

    typedef enum {
        kEnvelopeRetriggerMode = 0,
        kLegatoMode = 1
    } LegatoMode;

    
    ADSREnvelope(ADSRType adsrType = ADSR_TYPE_VCA, double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
        mDeltaOmega = 1.0 / sampleRate;
        mOmega = 0.0f;
        mLevel = 0.0f;
        mState = kOff;
        mADSRType = adsrType;
    }
    
    double getSampleForOmega(double omega) {
        return 0.0f;
    }

    double process() {
        float attack, decay, sustain, release;
        
        if(mADSRType == ADSR_TYPE_VCA) {
            attack = synthParams.vca_attack;
            decay = synthParams.vca_decay;
            sustain = synthParams.vca_sustain;
            release = synthParams.vca_release;
        } else if(mADSRType == ADSR_TYPE_VCF) {
            attack = synthParams.vcf_attack;
            decay = synthParams.vcf_decay;
            sustain = synthParams.vcf_sustain;
            release = synthParams.vcf_release;
        }

        
        if(mState == kOff) {
            return 0.0f;
        }
        else if(mState == kAttack)
        {
            // attack: y = mx+b
            double m, sample;
            
            if(attack == 0.0f) {
                sample = 1.0f;
            }
            else{
                m = (1.0 - mResumeLevel) / (attack / 1000.0f);
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
            
            if(decay == 0.0f) {
                sample = sustain;
            }
            else {
//                m = (sustain - 1.0) / (decay / 1000.0f);
//                sample = m * mOmega + 1.0f;
                sample = ((1.0-sustain) / pow(decay/1000.0f, 2.0f))*pow((mOmega - decay/1000.0f), 2) + sustain;
            }
            
            mLevel = sample;
            
            if(sample <= sustain  || mOmega >= decay/1000.0f) {
                sample = sustain;
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
            double sample = sustain;
            mLevel = sample;
            mOmega += mDeltaOmega;
            return sample;
        } else if(mState == kRelease) {
            // attack: y = -mx+b
            double m, sample=0.0f;
            
            if(release == 0.0f) {
                sample = 0.0f;
            }
            else {
                /* exp release mode */
                if(mReleaseMode == kExponentialReleaseMode){
                    sample = (mResumeLevel / pow(release/1000.0f, 2.0f))*pow((mOmega - release/1000.0f), 2);

                }
                /* linear release mode */
                else if(mReleaseMode == kLinearReleaseMode) {
                    m = (0.0 - mResumeLevel) / (release / 1000.0f);
                    sample = m * mOmega + mResumeLevel;
                }
            }
            
            mLevel = sample;
            
            if(sample >= 1.0f) {
                sample = 1.0f;
                mLevel = sample;
            }
            
            if(sample <= 0.0f || mOmega >= release/1000.0f) {
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

private:
    State mState = kOff;
    ReleaseMode mReleaseMode = kExponentialReleaseMode;
    LegatoMode mLegatoMode = kLegatoMode;
    ADSRType mADSRType = ADSR_TYPE_VCA;
    
    double mLevel = {0.0};
    double mResumeLevel = {0.0};
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
};
