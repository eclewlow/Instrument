//
//  KarlsenFastLadderFilter.h
//  Instrument
//
//  Created by Eugene Clewlow on 2/9/23.
//

#ifndef KarlsenFastLadderFilter_h
#define KarlsenFastLadderFilter_h

/*
 // An updated version of "Karlsen 24dB Filter"
 // This time, the fastest incarnation possible.
 // The very best greetings, Arif Ove Karlsen.
 // arifovekarlsen->hotmail.com

 b_rscl = b_buf4; if (b_rscl > 1) {b_rscl = 1;}
 b_in = (-b_rscl * b_rez) + b_in;
 b_buf1 = ((-b_buf1 + b_in1) * b_cut) + b_buf1;
 b_buf2 = ((-b_buf2 + b_buf1) * b_cut) + b_buf2;
 b_buf3 = ((-b_buf3 + b_buf2) * b_cut) + b_buf3;
 b_buf4 = ((-b_buf4 + b_buf3) * b_cut) + b_buf4;
 b_lpout = b_buf4;
 --------------------------------------------------------
 The parameters are:
 b_cut - cutoff freq
 b_rez - resonance
 b_in1 - input

 Cutoff is normalized frequency in rads (2*pi*cutoff/samplerate). Stability limit for b_cut is around 0.7-0.8.

 There's a typo, the input is sometimes b_in, sometimes b_in1. Anyways why do you use a b_ prefix for all your variables? Wouldn't it be more easy to read like this:

 resoclip = buf4; if (resoclip > 1) resoclip = 1;
 in = in - (resoclip * res);
 buf1 = ((in - buf1) * cut) + buf1;
 buf2 = ((buf1 - buf2) * cut) + buf2;
 buf3 = ((buf2 - buf3) * cut) + buf3;
 buf4 = ((buf3 - buf4) * cut) + buf4;
 lpout = buf4;

 Also note that asymmetrical clipping gives you DC offset (at least that's what I get), so symmetrical clipping is better (and gives a much smoother sound).

 -- peter schoffhauzer
 
 
 */

class KarlsenFastLadderFilter {
public:
    // MARK: Types
    struct FilterState {
        float buf1 = 0.0;
        float buf2 = 0.0;
        float buf3 = 0.0;
        float buf4 = 0.0;

        void clear() {
            buf1 = 0.0;
            buf2 = 0.0;
            buf3 = 0.0;
            buf4 = 0.0;
        }
    };
    
    KarlsenFastLadderFilter(double cutoff, double resonance, double sampleRate = 44100.0) {
        mSampleRate = sampleRate;
        mCutoff = cutoff;
        mResonance = resonance;
        nyquist = 0.5 * sampleRate;
        inverseNyquist = 1.0 / nyquist;
    }
    
    
    double process(float input) {
        
        float resoclip;
        float lpout;
        float cut = inverseNyquist * mCutoff;
//        float cut = 1;
//        float res = pow(10.0, 0.05 * mResonance);
        float res = 4 * (mResonance + 20)/40;
//        float res = mResonance;
//        float res = 0.1;
        
        FilterState& state = filterState;

        
//        Cutoff is normalized frequency in rads (2*pi*cutoff/samplerate). Stability limit for b_cut is around 0.7-0.8.

//        There's a typo, the input is sometimes b_in, sometimes b_in1. Anyways why do you use a b_ prefix for all your variables? Wouldn't it be more easy to read like this:

        resoclip = state.buf4; if (resoclip > 1) resoclip = 1;
        input = input - (resoclip * res);
        state.buf1 = ((input - state.buf1) * cut) + state.buf1;
        state.buf2 = ((state.buf1 - state.buf2) * cut) + state.buf2;
        state.buf3 = ((state.buf2 - state.buf3) * cut) + state.buf3;
        state.buf4 = ((state.buf3 - state.buf4) * cut) + state.buf4;
        lpout = state.buf4;
        
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


#endif /* KarlsenFastLadderFilter_h */
