//
//  Voice.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#import "Oscillator.h"
#import "ADSREnvelope.h"
#import "BiquadFilter.h"
#import "SynthParams.h"
#import "ParameterInterpolator.h"

class Voice {
public:
    Voice(double sampleRate = 44100.0, SynthParams* synthParams={}):mVCAEnv(ADSREnvelope::ADSR_TYPE_VCA, synthParams),mVCFEnv(ADSREnvelope::ADSR_TYPE_VCF, synthParams), mResonantFilter(sampleRate, synthParams),mResonantFilter2(sampleRate, synthParams){
        mSampleRate = sampleRate;
        mOsc1 =Oscillator(sampleRate, synthParams);
        mOsc2 =Oscillator(sampleRate, synthParams);
        mSynthParams = synthParams;
        fm_frequency = ParameterInterpolator();
    }
    
    inline double Oscillator2MIDINoteToFrequency(double note) {
        constexpr auto kMiddleA = 440.0;
        double detune = mSynthParams->detune/100.0f;
        double pitchBend = (mSynthParams->pitch_bend - 0x40) * 12.0 / 0x40;
        
        // pitch bend is 0x00 -> 0x40 -> 0x7F
        // this allows for 64 values below middle
        // and allows for 63 values above middle,
        // including the middle this totals to 128 possible values
        return (kMiddleA / 32.0) * pow(2, (((note+detune+pitchBend) - 9.0) / 12.0));
    }
    
    inline double Oscillator1MIDINoteToFrequency(double note) {
        constexpr auto kMiddleA = 440.0;
        double pitchBend = (mSynthParams->pitch_bend - 0x40) * 12.0 / 0x40;
        
        return (kMiddleA / 32.0) * pow(2, (((note+pitchBend) - 9.0) / 12.0));
    }
    
    bool isFinished() const {
        return mVCAEnv.getEnvelopeState() == ADSREnvelope::kOff;
    }
    
    double process() {
        
        /* do FM manipulation here.
         the modulator should calculate normally.
         
         the output of the modulator
         should be sent to the 2nd oscillator and should be added onto its phase
         */
        float oscillatorOutput;
        if(mSynthParams->oscillator_mode == OSCILLATOR_MODE_FM) {

            // add morph 0.5 feedback
            // phase feedback is 0 cancel the next step.
            // modulator_freq * (1+feedback * feedback * 0.5 * previous sample)
            // modulator phase actually gets incremented
            // then modulator feedback, phase temporarily incfemented by 0.25 * feedback *feedback * previous_sample
            float feedback = -1.0;
            float modulator_fb = feedback > 0.0f ? 0.25f * feedback * feedback : 0.0f;
            float phase_feedback = feedback < 0.0f ? 0.5f * feedback * feedback : 0.0f;
            
            fm_frequency.Update(fm_gain_, mSynthParams->fm_gain, 24);
            
            fm_gain_ += fm_frequency.Next();
            
            oscillatorOutput = mOsc1.process((previous_sample_ * phase_feedback) + (previous_sample_ * modulator_fb));
            oscillatorOutput = mOsc2.process(oscillatorOutput * fm_gain_);
            
            previous_sample_ += 0.05 * (oscillatorOutput - previous_sample_);
            
        } else {
            oscillatorOutput = mOsc1.process() + mOsc2.process();
        }
        
        float vcfEnvelopeControlVoltage = mVCFEnv.process();
        
        float filterStage1Output = mResonantFilter.process(oscillatorOutput, vcfEnvelopeControlVoltage, mNote);
        float filterState2Output = mResonantFilter2.process(filterStage1Output, vcfEnvelopeControlVoltage, mNote);
        float vcaEnvelopeOutput = mVCAEnv.process() * filterState2Output;
        
        return vcaEnvelopeOutput;
    }
    
    void noteOn(int note) {
        mNote = note;
        if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_FM) {
            double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
            double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
            mOsc1.setFrequency(osc1freq * mSynthParams->fm_ratio);
            mOsc2.setFrequency(osc2freq);
        } else {
            double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
            double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
            mOsc1.setFrequency(osc1freq);
            mOsc2.setFrequency(osc2freq);
        }
        mVCAEnv.noteOn();
        mVCFEnv.noteOn();
    }
    
    void noteOff(int note) {
        mVCAEnv.noteOff();
        mVCFEnv.noteOff();
    }
    
    void recomputeFrequency() {
        if (mSynthParams->oscillator_mode == OSCILLATOR_MODE_FM) {
            double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
            double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
            mOsc1.setFrequency(osc1freq * mSynthParams->fm_ratio);
            mOsc2.setFrequency(osc2freq);
        } else {
            double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
            double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
            mOsc1.setFrequency(osc1freq);
            mOsc2.setFrequency(osc2freq);
        }
    }
    
    int getNote() const {
        return mNote;
    }
    
    void setNote(int note) {
        mNote = note;
    }
    
private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    int mNote;
    double mFrequency = {0.0f};
    float fm_gain_ = 0.0;
    float fm_feedback_gain_ = 0.0;
    float previous_sample_ = 0.0;
    
    ParameterInterpolator fm_frequency;
    
    Oscillator mOsc1;
    Oscillator mOsc2;
    ADSREnvelope mVCAEnv;
    ADSREnvelope mVCFEnv;
    BiquadFilter mResonantFilter;
    BiquadFilter mResonantFilter2;
    SynthParams* mSynthParams;
};
