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
#import "Oscillator.h"
#import "ADSREnvelope.h"
#import "Voice.h"
#import "SynthParams.h"

#define MAX_VOICES 6

const bool voice_is_off (const Voice v)
   {
       return v.isFinished();
   }

class InstrumentExtensionDSPKernel;

class VoiceManager {
public:
  
    
    VoiceManager(double sampleRate = 44100.0, SynthParams* synthParams={}) {
        mSampleRate = sampleRate;
        mSynthParams = synthParams;
    }

    double process() {
        std::list<Voice>::iterator it;
        
        double sample = 0.0f;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            
            if (mSynthParams->recompute_frequency) {
                (*it).recomputeFrequency();
            }

            
            sample += (*it).process();
        }

        mSynthParams->recompute_frequency = false;

        return sample;
    }

    
    void noteOn(int note) {
        std::list<Voice>::iterator it;
                
        mVoiceList.remove_if(voice_is_off);
        
        int foundNoteIndex = -1;

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
        if(foundNoteIndex < 0) {
            Voice v = Voice(mSampleRate, mSynthParams);
            v.noteOn(note);
            mVoiceList.push_back(v);
        }
    }
    
    void allNotesOff() {
        std::list<Voice>::iterator it;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            (*it).noteOff(0);
        }
    }
    
    void noteOff(int note) {
        std::list<Voice>::iterator it;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            
            if((*it).getNote() == note) {
                (*it).noteOff(note);
            }
        }
    }
    
private:
    double mSampleRate = { 0.0 };
    SynthParams* mSynthParams;
    std::list<Voice>mVoiceList;
};
