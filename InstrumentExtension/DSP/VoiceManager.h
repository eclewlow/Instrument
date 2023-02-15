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

//    void setFrequency(double frequency) {
//        mDeltaOmega = frequency / mSampleRate;
//    }

    double process() {
        std::list<Voice>::iterator it;
        
        double sample = 0.0f;
        for (it = mVoiceList.begin(); it!= mVoiceList.end(); it++) {
            
            if (synthParams.recompute_frequency) {
                (*it).recomputeFrequency();
            }

            
            sample += (*it).process();
        }

        synthParams.recompute_frequency = false;

        return sample;
    }

    
    void noteOn(int note) {
        std::list<Voice>::iterator it;
        
//        printf("num_voices=%d\n", mVoiceList.size());
        
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
                Voice v = Voice();
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
    
private:
    double mSampleRate = { 0.0 };
    std::list<Voice>mVoiceList;
};
