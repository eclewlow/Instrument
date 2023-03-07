//
//  SynthParams.h
//  Instrument
//
//  Created by Eugene Clewlow on 2/10/23.
//

#ifndef SynthParams_h
#define SynthParams_h

typedef struct SynthParams {
    float vca_attack = 10.0;
    float vca_decay = 0.0;
    float vca_sustain = 1.0;
    float vca_release = 100.0;

    float vcf_attack = 10.0;
    float vcf_decay = 0.0;
    float vcf_sustain = 1.0;
    float vcf_release = 100.0;
    
    float vcf_envelope_amount = 0.0;
    float vcf_keyboard_tracking_amount = 0.0;

    float cutoff = 8500.0;
    float resonance = -8.0;

    float detune = 0.0;
    
    uint8 pitch_bend = 0x40;
    
    bool recompute_frequency = false;
} SynthParams;

#endif /* SynthParams_h */
