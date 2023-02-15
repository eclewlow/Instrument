//
//  Voice.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#import "SinOscillator.h"
#import "SawtoothOscillator.h"
#import "ADSREnvelope.h"
#import "BiquadFilter.h"
#import "SynthParams.h"

class Voice {
public:
    Voice(double sampleRate = 44100.0):mVCAEnv(ADSREnvelope::ADSR_TYPE_VCA),mVCFEnv(ADSREnvelope::ADSR_TYPE_VCF), mResonantFilter(),mResonantFilter2(){
        mSampleRate = sampleRate;
        mOsc1 =SawtoothOscillator();
        mOsc2 =SawtoothOscillator();
    }

    inline double Oscillator2MIDINoteToFrequency(double note) {
        constexpr auto kMiddleA = 440.0;
        double detune = synthParams.detune/100.0f;
        double pitchBend = (synthParams.pitch_bend - 0x40) * 12.0 / 0x40;
        
        // pitch bend is 0x00 -> 0x40 -> 0x7F
        // this allows for 64 values below middle
        // and allows for 63 values above middle,
        // including the middle this totals to 128 possible values
        return (kMiddleA / 32.0) * pow(2, (((note+detune+pitchBend) - 9.0) / 12.0));
//        return (kMiddleA / 32.0) * pow(2, (((note) - 9.0) / 12.0));
    }

    inline double Oscillator1MIDINoteToFrequency(double note) {
        constexpr auto kMiddleA = 440.0;
        double pitchBend = (synthParams.pitch_bend - 0x40) * 12.0 / 0x40;
//        printf("%lf\n", pitchBend);
        return (kMiddleA / 32.0) * pow(2, (((note+pitchBend) - 9.0) / 12.0));
//        return (kMiddleA / 32.0) * pow(2, (((note) - 9.0) / 12.0));
    }

//    void setFrequency(double frequency) {
//        mDeltaOmega = frequency / mSampleRate;
//    }

    bool isFinished() const {
        return mVCAEnv.getEnvelopeState() == ADSREnvelope::kOff;
    }
    
    double process() {
//        if (synthParams.recompute_frequency) {
//            recomputeFrequency();
//            synthParams.recompute_frequency = false;
//        }
//        return mADSREnv.process() * mKarlsenFastLadderFilter.process(mOsc1.process() + mOsc2.process());
//                return mADSREnv.process() * mResonantFilter.process(mOsc1.process() + mOsc2.process());
        float oscillatorOutput = mOsc1.process() + mOsc2.process();

        float vcfEnvelopeControlVoltage = mVCFEnv.process() * synthParams.vcf_amount;
        
//        vcfEnvelopeControlVoltage = 0.0;
//        printf("%lf\n", vcfEnvelopeControlVoltage);

        float filterStage1Output = mResonantFilter.process(oscillatorOutput, vcfEnvelopeControlVoltage);
        float filterState2Output = mResonantFilter2.process(filterStage1Output, vcfEnvelopeControlVoltage);
        float vcaEnvelopeOutput = mVCAEnv.process() * filterState2Output;
//                return mADSREnv.process() * mKarlsenLPF.process(mOsc1.process() + mOsc2.process());
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
//        mNote = -1;
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
    
    SawtoothOscillator mOsc1;
    SawtoothOscillator mOsc2;
    ADSREnvelope mVCAEnv;
    ADSREnvelope mVCFEnv;
    BiquadFilter mResonantFilter;
    BiquadFilter mResonantFilter2;
//    KarlsenFastLadderFilter mKarlsenFastLadderFilter;
//    KarlsenLPF mKarlsenLPF;
};
