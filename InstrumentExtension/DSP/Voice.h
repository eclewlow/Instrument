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

class Voice {
public:
    Voice(double vcaAttack, double vcaDecay, double vcaSustain, double vcaRelease, int osc2DetuneCents = 0, double sampleRate = 44100.0):mADSREnv(vcaAttack, vcaDecay, vcaSustain, vcaRelease, mSampleRate) {
        mSampleRate = sampleRate;
        mOsc1 =SawtoothOscillator(mSampleRate);
        mOsc2 =SawtoothOscillator(mSampleRate);
        mADSREnv = ADSREnvelope(vcaAttack, vcaDecay, vcaSustain, vcaRelease, mSampleRate);
        mOsc2DetuneCents = osc2DetuneCents;
    }

    inline double MIDINoteToFrequency(int note) {
        constexpr auto kMiddleA = 440.0;
        return (kMiddleA / 32.0) * pow(2, ((note - 9) / 12.0));
    }

    inline double MIDINoteDoubleToFrequency(double note, double detune=0.0f) {
        constexpr auto kMiddleA = 440.0;
        return (kMiddleA / 32.0) * pow(2, (((note+detune) - 9.0) / 12.0));
    }

//    void setFrequency(double frequency) {
//        mDeltaOmega = frequency / mSampleRate;
//    }

    bool isFinished() const {
        return mADSREnv.getEnvelopeState() == ADSREnvelope::kOff;
    }
    
    double process() {
        return mADSREnv.process() * (mOsc1.process() + mOsc2.process()) / 2.0f;
    }
    
    void noteOn(int note) {
        mNote = note;
        double osc1freq = MIDINoteDoubleToFrequency(mNote, 0.0f);
        double osc2freq = MIDINoteDoubleToFrequency(mNote, mOsc2DetuneCents/100.0f);
        mOsc1.setFrequency(osc1freq);
        mOsc2.setFrequency(osc2freq);
        mADSREnv.noteOn();
    }
    
    void noteOff(int note) {
        mNote = -1;
        mADSREnv.noteOff();
    }
    
    void setDetune(int detuneCents) {
        mOsc2DetuneCents = detuneCents;
        double osc2freq = MIDINoteDoubleToFrequency(mNote, detuneCents/100.0f);
        mOsc2.setFrequency(osc2freq);
    }
    
    int getNote() const {
        return mNote;
    }
    
    void setNote(int note) {
        mNote = note;
    }
    
    void setADSREnvelope(double attack, double decay, double sustain, double release) {
        mADSREnv.setEnvelope( attack,  decay,  sustain,  release);
    }
    
private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    int mNote;
    int mOsc2DetuneCents = {0};
    double mFrequency = {0.0f};
    
    SawtoothOscillator mOsc1;
    SawtoothOscillator mOsc2;
    ADSREnvelope mADSREnv;
};
