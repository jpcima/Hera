// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "JpcVCF.hxx"

class HeraVCF_Jpc {
public:
    HeraVCF_Jpc()
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
    JpcVCF vcf;
};
