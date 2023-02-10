//
//  KarlsenLPF.h
//  Instrument
//
//  Created by Eugene Clewlow on 2/9/23.
//

#ifndef KarlsenLPF_h
#define KarlsenLPF_h

class KarlsenLPF {
public:
    // MARK: Types
    struct FilterState {
        float b_inSH = 0.0;
        float b_in = 0.0;
        float b_f = 0.0;
        float b_q = 0.0;
        float b_fp = 0.0;
        float pole1 = 0.0;
        float pole2 = 0.0;
        float pole3 = 0.0;
        float pole4 = 0.0;
        
        void clear() {
             b_inSH = 0.0;
             b_in = 0.0;
             b_f = 0.0;
             b_q = 0.0;
             b_fp = 0.0;
             pole1 = 0.0;
             pole2 = 0.0;
             pole3 = 0.0;
             pole4 = 0.0;
        }
    };
    
    KarlsenLPF(double cutoff, double resonance, double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
        mCutoff = cutoff;
        mResonance = resonance;
        nyquist = 0.5 * sampleRate;
        inverseNyquist = 1.0 / nyquist;
    }
    
    
    double process(float input) {
        
        float resoclip;
        float lpout;
        float freq = mCutoff/20000.0f;
//        float cut = 1;
//        float res = pow(10.0, 0.05 * mResonance);
        float res = 50 * (mResonance + 20)/40;
        
        FilterState& state = filterState;

        /*
         // Karlsen 24dB Filter by Ove Karlsen / Synergy-7 in the year 2003.
         // b_f = frequency 0..1
         // b_q = resonance 0..50
         // b_in = input
         // to do bandpass, subtract poles from eachother, highpass subtract with input.

*/
        state.b_inSH = input;
        state.b_in = input;
        if(freq > 1.0f)freq = 1.0f;
        if(freq < 0.0f)freq = 0.0f;
        state.b_f = freq;
        state.b_q = res;
                
        uint8 b_oversample = 0;

             while (b_oversample < 2) {              //                                2x oversampling (@44.1khz)
                     float prevfp;
                     prevfp = state.b_fp;
                     if (prevfp > 1) {prevfp = 1;} //                                   Q-limiter

                 state.b_fp = (state.b_fp * 0.418) + ((state.b_q * state.pole4) * 0.582);            //     dynamic feedback
                     float intfp;
                     intfp = (state.b_fp * 0.36) + (prevfp * 0.64);  //                       feedback phase
                 state.b_in =  state.b_inSH - intfp;                   //                       inverted feedback

                 state.pole1 = (state.b_in   * state.b_f) + (state.pole1 * (1 - state.b_f));               //     pole 1
                     if (state.pole1 > 1) {state.pole1 = 1;} else if (state.pole1 < -1) {state.pole1 = -1;}  // pole 1 clipping
                 state.pole2 = (state.pole1   * state.b_f) + (state.pole2 * (1 - state.b_f));                   //pole 2
                 state.pole3 = (state.pole2   * state.b_f) + (state.pole3 * (1 - state.b_f));                   //pole 3
                 state.pole4 = (state.pole3   * state.b_f) + (state.pole4 * (1 - state.b_f));                   //pole 4

                     b_oversample++;
                     }
            lpout = state.pole4;
         
        
        return lpout;
    }
    
    void setCutoffResonance(double cutoff, double resonance) {
        mCutoff = cutoff;
        mResonance = resonance;
    }
    
    void setCutoff(double cutoff) {
        mCutoff = cutoff;
    }

    void setResonance(double resonance) {
        mResonance = resonance;
    }

private:
    double mCutoff = { 0 };
    double mResonance = { 8000.0 };
    double nyquist;
    double inverseNyquist;
    FilterState filterState;

    double mSampleRate = { 0.0 };
};



#endif /* KarlsenLPF_h */
