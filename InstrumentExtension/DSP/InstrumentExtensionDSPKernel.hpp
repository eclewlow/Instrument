//
//  InstrumentExtensionDSPKernel.hpp
//  InstrumentExtension
//
//  Created by Eugene Clewlow on 1/25/23.
//

#pragma once

#import <AudioToolbox/AudioToolbox.h>
#import <CoreMIDI/CoreMIDI.h>
#import <algorithm>
#import <vector>
#import <span>

#import "SinOscillator.h"
#import "SawtoothOscillator.h"
#import "ADSREnvelope.h"
#import "Voice.h"
#import "VoiceManager.h"
#import "InstrumentExtension-Swift.h"
#import "InstrumentExtensionParameterAddresses.h"
#import "FIRFilter.h"

/*
 InstrumentExtensionDSPKernel
 As a non-ObjC class, this is safe to use from render thread.
 */
class InstrumentExtensionDSPKernel {
public:
    void initialize(int channelCount, double inSampleRate) {
        mSampleRate = inSampleRate;
        mVoiceManager = VoiceManager(mVCAAttack, mVCADecay, mVCASustain, mVCARelease, mDetune, mCutoff, mResonance, inSampleRate);
        FIRFilter_Init(&mFilter);
    }
    
    void deInitialize() {
    }
    
    // MARK: - Bypass
    bool isBypassed() {
        return mBypassed;
    }
    
    void setBypass(bool shouldBypass) {
        mBypassed = shouldBypass;
    }
    
