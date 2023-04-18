//
//  ParameterInterpolator.h
//  Instrument
//
//  Created by Eugene Clewlow on 4/12/23.
//

#ifndef ParameterInterpolator_h
#define ParameterInterpolator_h


class ParameterInterpolator {
public:
    ParameterInterpolator() { }
    ~ParameterInterpolator() {
    }

    
    inline void Update(float current, float target, size_t size) {
        if(target_ != target) {
            target_ = target;
            tick_ = 0;
            increment_ = (target - current) / size;
            size_ = size;
        } else if(tick_ >= size_) {
            increment_ = 0;
        }
    }
    
    inline float Next() {
        tick_++;
        return increment_;
    }

    
private:
    float* state_;
    float value_;
    float increment_;
    float target_;
    size_t size_;
    uint32_t tick_;
};

#endif /* ParameterInterpolator_h */
