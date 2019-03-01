//
//  Filter.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#ifndef Filter_hpp
#define Filter_hpp

#include "Audio.h"

class FirstOrderFilter {
public:
    float B0;
    float B1;
    float A1;
    float prevX;
    float prevY;
    
    float step(float x)
    {
        float y = B0*x + B1*prevX - A1*prevY;
        prevY = y;
        prevX = x;
        return y;
    }
};

class Filter
{
public:
    Filter (const float _sampleRate, const float _cutoff)
    {
        sampleRate = _sampleRate;
        cutoff = _cutoff;
    }
    float step(float x)
    {
        return coreFilter.step(x);
    }
    float sampleRate;
    float cutoff;
    
    FirstOrderFilter coreFilter;

};

class LowPassFilter : public Filter
{
    LowPassFilter (const float _sampleRate, const float _cutoff) : Filter (_sampleRate, _cutoff)
    {
        float c = sampleRate / M_PI / cutoff;
        float a0i = 1.0 / (1.0 + c);
        coreFilter.B0 = a0i;
        coreFilter.B1 = a0i;
        coreFilter.A1 = (1.0 - c) * a0i;
    }
};


class HighPassFilter : public Filter
{
    HighPassFilter (const float _sampleRate, const float _cutoff) : Filter (_sampleRate, _cutoff)
    {

        float c = sampleRate / M_PI / cutoff;
        float a0i = 1.0 / (1.0 + c);
        
        coreFilter.B0 = c * a0i;
        coreFilter.B1 = -c * a0i;
        coreFilter.A1 = (1.0 - c) * a0i;
    }
};



#if 0
    type FilterChain []Filter
    
    func (fc FilterChain) Step(x float32) float32 {
        if fc != nil {
            for i := range fc {
                x = fc[i].Step(x)
            }
        }
        return x
    }
#endif

#endif /* Filter_hpp */
