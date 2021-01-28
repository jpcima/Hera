#pragma once
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
author: "Jean Pierre Cimalando"
license: "GPL-3.0-or-later"
name: "HeraHPF"
Code generated with Faust 2.27.2 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 0
------------------------------------------------------------ */

#ifndef __HeraHPF_H__
#define __HeraHPF_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/* link with : "" */
#include "HeraTables.h"
#include <algorithm>
#include <cmath>
#include <math.h>


#ifndef FAUSTCLASS
#define FAUSTCLASS HeraHPF
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif


// BEFORE class //
class HeraHPF {
    // BEGIN class //


private:
    FAUSTFLOAT fHslider0;
    int fSampleRate;
    float fConst0;
    float fConst1;
    float fConst2;
    float fRec0[2];
    float fConst3;
    float fRec1[2];
    float fRec2[2];

public:
    static constexpr int getNumInputs()
    {
        return 1;
    }
    static constexpr int getNumOutputs()
    {
        return 1;
    }

    static void classInit(int sample_rate)
    {
        // BEGIN classInit //
        // END classInit //
    }

    void instanceConstants(int sample_rate)
    {
        // BEGIN instanceConstants //
        fSampleRate = sample_rate;
        fConst0 = float(fSampleRate);
        fConst1 = std::exp((0.0f - (100.0f / fConst0)));
        fConst2 = (1.0f - fConst1);
        fConst3 = (0.0f - (6.28318548f / fConst0));
        // END instanceConstants //
    }

    void instanceResetUserInterface()
    {
        // BEGIN instanceResetUserInterface //
        fHslider0 = FAUSTFLOAT(0.0f);
        // END instanceResetUserInterface //
    }

    void instanceClear()
    {
        // BEGIN instanceClear //
        for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
            fRec0[l0] = 0.0f;
        }
        for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
            fRec1[l1] = 0.0f;
        }
        for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
            fRec2[l2] = 0.0f;
        }
        // END instanceClear //
    }

    void init(int sample_rate)
    {
        // BEGIN init //
        classInit(sample_rate);
        instanceInit(sample_rate);
        // END init //
    }
    void instanceInit(int sample_rate)
    {
        // BEGIN instanceInit //
        instanceConstants(sample_rate);
        instanceResetUserInterface();
        instanceClear();
        // END instanceInit //
    }


    int getSampleRate()
    {
        return fSampleRate;
    }


    void compute(int count, FAUSTFLOAT **inputs, FAUSTFLOAT **outputs)
    {
        // BEGIN compute //
        FAUSTFLOAT *input0 = inputs[0];
        FAUSTFLOAT *output0 = outputs[0];
        float fSlow0 = float(fHslider0);
        float fSlow1 = float(curveFromHpfSliderToFreq(float(fSlow0)));
        int iSlow2 = (fSlow1 > 0.0f);
        float fSlow3 = (fConst2 * std::min<float>(1.0f, (4.0f * fSlow0)));
        float fSlow4 = (fConst2 * std::exp((fConst3 * fSlow1)));
        for (int i = 0; (i < count); i = (i + 1)) {
            float fTemp0 = float(input0[i]);
            fRec0[0] = (fSlow3 + (fConst1 * fRec0[1]));
            fRec1[0] = ((fConst1 * fRec1[1]) - fSlow4);
            float fTemp1 = ((fTemp0 * (fRec1[0] + 1.0f)) + fRec2[1]);
            fRec2[0] = (0.0f - (fRec1[0] * fTemp1));
            output0[i] = FAUSTFLOAT((iSlow2 ? (fTemp0 - (fRec0[0] * fTemp1)) : fTemp0));
            fRec0[1] = fRec0[0];
            fRec1[1] = fRec1[0];
            fRec2[1] = fRec2[0];
        }
        // END compute //
    }


    FAUSTFLOAT getAmount() const
    {
        return fHslider0;
    }
    void setAmount(FAUSTFLOAT value)
    {
        fHslider0 = value;
    }

    // END class //
};
// AFTER class //


#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
