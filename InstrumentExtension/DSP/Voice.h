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

class Voice {
public:
    Voice(double sampleRate = 44100.0, SynthParams* synthParams={}):mVCAEnv(ADSREnvelope::ADSR_TYPE_VCA, synthParams),mVCFEnv(ADSREnvelope::ADSR_TYPE_VCF, synthParams), mResonantFilter(sampleRate, synthParams),mResonantFilter2(sampleRate, synthParams){
        mSampleRate = sampleRate;
        mOsc1 =Oscillator(sampleRate, synthParams);
        mOsc2 =Oscillator(sampleRate, synthParams);
        mSynthParams = synthParams;
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

        float oscillatorOutput = mOsc1.process() + mOsc2.process();

        float vcfEnvelopeControlVoltage = mVCFEnv.process();

        float filterStage1Output = mResonantFilter.process(oscillatorOutput, vcfEnvelopeControlVoltage, mNote);
        float filterState2Output = mResonantFilter2.process(filterStage1Output, vcfEnvelopeControlVoltage, mNote);
        float vcaEnvelopeOutput = mVCAEnv.process() * filterState2Output;

        return vcaEnvelopeOutput;
    }
    
    void noteOn(int note) {
        mNote = note;
        double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
        double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
        mOsc1.setFrequency(osc1freq);
        mOsc2.setFrequency(osc2freq);
        mVCAEnv.noteOn();
        mVCFEnv.noteOn();
    }
    
    void noteOff(int note) {
        mVCAEnv.noteOff();
        mVCFEnv.noteOff();
    }
        
    void recomputeFrequency() {
        double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
        double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
        mOsc1.setFrequency(osc1freq);
        mOsc2.setFrequency(osc2freq);
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
    
    Oscillator mOsc1;
    Oscillator mOsc2;
    ADSREnvelope mVCAEnv;
    ADSREnvelope mVCFEnv;
    BiquadFilter mResonantFilter;
    BiquadFilter mResonantFilter2;
    SynthParams* mSynthParams;
};
