#pragma once
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/* ------------------------------------------------------------
author: "Jean Pierre Cimalando"
license: "ISC"
name: "JpcVCF"
Code generated with Faust 2.27.2 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 0
------------------------------------------------------------ */

#ifndef __JpcVCF_H__
#define __JpcVCF_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/* link with : "" */
#include <algorithm>
#include <cmath>
#include <math.h>

class JpcVCFSIG0 {

private:
    int iRec3[2];

public:
    int getNumInputsJpcVCFSIG0()
    {
        return 0;
    }
    int getNumOutputsJpcVCFSIG0()
    {
        return 1;
    }
    int getInputRateJpcVCFSIG0(int channel)
    {
        int rate;
        switch ((channel)) {
        default: {
            rate = -1;
            break;
        }
        }
        return rate;
    }
    int getOutputRateJpcVCFSIG0(int channel)
    {
        int rate;
        switch ((channel)) {
        case 0: {
            rate = 0;
            break;
        }
        default: {
            rate = -1;
            break;
        }
        }
        return rate;
    }

    void instanceInitJpcVCFSIG0(int sample_rate)
    {
        for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
            iRec3[l0] = 0;
        }
    }

    void fillJpcVCFSIG0(int count, float *table)
    {
        for (int i = 0; (i < count); i = (i + 1)) {
            iRec3[0] = (iRec3[1] + 1);
            table[i] = float(tanhf(float(((0.0472440943f * float((iRec3[0] + -1))) + -3.0f))));
            iRec3[1] = iRec3[0];
        }
    }
};

static JpcVCFSIG0 *newJpcVCFSIG0()
{
    return (JpcVCFSIG0 *)new JpcVCFSIG0();
}
static void deleteJpcVCFSIG0(JpcVCFSIG0 *dsp)
{
    delete dsp;
}

static float ftbl0JpcVCFSIG0[128];

#ifndef FAUSTCLASS
#define FAUSTCLASS JpcVCF
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif


// BEFORE class //
class JpcVCF {
    // BEGIN class //


private:
    int fSampleRate;
    float fConst0;
    float fConst1;
    int iRec10[2];
    float fConst2;
    float fRec8[2];
    float fRec6[2];
    float fRec4[2];
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
        JpcVCFSIG0 *sig0 = newJpcVCFSIG0();
        sig0->instanceInitJpcVCFSIG0(sample_rate);
        sig0->fillJpcVCFSIG0(128, ftbl0JpcVCFSIG0);
        deleteJpcVCFSIG0(sig0);
        // END classInit //
    }

    void instanceConstants(int sample_rate)
    {
        // BEGIN instanceConstants //
        fSampleRate = sample_rate;
        fConst0 = float(fSampleRate);
        fConst1 = (6.28318548f / fConst0);
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
        for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
            iRec10[l1] = 0;
        }
        for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
            fRec8[l2] = 0.0f;
        }
        for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
            fRec6[l3] = 0.0f;
        }
        for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
            fRec4[l4] = 0.0f;
        }
        for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
            fRec1[l5] = 0.0f;
        }
        for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
            fRec0[l6] = 0.0f;
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
            float fTemp3 = (21.166666f * (fRec0[1] + 3.0f));
            int iTemp4 = int(fTemp3);
            float fTemp5 = ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, iTemp4))];
            iRec10[0] = ((1103515245 * iRec10[1]) + 12345);
            float fTemp6 = (4.65661276e-14f * float(iRec10[0]));
            float fTemp7 = (21.166666f * ((fTemp0 + (((fTemp5 + ((fTemp3 - float(iTemp4)) * (ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, (iTemp4 + 1)))] - fTemp5))) * (0.0f - (4.0f * fTemp2))) + fTemp6)) + 3.0f));
            int iTemp8 = int(fTemp7);
            float fTemp9 = ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, iTemp8))];
            float fTemp10 = ((fConst2 * fTemp1) + 1.0f);
            float fTemp11 = ((fTemp1 * ((fTemp9 + ((fTemp7 - float(iTemp8)) * (ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, (iTemp8 + 1)))] - fTemp9))) - fRec8[1])) / fTemp10);
            fRec8[0] = (fRec8[1] + (fConst1 * fTemp11));
            float fRec9 = (fRec8[1] + (fConst2 * fTemp11));
            float fTemp12 = (21.166666f * ((fRec9 + fTemp6) + 3.0f));
            int iTemp13 = int(fTemp12);
            float fTemp14 = ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, iTemp13))];
            float fTemp15 = ((fTemp1 * ((fTemp14 + ((fTemp12 - float(iTemp13)) * (ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, (iTemp13 + 1)))] - fTemp14))) - fRec6[1])) / fTemp10);
            fRec6[0] = (fRec6[1] + (fConst1 * fTemp15));
            float fRec7 = (fRec6[1] + (fConst2 * fTemp15));
            float fTemp16 = (21.166666f * ((fRec7 + fTemp6) + 3.0f));
            int iTemp17 = int(fTemp16);
            float fTemp18 = ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, iTemp17))];
            float fTemp19 = ((fTemp1 * ((fTemp18 + ((fTemp16 - float(iTemp17)) * (ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, (iTemp17 + 1)))] - fTemp18))) - fRec4[1])) / fTemp10);
            fRec4[0] = (fRec4[1] + (fConst1 * fTemp19));
            float fRec5 = (fRec4[1] + (fConst2 * fTemp19));
            float fTemp20 = (21.166666f * ((fRec5 + fTemp6) + 3.0f));
            int iTemp21 = int(fTemp20);
            float fTemp22 = ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, iTemp21))];
            float fTemp23 = ((fTemp1 * ((fTemp22 + ((fTemp20 - float(iTemp21)) * (ftbl0JpcVCFSIG0[std::min<int>(127, std::max<int>(0, (iTemp21 + 1)))] - fTemp22))) - fRec1[1])) / fTemp10);
            fRec1[0] = (fRec1[1] + (fConst1 * fTemp23));
            float fRec2 = (fRec1[1] + (fConst2 * fTemp23));
            fRec0[0] = fRec2;
            output0[i] = FAUSTFLOAT(fRec0[0]);
            iRec10[1] = iRec10[0];
            fRec8[1] = fRec8[0];
            fRec6[1] = fRec6[0];
            fRec4[1] = fRec4[0];
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
