// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

class HeraVCF_Pirkle
{
public:
    HeraVCF_Pirkle();
    void setSampleRate(float newRate);
    void processNextBlock(float *inputAndOutput, const float *cutoff, const float *resonance, int numSamples);
    void reset();

private:
    float sampleRate = 44100;
    float z[4] = {};
};
