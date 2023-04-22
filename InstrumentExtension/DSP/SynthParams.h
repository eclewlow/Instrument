//
//  SynthParams.h
//  Instrument
//
//  Created by Eugene Clewlow on 2/10/23.
//

#ifndef SynthParams_h
#define SynthParams_h

enum OscillatorMode {
    OSCILLATOR_MODE_SINE = 0,
    OSCILLATOR_MODE_SAW = 1,
    OSCILLATOR_MODE_SQUARE = 2,
    OSCILLATOR_MODE_TRIANGLE = 3,
    OSCILLATOR_MODE_FM = 4
};

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

    float fine_tune = 0.0;
    float coarse_tune = 0.0;

    uint8 pitch_bend = 0x40;
    
    OscillatorMode oscillator_mode = OSCILLATOR_MODE_SAW;
    
    float fm_ratio = 1.0;
    
    float fm_gain = 0.0;
    float fm_feedback = 0.0;
    
    float pulse_width = 50;
    
    bool hard_sync = false;

    bool recompute_frequency = false;
} SynthParams;

#endif /* SynthParams_h */
