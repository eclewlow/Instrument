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

#import "VoiceManager.h"
#import "InstrumentExtension-Swift.h"
#import "InstrumentExtensionParameterAddresses.h"
#import "SynthParams.h"

/*
 InstrumentExtensionDSPKernel
 As a non-ObjC class, this is safe to use from render thread.
 */

class InstrumentExtensionDSPKernel {
public:
    void initialize(int channelCount, double inSampleRate) {
        mSampleRate = inSampleRate;
        mVoiceManager = VoiceManager(mSampleRate, &synthParams);
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
            case InstrumentExtensionParameterAddress::vca_attack:
                synthParams.vca_attack = value;
                break;
            case InstrumentExtensionParameterAddress::vca_decay:
                synthParams.vca_decay = value;
                break;
            case InstrumentExtensionParameterAddress::vca_sustain:
                synthParams.vca_sustain = value;
                break;
            case InstrumentExtensionParameterAddress::vca_release:
                synthParams.vca_release = value;
                break;
            case InstrumentExtensionParameterAddress::vcf_attack:
                synthParams.vcf_attack = value;
                break;
            case InstrumentExtensionParameterAddress::vcf_decay:
                synthParams.vcf_decay = value;
                break;
            case InstrumentExtensionParameterAddress::vcf_sustain:
                synthParams.vcf_sustain = value;
                break;
            case InstrumentExtensionParameterAddress::vcf_release:
                synthParams.vcf_release = value;
                break;
            case InstrumentExtensionParameterAddress::cutoff:
                synthParams.cutoff = value;
                break;
            case InstrumentExtensionParameterAddress::resonance:
                synthParams.resonance = value;
                break;
            case InstrumentExtensionParameterAddress::detune:
                synthParams.detune = value;
                synthParams.recompute_frequency = true;
                break;
            case InstrumentExtensionParameterAddress::vcf_envelope_amount:
                synthParams.vcf_envelope_amount = value;
                break;
            case InstrumentExtensionParameterAddress::vcf_keyboard_tracking_amount:
                synthParams.vcf_keyboard_tracking_amount = value;
                break;
            case InstrumentExtensionParameterAddress::oscillator_mode:
                synthParams.oscillator_mode = (OscillatorMode)value;
                synthParams.recompute_frequency = true;
                break;
            case InstrumentExtensionParameterAddress::fm_ratio:
                synthParams.fm_ratio = value;
                synthParams.recompute_frequency = true;
                break;
            case InstrumentExtensionParameterAddress::fm_gain:
                synthParams.fm_gain = value;
                break;
        }
    }
    
    AUValue getParameter(AUParameterAddress address) {
        // Return the goal. It is not thread safe to return the ramping value.
        
        switch (address) {
            case InstrumentExtensionParameterAddress::gain:
                return (AUValue)mGain;
            case InstrumentExtensionParameterAddress::vca_attack:
                return (AUValue) synthParams.vca_attack;
            case InstrumentExtensionParameterAddress::vca_decay:
                return (AUValue) synthParams.vca_decay;
            case InstrumentExtensionParameterAddress::vca_sustain:
                return (AUValue) synthParams.vca_sustain;
            case InstrumentExtensionParameterAddress::vca_release:
                return (AUValue) synthParams.vca_release;
            case InstrumentExtensionParameterAddress::vcf_attack:
                return (AUValue) synthParams.vcf_attack;
            case InstrumentExtensionParameterAddress::vcf_decay:
                return (AUValue) synthParams.vcf_decay;
            case InstrumentExtensionParameterAddress::vcf_sustain:
                return (AUValue) synthParams.vcf_sustain;
            case InstrumentExtensionParameterAddress::vcf_release:
                return (AUValue) synthParams.vcf_release;
            case InstrumentExtensionParameterAddress::detune:
                return (AUValue) synthParams.detune;
            case InstrumentExtensionParameterAddress::cutoff:
                return (AUValue) synthParams.cutoff;
            case InstrumentExtensionParameterAddress::resonance:
                return (AUValue) synthParams.resonance;
            case InstrumentExtensionParameterAddress::vcf_envelope_amount:
                return (AUValue) synthParams.vcf_envelope_amount;
            case InstrumentExtensionParameterAddress::vcf_keyboard_tracking_amount:
                return (AUValue) synthParams.vcf_keyboard_tracking_amount;
            case InstrumentExtensionParameterAddress::oscillator_mode:
                return (AUValue) synthParams.oscillator_mode;
            case InstrumentExtensionParameterAddress::fm_ratio:
                return (AUValue) synthParams.fm_ratio;
            case InstrumentExtensionParameterAddress::fm_gain:
                return (AUValue) synthParams.fm_gain;
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
    
    inline void reset() {
        mVoiceManager.allNotesOff();
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
            const double sample = mVoiceManager.process();// * mGain;

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
            
//            printf("voice message = %x\n", message.channelVoice2.status);

            switch (message.type) {
                case kMIDIMessageTypeChannelVoice2: {
                    thisObject->handleMIDI2VoiceMessage(message);
                }
                    break;
                 
                case kMIDIMessageTypeChannelVoice1: {
                    thisObject->handleMIDI1VoiceMessage(message);
                }
                    break;
                case kMIDIMessageTypeSystem: {
                    thisObject->handleMIDISystemMessage(message);
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
/*
 kMIDICVStatusProgramChange        =    0xC,
 kMIDICVStatusChannelPressure    =    0xD,
 kMIDICVStatusPitchBend            =    0xE,

 // MIDI 2.0
 kMIDICVStatusRegisteredPNC            =     0x0, // Per-Note Controller
 kMIDICVStatusAssignablePNC            =    0x1,
 kMIDICVStatusRegisteredControl        =    0x2, // Registered Parameter Number (RPN)
 */
        switch (message.channelVoice2.status) {
            case kMIDICVStatusRegisteredControl: {
            }
                break;
            case kMIDICVStatusProgramChange: {
//                printf("program change bank = %x\n", message.channelVoice2.programChange.bank);
//                printf("program change program = %x\n", message.channelVoice2.programChange.program);
//                printf("program change options= %x\n", message.channelVoice2.programChange.options);
//                mVoiceManager.allNotesOff();
            }
                break;
            case kMIDICVStatusControlChange: {
//                printf("control change index= %x\n", message.channelVoice2.controlChange.index);
//                printf("control change data= %x\n", message.channelVoice2.controlChange.data);
            }
                break;
                
            case kMIDICVStatusNoteOff: {
//                printf("note off = %x\n", note.number);
                mVoiceManager.noteOff(note.number);
            }
                break;
                
            case kMIDICVStatusNoteOn: {
//                printf("note on = %x\n", note.number);

                mVoiceManager.noteOn(note.number);
                synthParams.recompute_frequency = true;
            }
                break;
            case kMIDICVStatusPitchBend: {

                uint8 pitchBend = (message.channelVoice2.pitchBend.data >> 25) & 0x7F;

                synthParams.pitch_bend = pitchBend;
                synthParams.recompute_frequency = true;
            }
                break;
            default:
                break;
        }
    }
    void handleMIDISystemMessage(const struct MIDIUniversalMessage& message) {
        switch (message.system.status) {
            case kMIDIStatusStop: {
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
    
    bool mBypassed = false;
    AUAudioFrameCount mMaxFramesToRender = 1024;
    
    VoiceManager mVoiceManager;
    SynthParams synthParams;
};
