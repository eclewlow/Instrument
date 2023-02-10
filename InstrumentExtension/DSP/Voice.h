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
#import "KarlsenLPF.h"
#import "KarlsenFastLadderFilter.h"

class Voice {
public:
    Voice(double vcaAttack, double vcaDecay, double vcaSustain, double vcaRelease, int osc2DetuneCents, uint8 pitchBend, double cutoff, double resonance, double sampleRate = 44100.0):mADSREnv(vcaAttack, vcaDecay, vcaSustain, vcaRelease, sampleRate),mVCFEnvelope(vcaAttack, vcaDecay, vcaSustain, vcaRelease, sampleRate), mResonantFilter(cutoff, resonance, sampleRate),mResonantFilter2(cutoff, resonance, sampleRate),mKarlsenFastLadderFilter(cutoff, resonance, sampleRate), mKarlsenLPF(cutoff, resonance, sampleRate) {
        mSampleRate = sampleRate;
        mOsc1 =SawtoothOscillator(mSampleRate);
        mOsc2 =SawtoothOscillator(mSampleRate);
        mADSREnv = ADSREnvelope(vcaAttack, vcaDecay, vcaSustain, vcaRelease, mSampleRate);
        mOsc2DetuneCents = osc2DetuneCents;
        mPitchBend = pitchBend;
    }

    inline double Oscillator2MIDINoteToFrequency(double note) {
        constexpr auto kMiddleA = 440.0;
        double detune = mOsc2DetuneCents/100.0f;
        double pitchBend = (mPitchBend - 0x40) * 12.0 / 0x40;
        // pitch bend is 0x00 -> 0x40 -> 0x7F
        // this allows for 64 values below middle
        // and allows for 63 values above middle,
        // including the middle this totals to 128 possible values
        return (kMiddleA / 32.0) * pow(2, (((note+detune+pitchBend) - 9.0) / 12.0));
    }

    inline double Oscillator1MIDINoteToFrequency(double note) {
        constexpr auto kMiddleA = 440.0;
        double pitchBend = (mPitchBend - 0x40) * 12.0 / 0x40;
//        printf("%lf\n", pitchBend);
        return (kMiddleA / 32.0) * pow(2, (((note+pitchBend) - 9.0) / 12.0));
    }

//    void setFrequency(double frequency) {
//        mDeltaOmega = frequency / mSampleRate;
//    }

    bool isFinished() const {
        return mADSREnv.getEnvelopeState() == ADSREnvelope::kOff;
    }
    
    double process() {
//        return mADSREnv.process() * mKarlsenFastLadderFilter.process(mOsc1.process() + mOsc2.process());
//                return mADSREnv.process() * mResonantFilter.process(mOsc1.process() + mOsc2.process());
        return mADSREnv.process() * mResonantFilter2.process(mResonantFilter.process(mOsc1.process() + mOsc2.process()));
//                return mADSREnv.process() * mKarlsenLPF.process(mOsc1.process() + mOsc2.process());
    }
    
    void noteOn(int note) {
        mNote = note;
        double osc1freq = Oscillator1MIDINoteToFrequency(mNote);
        double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
        mOsc1.setFrequency(osc1freq);
        mOsc2.setFrequency(osc2freq);
        mADSREnv.noteOn();
    }
    
    void noteOff(int note) {
//        mNote = -1;
        mADSREnv.noteOff();
    }
    
    void setDetune(int detuneCents) {
        mOsc2DetuneCents = detuneCents;
        double osc2freq = Oscillator2MIDINoteToFrequency(mNote);
        mOsc2.setFrequency(osc2freq);
    }
    
    void setPitchBend(uint8 pitchBend) {
        mPitchBend = pitchBend;
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
    
//    void setADSREnvelope(double attack, double decay, double sustain, double release) {
//        mADSREnv.setEnvelope( attack,  decay,  sustain,  release);
//    }
    
    void setADSREnvelopeAttack(double attack) {
        mADSREnv.setAttack(attack);
    }

    void setADSREnvelopeDecay(double decay) {
        mADSREnv.setDecay(decay);
    }

    void setADSREnvelopeSustain(double sustain) {
        mADSREnv.setSustain(sustain);
    }

    void setADSREnvelopeRelease(double release) {
        mADSREnv.setRelease(release);
    }

    void setCutoff(double cutoff) {
        mResonantFilter.setCutoff(cutoff);
        mResonantFilter2.setCutoff(cutoff);
        mKarlsenFastLadderFilter.setCutoff(cutoff);
        mKarlsenLPF.setCutoff(cutoff);
    }

    void setResonance(double resonance) {
        mResonantFilter.setResonance(resonance);
        mResonantFilter2.setResonance(resonance);
        mKarlsenFastLadderFilter.setResonance(resonance);
        mKarlsenLPF.setResonance(resonance);
    }

    void setCutoffResonance(double cutoff, double resonance) {
        mResonantFilter.setCutoffResonance(cutoff, resonance);
        mResonantFilter2.setCutoffResonance(cutoff, resonance);
        mKarlsenFastLadderFilter.setCutoffResonance(cutoff, resonance);
        mKarlsenLPF.setCutoffResonance(cutoff, resonance);
    }
    
private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    int mNote;
    int mOsc2DetuneCents = {0};
    double mFrequency = {0.0f};
    uint8 mPitchBend = 0x40;
    
    SawtoothOscillator mOsc1;
    SawtoothOscillator mOsc2;
    ADSREnvelope mADSREnv;
    ADSREnvelope mVCFEnvelope;
    BiquadFilter mResonantFilter;
    BiquadFilter mResonantFilter2;
    KarlsenFastLadderFilter mKarlsenFastLadderFilter;
    KarlsenLPF mKarlsenLPF;
};
