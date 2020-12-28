#pragma once
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
author: "Jean Pierre Cimalando"
license: "GPL-3.0-or-later"
name: "HeraVCA"
Code generated with Faust 2.27.2 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 0
------------------------------------------------------------ */

#ifndef __HeraVCA_H__
#define __HeraVCA_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>


#ifndef FAUSTCLASS
#define FAUSTCLASS HeraVCA
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif


// BEFORE class //
class HeraVCA {
    // BEGIN class //


private:
    int fSampleRate;
    float fConst0;
    float fConst1;
    FAUSTFLOAT fHslider0;
    float fRec0[2];

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
        fConst0 = std::exp((0.0f - (100.0f / float(fSampleRate))));
        fConst1 = (0.100000001f * (1.0f - fConst0));
        // END instanceConstants //
    }

    void instanceResetUserInterface()
    {
        // BEGIN instanceResetUserInterface //
        fHslider0 = FAUSTFLOAT(0.5f);
        // END instanceResetUserInterface //
    }

    void instanceClear()
    {
        // BEGIN instanceClear //
        for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
            fRec0[l0] = 0.0f;
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
        float fSlow0 = (fConst1 * std::pow(1.25890005f, (10.0f * float(fHslider0))));
        for (int i = 0; (i < count); i = (i + 1)) {
            float fTemp0 = float(input0[i]);
            fRec0[0] = (fSlow0 + (fConst0 * fRec0[1]));
            output0[i] = FAUSTFLOAT((fTemp0 * fRec0[0]));
            fRec0[1] = fRec0[0];
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
