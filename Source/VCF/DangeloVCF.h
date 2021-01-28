// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "DangeloVCF.hxx"

class HeraVCF_Dangelo {
public:
    HeraVCF_Dangelo()
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
    DangeloVCF vcf;
};
