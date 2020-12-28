// SPDX-License-Identifier: GPL-3.0-or-later

#include "LadderFilter.h"
#include <cmath>

static constexpr float coeffsOfMode[][5] = {
    /* LPF2 */ { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
    /* LPF4 */ { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
    /* BPF2 */ { 0.0f, 2.0f, -2.0f, 0.0f, 0.0f },
    /* BPF4 */ { 0.0f, 0.0f, 4.0f, -8.0f, 4.0f },
    /* HPF2 */ { 1.0f, -2.0f, 1.0f, 0.0f, 0.0f },
    /* HPF4 */ { 1.0f, -4.0f, 6.0f, -4.0f, 1.0f },
};

///
LadderFilterOld::LadderFilterOld()
{
    getCutoffFactorTable();
}

void LadderFilterOld::setSampleRate(float newRate)
{
    sampleRate = newRate;
    reset();
}

void LadderFilterOld::processNextBlock(float *inputAndOutput, const float *cutoff, const float *resonance, int numSamples)
{
    float samplePeriod = 1.0f / sampleRate;
    const LerpTable &cutoffTable = getCutoffFactorTable();

    float z1 = z[0];
    float z2 = z[1];
    float z3 = z[2];
    float z4 = z[3];

    const float *coeffs = coeffsOfMode[mode];

    for (int i = 0; i < numSamples; ++i) {
        float cutoffFactor = cutoffTable(cutoff[i] * samplePeriod);
        float oneOverOnePlusg = 1.0f / (1.0f + cutoffFactor);

        // Feedforward coefficient for VA one-pole filters.
        float alpha = cutoffFactor * oneOverOnePlusg;

        // Feedback coefficients for VA one-pole filters.
        float beta4 = oneOverOnePlusg;
        float beta3 = beta4 * alpha;
        float beta2 = beta3 * alpha;
        float beta1 = beta2 * alpha;

        // Mix the feedback with the input.
        float feedback = beta1 * z1 + beta2 * z2 + beta3 * z3 + beta4 * z4;
        float k = 4.0f * resonance[i];
        float xin = (inputAndOutput[i] - k * feedback) / (1.0f + k * alpha * alpha * alpha * alpha);

        // Apply pole 1.
        float lpf1In = (xin - z1) * alpha;
        float lpf1Out = lpf1In + z1;
        z1 = lpf1In + lpf1Out;

        // Apply pole 2.
        float lpf2In = (lpf1Out - z2) * alpha;
        float lpf2Out = lpf2In + z2;
        z2 = lpf2In + lpf2Out;

        // Apply pole 3.
        float lpf3In = (lpf2Out - z3) * alpha;
        float lpf3Out = lpf3In + z3;
        z3 = lpf3In + lpf3Out;

        // Apply pole 4.
        float lpf4In = (lpf3Out - z4) * alpha;
        float lpf4Out = lpf4In + z4;
        z4 = lpf4In + lpf4Out;

        // Implement the specified filter-mode.
        inputAndOutput[i] = coeffs[4] * lpf4Out + coeffs[3] * lpf3Out + coeffs[2] * lpf2Out + coeffs[1] * lpf1Out + coeffs[0] * xin;
    }

    z[0] = z1;
    z[1] = z2;
    z[2] = z3;
    z[3] = z4;
}

const LerpTable &LadderFilterOld::getCutoffFactorTable()
{
    static const LerpTable table(
        [](double x) -> double { return std::tan(x * M_PI); },
        0.0f, 0.49f, 512);
    return table;
}

void LadderFilterOld::reset()
{
    z[0] = 0;
    z[1] = 0;
    z[2] = 0;
    z[3] = 0;
}
