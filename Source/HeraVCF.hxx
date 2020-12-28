#pragma once
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
author: "Jean Pierre Cimalando"
license: "ISC"
name: "HeraVCF"
Code generated with Faust 2.27.2 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 0
------------------------------------------------------------ */

#ifndef __HeraVCF_H__
#define __HeraVCF_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

static float HeraVCF_faustpower2_f(float value)
{
    return (value * value);
}

#ifndef FAUSTCLASS
#define FAUSTCLASS HeraVCF
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif


// BEFORE class //
class HeraVCF {
    // BEGIN class //


private:
    int fSampleRate;
    float fConst0;
    float fConst1;
    float fConst2;
    float fVec0[2];
    float fRec7[2];
    float fRec6[2];
    float fVec1[2];
    float fRec5[2];
    float fRec4[2];
    float fVec2[2];
    float fRec3[2];
    float fRec2[2];
    float fVec3[2];
    float fRec9[2];
    float fRec8[2];
    float fRec1[2];
    float fRec0[2];

public:
    static constexpr int getNumInputs()
    {
        return 3;
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
        fConst1 = (1.96035385f / fConst0);
        fConst2 = (3.14159274f / fConst0);
        // END instanceConstants //
    }

    void instanceResetUserInterface()
    {
        // BEGIN instanceResetUserInterface //
        // END instanceResetUserInterface //
    }

    void instanceClear()
    {
        // BEGIN instanceClear //
        for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
            fVec0[l0] = 0.0f;
        }
        for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
            fRec7[l1] = 0.0f;
        }
        for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
            fRec6[l2] = 0.0f;
        }
        for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
            fVec1[l3] = 0.0f;
        }
        for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
            fRec5[l4] = 0.0f;
        }
        for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
            fRec4[l5] = 0.0f;
        }
        for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
            fVec2[l6] = 0.0f;
        }
        for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
            fRec3[l7] = 0.0f;
        }
        for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
            fRec2[l8] = 0.0f;
        }
        for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
            fVec3[l9] = 0.0f;
        }
        for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
            fRec9[l10] = 0.0f;
        }
        for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
            fRec8[l11] = 0.0f;
        }
        for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
            fRec1[l12] = 0.0f;
        }
        for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
            fRec0[l13] = 0.0f;
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
        FAUSTFLOAT *input1 = inputs[1];
        FAUSTFLOAT *input2 = inputs[2];
        FAUSTFLOAT *output0 = outputs[0];
        for (int i = 0; (i < count); i = (i + 1)) {
            float fTemp0 = float(input0[i]);
            float fTemp1 = float(input1[i]);
            float fTemp2 = float(input2[i]);
            float fTemp3 = std::min<float>(21609.0f, fTemp1);
            float fTemp4 = (fConst2 * fTemp3);
            float fTemp5 = (fTemp3 * (1.0f - fTemp4));
            float fTemp6 = std::min<float>(1.0f, std::max<float>(-1.0f, (1.6025641f * (fTemp0 + (2.0f * (fTemp2 * fRec0[1]))))));
            float fTemp7 = (fTemp4 + 1.0f);
            float fTemp8 = ((fTemp5 * (0.0f - (fRec6[1] + (fTemp6 * (1.0f - (0.333333343f * HeraVCF_faustpower2_f(fTemp6))))))) / fTemp7);
            fVec0[0] = fTemp8;
            fRec7[0] = (fRec7[1] + (fConst1 * (fTemp8 + fVec0[1])));
            float fTemp9 = std::min<float>(1.0f, std::max<float>(-1.0f, (1.6025641f * fRec7[0])));
            fRec6[0] = (fTemp9 * (1.0f - (0.333333343f * HeraVCF_faustpower2_f(fTemp9))));
            float fTemp10 = ((fTemp5 * (fRec6[0] - fRec4[1])) / fTemp7);
            fVec1[0] = fTemp10;
            fRec5[0] = (fRec5[1] + (fConst1 * (fTemp10 + fVec1[1])));
            float fTemp11 = std::min<float>(1.0f, std::max<float>(-1.0f, (1.6025641f * fRec5[0])));
            fRec4[0] = (fTemp11 * (1.0f - (0.333333343f * HeraVCF_faustpower2_f(fTemp11))));
            float fTemp12 = ((fTemp5 * (fRec4[0] - fRec2[1])) / fTemp7);
            fVec2[0] = fTemp12;
            fRec3[0] = (fRec3[1] + (fConst1 * (fTemp12 + fVec2[1])));
            float fTemp13 = std::min<float>(1.0f, std::max<float>(-1.0f, (1.6025641f * fRec3[0])));
            fRec2[0] = (fTemp13 * (1.0f - (0.333333343f * HeraVCF_faustpower2_f(fTemp13))));
            float fTemp14 = ((fTemp5 * (fRec2[0] - fRec8[1])) / fTemp7);
            fVec3[0] = fTemp14;
            float fTemp15 = (fConst1 * (fTemp14 + fVec3[1]));
            fRec9[0] = (fRec9[1] + fTemp15);
            float fTemp16 = std::min<float>(1.0f, std::max<float>(-1.0f, (1.6025641f * fRec9[0])));
            fRec8[0] = (fTemp16 * (1.0f - (0.333333343f * HeraVCF_faustpower2_f(fTemp16))));
            fRec1[0] = (fRec1[1] + fTemp15);
            fRec0[0] = fRec1[0];
            output0[i] = FAUSTFLOAT(fRec0[0]);
            fVec0[1] = fVec0[0];
            fRec7[1] = fRec7[0];
            fRec6[1] = fRec6[0];
            fVec1[1] = fVec1[0];
            fRec5[1] = fRec5[0];
            fRec4[1] = fRec4[0];
            fVec2[1] = fVec2[0];
            fRec3[1] = fRec3[0];
            fRec2[1] = fRec2[0];
            fVec3[1] = fVec3[0];
            fRec9[1] = fRec9[0];
            fRec8[1] = fRec8[0];
            fRec1[1] = fRec1[0];
            fRec0[1] = fRec0[0];
        }
        // END compute //
    }


    // END class //
};
// AFTER class //


#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
