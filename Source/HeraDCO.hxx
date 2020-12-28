#pragma once
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
author: "Jean Pierre Cimalando"
license: "GPL-3.0-or-later"
name: "HeraDCO"
Code generated with Faust 2.27.2 (https://faust.grame.fr)
Compilation options: -lang cpp -scal -ftz 0
------------------------------------------------------------ */

#ifndef __HeraDCO_H__
#define __HeraDCO_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>


#ifndef FAUSTCLASS
#define FAUSTCLASS HeraDCO
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif


// BEFORE class //
class HeraDCO {
    // BEGIN class //


private:
    FAUSTFLOAT fHslider0;
    FAUSTFLOAT fButton0;
    int fSampleRate;
    float fConst0;
    float fConst1;
    float fRec0[2];
    float fConst2;
    FAUSTFLOAT fHslider1;
    float fRec2[2];
    float fRec1[2];
    FAUSTFLOAT fHslider2;
    float fRec3[2];
    float fRec4[2];
    float fRec5[2];
    float fRec6[2];
    float fRec7[2];
    FAUSTFLOAT fHslider3;
    float fRec8[2];
    float fRec9[2];
    FAUSTFLOAT fHslider4;
    float fRec10[2];
    int iRec12[2];
    float fRec11[4];

public:
    static constexpr int getNumInputs()
    {
        return 2;
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
        fConst2 = (1.0f / fConst0);
        // END instanceConstants //
    }

    void instanceResetUserInterface()
    {
        // BEGIN instanceResetUserInterface //
        fHslider0 = FAUSTFLOAT(0.5f);
        fButton0 = FAUSTFLOAT(0.0f);
        fHslider1 = FAUSTFLOAT(0.0f);
        fHslider2 = FAUSTFLOAT(0.0f);
        fHslider3 = FAUSTFLOAT(0.0f);
        fHslider4 = FAUSTFLOAT(0.0f);
        // END instanceResetUserInterface //
    }

    void instanceClear()
    {
        // BEGIN instanceClear //
        for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
            fRec0[l0] = 0.0f;
        }
        for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
            fRec2[l1] = 0.0f;
        }
        for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
            fRec1[l2] = 0.0f;
        }
        for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
            fRec3[l3] = 0.0f;
        }
        for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
            fRec4[l4] = 0.0f;
        }
        for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
            fRec5[l5] = 0.0f;
        }
        for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
            fRec6[l6] = 0.0f;
        }
        for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
            fRec7[l7] = 0.0f;
        }
        for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
            fRec8[l8] = 0.0f;
        }
        for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
            fRec9[l9] = 0.0f;
        }
        for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
            fRec10[l10] = 0.0f;
        }
        for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
            iRec12[l11] = 0;
        }
        for (int l12 = 0; (l12 < 4); l12 = (l12 + 1)) {
            fRec11[l12] = 0.0f;
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
        FAUSTFLOAT *output0 = outputs[0];
        float fSlow0 = (int(float(fButton0)) ? 0.0f : fConst1);
        float fSlow1 = (1.0f - fSlow0);
        float fSlow2 = ((0.200000003f * float(fHslider0)) * fSlow1);
        float fSlow3 = (float(fHslider1) * fSlow1);
        float fSlow4 = ((0.200000003f * float(fHslider2)) * fSlow1);
        float fSlow5 = ((0.194999993f * float(fHslider3)) * fSlow1);
        float fSlow6 = ((0.209999993f * float(fHslider4)) * fSlow1);
        for (int i = 0; (i < count); i = (i + 1)) {
            fRec0[0] = (fSlow2 + (fSlow0 * fRec0[1]));
            fRec2[0] = (fSlow3 + (fSlow0 * fRec2[1]));
            float fTemp0 = (float(input0[i]) * fRec2[0]);
            float fTemp1 = (fConst2 * fTemp0);
            float fTemp2 = float(int(fRec1[1]));
            fRec1[0] = ((fRec1[1] + fTemp1) - fTemp2);
            float fTemp3 = float(int(fRec1[0]));
            float fTemp4 = (fRec1[0] - fTemp3);
            float fTemp5 = (fRec1[0] + (-1.0f - fTemp3));
            float fTemp6 = ((fTemp4 < fTemp1) ? ((fConst0 * ((fTemp4 * (2.0f - (fConst0 * (fTemp4 / fTemp0)))) / fTemp0)) + -1.0f) : ((((fRec1[0] + fTemp1) - fTemp3) > 1.0f) ? ((fConst0 * ((fTemp5 * ((fConst0 * (fTemp5 / fTemp0)) + 2.0f)) / fTemp0)) + 1.0f) : 0.0f));
            fRec3[0] = (fSlow4 + (fSlow0 * fRec3[1]));
            int iTemp7 = (fRec1[0] >= 1.0f);
            float fTemp8 = float(input1[i]);
            fRec4[0] = (iTemp7 ? (0.5f - (0.449999988f * fTemp8)) : fRec4[1]);
            int iTemp9 = (fTemp4 > fRec4[0]);
            float fTemp10 = (0.949999988f * fTemp8);
            fRec5[0] = (iTemp7 ? (0.449999988f * (2.0f - fTemp10)) : fRec5[1]);
            fRec6[0] = (iTemp7 ? -1.0f : (iTemp9 ? fRec6[1] : (fConst1 * fRec6[1])));
            fRec7[0] = (iTemp7 ? (1.0f - fTemp10) : (iTemp9 ? (fConst1 * fRec7[1]) : fRec7[1]));
            float fTemp11 = (fRec4[0] + fTemp3);
            float fTemp12 = (fRec1[0] - fTemp11);
            float fTemp13 = ((fTemp12 < 0.0f) ? (fRec1[0] + (1.0f - fTemp11)) : fTemp12);
            float fTemp14 = (fTemp13 + -1.0f);
            fRec8[0] = (fSlow5 + (fSlow0 * fRec8[1]));
            fRec9[0] = ((((fRec1[1] - fTemp2) < 0.5f) & (fTemp4 >= 0.5f)) ? ((fRec9[1] > 0.0f) ? -1.0f : 1.0f) : (fConst1 * fRec9[1]));
            float fTemp15 = (fRec1[0] + (-0.5f - fTemp3));
            float fTemp16 = ((fTemp15 < 0.0f) ? (fRec1[0] + (0.5f - fTemp3)) : fTemp15);
            float fTemp17 = (fTemp16 + -1.0f);
            fRec10[0] = (fSlow6 + (fSlow0 * fRec10[1]));
            iRec12[0] = ((1103515245 * iRec12[1]) + 12345);
            fRec11[0] = (((0.522189379f * fRec11[3]) + ((4.65661287e-10f * float(iRec12[0])) + (2.49495602f * fRec11[1]))) - (2.0172658f * fRec11[2]));
            output0[i] = FAUSTFLOAT((((0.25999999f * (((fRec0[0] * ((2.0f * fTemp4) + (-1.0f - fTemp6))) + (fRec3[0] * ((iTemp9 ? fRec7[0] : fRec6[0]) - (fRec5[0] * (fTemp6 - ((fTemp13 < fTemp1) ? ((fConst0 * ((fTemp13 * (2.0f - (fConst0 * (fTemp13 / fTemp0)))) / fTemp0)) + -1.0f) : (((fTemp1 + fTemp13) > 1.0f) ? ((fConst0 * ((fTemp14 * ((fConst0 * (fTemp14 / fTemp0)) + 2.0f)) / fTemp0)) + 1.0f) : 0.0f))))))) + (fRec8[0] * (fRec9[0] - (fConst1 * (fRec9[1] * ((fTemp16 < fTemp1) ? ((fConst0 * ((fTemp16 * (2.0f - (fConst0 * (fTemp16 / fTemp0)))) / fTemp0)) + -1.0f) : (((fTemp1 + fTemp16) > 1.0f) ? ((fConst0 * ((fTemp17 * ((fConst0 * (fTemp17 / fTemp0)) + 2.0f)) / fTemp0)) + 1.0f) : 0.0f)))))))) + (3.25f * (fRec10[0] * (((0.0499220341f * fRec11[0]) + (0.0506126992f * fRec11[2])) - ((0.0959935337f * fRec11[1]) + (0.00440878607f * fRec11[3])))))) / ((0.300000012f * (std::max<float>((((fRec0[0] + fRec3[0]) + fRec8[0]) + fRec10[0]), 0.25999999f) + -0.25999999f)) + 0.25999999f)));
            fRec0[1] = fRec0[0];
            fRec2[1] = fRec2[0];
            fRec1[1] = fRec1[0];
            fRec3[1] = fRec3[0];
            fRec4[1] = fRec4[0];
            fRec5[1] = fRec5[0];
            fRec6[1] = fRec6[0];
            fRec7[1] = fRec7[0];
            fRec8[1] = fRec8[0];
            fRec9[1] = fRec9[0];
            fRec10[1] = fRec10[0];
            iRec12[1] = iRec12[0];
            for (int j0 = 3; (j0 > 0); j0 = (j0 - 1)) {
                fRec11[j0] = fRec11[(j0 - 1)];
            }
        }
        // END compute //
    }


    FAUSTFLOAT getSmoothDisabled() const
    {
        return fButton0;
    }
    void setSmoothDisabled(FAUSTFLOAT value)
    {
        fButton0 = value;
    }

    FAUSTFLOAT getFrequency() const
    {
        return fHslider1;
    }
    void setFrequency(FAUSTFLOAT value)
    {
        fHslider1 = value;
    }

    FAUSTFLOAT getSawLevel() const
    {
        return fHslider0;
    }
    void setSawLevel(FAUSTFLOAT value)
    {
        fHslider0 = value;
    }

    FAUSTFLOAT getPulseLevel() const
    {
        return fHslider2;
    }
    void setPulseLevel(FAUSTFLOAT value)
    {
        fHslider2 = value;
    }

    FAUSTFLOAT getSubLevel() const
    {
        return fHslider3;
    }
    void setSubLevel(FAUSTFLOAT value)
    {
        fHslider3 = value;
    }

    FAUSTFLOAT getNoiseLevel() const
    {
        return fHslider4;
    }
    void setNoiseLevel(FAUSTFLOAT value)
    {
        fHslider4 = value;
    }

    // END class //
};
// AFTER class //


#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