    // MARK: - Parameter Getter / Setter
    // Add a case for each parameter in InstrumentExtensionParameterAddresses.h
    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case InstrumentExtensionParameterAddress::gain:
                mGain = value;
                break;
            case InstrumentExtensionParameterAddress::attack:
                mVCAAttack = value;
                mVoiceManager.setADSREnvelope(mVCAAttack, mVCADecay, mVCASustain, mVCARelease);
                break;    
            case InstrumentExtensionParameterAddress::release:
                mVCARelease = value;
                mVoiceManager.setADSREnvelope(mVCAAttack, mVCADecay, mVCASustain, mVCARelease);
                break;
            case InstrumentExtensionParameterAddress::cutoff:
                mCutoff = value;
                mVoiceManager.setCutoffResonance(mCutoff, mResonance);
                break;
            case InstrumentExtensionParameterAddress::resonance:
                mResonance = value;
                mVoiceManager.setCutoffResonance(mCutoff, mResonance);
                break;
            case InstrumentExtensionParameterAddress::detune:
                mDetune = value;
//                mVoice.setDetune(mDetune);
                mVoiceManager.setDetune(mDetune);
                break;
        }
    }
    
    AUValue getParameter(AUParameterAddress address) {
        // Return the goal. It is not thread safe to return the ramping value.
        
        switch (address) {
            case InstrumentExtensionParameterAddress::gain:
                return (AUValue)mGain;
            case InstrumentExtensionParameterAddress::attack:
                return (AUValue) mVCAAttack;
            case InstrumentExtensionParameterAddress::release:
                return (AUValue) mVCARelease;
            case InstrumentExtensionParameterAddress::detune:
                return (AUValue) mDetune;
            case InstrumentExtensionParameterAddress::cutoff:
                return (AUValue) mCutoff;
            case InstrumentExtensionParameterAddress::resonance:
                return (AUValue) mResonance;
            default: return 0.f;
        }
    }
    
    // MARK: - Max Frames
    AUAudioFrameCount maximumFramesToRender() const {
        return mMaxFramesToRender;
    }
    
    void setMaximumFramesToRender(const AUAudioFrameCount &maxFrames) {
        mMaxFramesToRender = maxFrames;
    }
    
    // MARK: - Musical Context
    void setMusicalContextBlock(AUHostMusicalContextBlock contextBlock) {
        mMusicalContextBlock = contextBlock;
    }
    
    // MARK: - MIDI Protocol
    MIDIProtocolID AudioUnitMIDIProtocol() const {
        return kMIDIProtocol_2_0;
    }
    
    inline double MIDINoteToFrequency(int note) {
        constexpr auto kMiddleA = 440.0;
        return (kMiddleA / 32.0) * pow(2, ((note - 9) / 12.0));
    }
    
    /**
     MARK: - Internal Process
     
     This function does the core siginal processing.
     Do your custom DSP here.
     */
    void process(std::span<float *> outputBuffers, AUEventSampleTime bufferStartTime, AUAudioFrameCount frameCount) {
        if (mBypassed) {
            // Fill the 'outputBuffers' with silence
            for (UInt32 channel = 0; channel < outputBuffers.size(); ++channel) {
                std::fill_n(outputBuffers[channel], frameCount, 0.f);
            }
            return;
        }
        
        // Use this to get Musical context info from the Plugin Host,
        // Replace nullptr with &memberVariable according to the AUHostMusicalContextBlock function signature
        if (mMusicalContextBlock) {
            mMusicalContextBlock(nullptr /* currentTempo */,
                                 nullptr /* timeSignatureNumerator */,
                                 nullptr /* timeSignatureDenominator */,
                                 nullptr /* currentBeatPosition */,
                                 nullptr /* sampleOffsetToNextBeat */,
                                 nullptr /* currentMeasureDownbeatPosition */);
        }
        
        // Generate per sample dsp before assigning it to out
        for (UInt32 frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            // Do your frame by frame dsp here...
//            const auto sample = mADSREnv.process() * mSawOsc.process() * mNoteEnvelope * mGain;
            const double sample = mVoiceManager.process();// * mNoteEnvelope * mGain;
//            FIRFilter_Update(&mFilter, sample);
//            const auto sample =mSawOsc.process() * mNoteEnvelope * mGain;
//                        const auto sample =mSawOsc.process() * mNoteEnvelope * mGain;

            for (UInt32 channel = 0; channel < outputBuffers.size(); ++channel) {
                outputBuffers[channel][frameIndex] = sample;
            }
        }
    }
    
    void handleOneEvent(AUEventSampleTime now, AURenderEvent const *event) {
        switch (event->head.eventType) {
            case AURenderEventParameter: {
                handleParameterEvent(now, event->parameter);
                break;
            }
                
            case AURenderEventMIDIEventList: {
                handleMIDIEventList(now, &event->MIDIEventsList);
                break;
            }
                
            default:
                break;
        }
    }
    
    void handleParameterEvent(AUEventSampleTime now, AUParameterEvent const& parameterEvent) {
        // Implement handling incoming Parameter events as needed
//        mVoice.setDetune(mDetune);
//        fprintf(
//          stdout,
//          "ParameterEvent: parameterIndex=%d time=%.3f value=%.4f\n",
//                parameterEvent.eventType,parameterEvent.eventSampleTime, parameterEvent.value);
//                parameterEvent.getIndex(),
//                parameterEvent.getTime(),
//                parameterEvent.getValue(),
//                parameterEvent.getSource()
    }
    
    void handleMIDIEventList(AUEventSampleTime now, AUMIDIEventList const* midiEvent) {
        auto visitor = [] (void* context, MIDITimeStamp timeStamp, MIDIUniversalMessage message) {
            auto thisObject = static_cast<InstrumentExtensionDSPKernel *>(context);
            
            switch (message.type) {
                case kMIDIMessageTypeChannelVoice2: {
                    thisObject->handleMIDI2VoiceMessage(message);
                }
                    break;
                 
                case kMIDIMessageTypeChannelVoice1: {
                    thisObject->handleMIDI1VoiceMessage(message);
                }
                    break;
                default:
                    break;
            }
        };
        
        MIDIEventListForEachEvent(&midiEvent->eventList, visitor, this);
    }
    void handleMIDI1VoiceMessage(const struct MIDIUniversalMessage& message) {
        switch (message.channelVoice1.status) {
            case kMIDICVStatusPitchBend: {
            }
                break;
            default:
                break;
        }
    }
    void handleMIDI2VoiceMessage(const struct MIDIUniversalMessage& message) {
        const auto& note = message.channelVoice2.note;
        
        switch (message.channelVoice2.status) {
            case kMIDICVStatusNoteOff: {
//                mNoteEnvelope = 0.0;
                mVoiceManager.noteOff(note.number);
            }
                break;
                
            case kMIDICVStatusNoteOn: {
//                const auto velocity = message.channelVoice2.note.velocity;
//                const auto freqHertz   = MIDINoteToFrequency(note.number);

                mVoiceManager.noteOn(note.number);

//                printf("%d\n", note.number) ;

//                mVoice.setFrequency(freqHertz);
                // Set frequency on per channel oscillator
                // Use velocity to set amp envelope level
//                mNoteEnvelope = (double)velocity / (double)std::numeric_limits<std::uint16_t>::max();
                mNoteEnvelope = 1.0f;
            }
                break;
            case kMIDICVStatusPitchBend: {
//                printf("%x\n", message.channelVoice2.pitchBend.data);
                uint8 pitchBend = (message.channelVoice2.pitchBend.data >> 25) & 0x7F;
//                                printf("%x\n", pitchBend);
                mVoiceManager.setPitchBend(pitchBend);
//                printf("%x\n", message.channelVoice1.pitchBend);
//                printf("%x\n", message.channelVoice2.pitchBend.data);
//                printf("%x\n", message.channelVoice2.pitchBend.reserved[0]);
//                printf("%x\n", message.channelVoice2.pitchBend.reserved[1]);
//                NSLog(<#NSString * _Nonnull format, ...#>)
//                0111 1111
                
            }
                break;
            default:
                break;
        }
    }
    
    // MARK: - Member Variables
    AUHostMusicalContextBlock mMusicalContextBlock;
    
    double mSampleRate = 44100.0;
    double mGain = 1.0;
    double mAttack = 0.0;
    double mVCAAttack = 10.0f;
    double mVCADecay = 0.0f;
    double mVCASustain = 1.0f;
    double mVCARelease = 100.0f;
    int mDetune = 0;
    double mNoteEnvelope = 0.0;
    double mADSREnvelope = 0.0f;
    double mCutoff=0;
    double mResonance=0;
    
    bool mBypassed = false;
    AUAudioFrameCount mMaxFramesToRender = 1024;
    
    VoiceManager mVoiceManager;
    FIRFilter mFilter;
};
