//
//  VoiceManager.h
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#include <numbers>
#include <cmath>
#include <list>
#import "SinOscillator.h"
#import "SawtoothOscillator.h"
#import "ADSREnvelope.h"
#import "Voice.h"
#import "InstrumentExtensionDSPKernel.hpp"

#define MAX_VOICES 6

const bool voice_is_off (const Voice v)
   {
       return v.isFinished();
   }

class InstrumentExtensionDSPKernel;

class VoiceManager {
public:
  
    
    VoiceManager(double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
    }
    
    inline double MIDINoteToFrequency(int note) {
        constexpr auto kMiddleA = 440.0;
        return (kMiddleA / 32.0) * pow(2, ((note - 9) / 12.0));
    }

//    void setFrequency(double frequency) {
//        mDeltaOmega = frequency / mSampleRate;
//    }

    void setADSREnvelope(double attack, double decay, double sustain, double release) {
        std::list<Voice>::iterator it;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            (*it).set( attack,  decay,  sustain,  release);
        }
        
         mVCAAttack = attack;
         mVCADecay = decay;
         mVCASustain = sustain;
         mVCARelease = release;
    }

    void setADSREnvelope(double attack, double decay, double sustain, double release) {
        std::list<Voice>::iterator it;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            (*it).setADSREnvelope( attack,  decay,  sustain,  release);
        }
        
         mVCAAttack = attack;
         mVCADecay = decay;
         mVCASustain = sustain;
         mVCARelease = release;
    }

    void setCutoffResonance(double cutoff, double resonance) {
        std::list<Voice>::iterator it;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            (*it).setCutoffResonance(cutoff, resonance);
        }
        
        mCutoff = cutoff;
        mResonance = resonance;
    }

    void setDetune(int detuneCents) {
        std::list<Voice>::iterator it;
        mOsc2DetuneCents = detuneCents;

        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            (*it).setDetune(detuneCents);
        }
    }


    double process() {
        std::list<Voice>::iterator it;
        
        double sample = 0.0f;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            sample += (*it).process();
//            sample = fmax(sample, (*it).process());
        }
//        sample = sample / mVoiceList.size();
        
        
        return sample;
//        return mADSREnv.process() * (mOsc1.process() + mOsc2.process()) / 2.0f;
    }

    
    void noteOn(int note) {
//        mNote = note;
        std::list<Voice>::iterator it;
        
        mVoiceList.remove_if(voice_is_off);
        
        int foundNoteIndex = -1;
//        int freeNoteIndex = -1;
        int i = 0;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            if((*it).getNote() == note) {
//                (*it).noteOff(note);
                (*it).noteOn(note);
                foundNoteIndex = i;
                break;
            }
            i++;
        }
//        i = 0;
//        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
//            if((*it).getNote() == -1) {
//                freeNoteIndex = i;
//                break;
//            }
//            i++;
//        }
        if(foundNoteIndex < 0) {
            // else , if we have voices left over, push a new voice with env generator reset
//            if(mVoiceList.size() >= MAX_VOICES) {
//                Voice v = mVoiceList.front();
//                mVoiceList.erase(mVoiceList.begin());
//                v.noteOn(note);
//                mVoiceList.push_back(v);
//                // if we've run out of voices.  Get the oldest voice and set it's frequency rather than start the envelope generator over
//            } else {
                Voice v = Voice(mVCAAttack, mVCADecay, mVCASustain, mVCARelease, mOsc2DetuneCents, mPitchBend, mCutoff, mResonance, mSampleRate);
                v.noteOn(note);
                mVoiceList.push_back(v);
//            }
        } else {
//            Voice v = *it;
//            mVoiceList.remove(v);
//            mVoiceList.erase(it);
//            v.noteOn(note);
//            mVoiceList.push_back(v);
        }
    }
    
    void noteOff(int note) {
//        mADSREnv.noteOff();
        std::list<Voice>::iterator it;
//        mVoiceList
//        mVoiceList.remove_if([note](Voice v){ return v.getNote() == note; });
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            
            if((*it).getNote() == note) {
                (*it).noteOff(note);
            }
        }
    }
    
    void setPitchBend(uint8 pitchBend) {
        std::list<Voice>::iterator it;
        mPitchBend = pitchBend;

        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            (*it).setPitchBend(pitchBend);
        }

    }
    
private:
    double mOmega = { 0.0 };
    double mDeltaOmega = { 0.0 };
    double mSampleRate = { 0.0 };
    int mNote;
    int mOsc2DetuneCents = {0};
    uint8 mPitchBend = 0x40;
    std::list<Voice>mVoiceList;
    
    double mVCAAttack = 10.0f;
    double mVCADecay = 0.0f;
    double mVCASustain = 1.0f;
    double mVCARelease = 100.0f;
    double mCutoff = 0;
    double mResonance = 0;
};
