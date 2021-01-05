// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "HeraVCF.hxx"
#include "LerpTable.h"

class LadderFilterNew {
public:
    LadderFilterNew()
    {
        vcf.init(44100);
    }

    void setSampleRate(float newRate)
    {
        vcf.classInit(newRate);
        vcf.instanceConstants(newRate);
        vcf.instanceClear();
    }

    void processNextBlock(float *inputAndOutput, const float *cutoff, const float *resonance, int numSamples)
    {
        const float *inputs[] = { inputAndOutput, cutoff, resonance};
        float *outputs[] = { inputAndOutput };
        vcf.compute(numSamples, (float **)inputs, outputs);
    }

    void reset()
    {
        vcf.instanceClear();
    }

private:
    HeraVCF vcf;
};

class LadderFilterOld
{
public:
    enum { ModeLPF2, ModeLPF4, ModeBPF2, ModeBPF4, ModeHPF2, ModeHPF4 };

    LadderFilterOld();
    void setSampleRate(float newRate);
    void processNextBlock(float *inputAndOutput, const float *cutoff, const float *resonance, int numSamples);
    void reset();

private:
    static const LerpTable &getCutoffFactorTable();

private:
    float sampleRate = 44100;
    float z[4] = {};
    int mode = ModeLPF4;
};

using LadderFilter = LadderFilterOld;
