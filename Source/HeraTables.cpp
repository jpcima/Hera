// SPDX-License-Identifier: GPL-3.0-or-later

#include "HeraTables.h"

#define DEFTABLE(id, min, max, ...)                  \
    static const float data__##id[] = __VA_ARGS__;   \
    const LerpTable id(data__##id, min, max, false)

// LFO potential approximation
//  Rate     21.8*(x^2.863)+0.4035
//  Delay    (x * 1.64)^1.923
//  Attack   0.0051*exp(4.13*(x+0.307))

DEFTABLE(curveFromLfoRateSliderToFreq, 0.0, 1.0, {0.3, 0.85, 3.39, 11.49, 22.22});
DEFTABLE(curveFromLfoDelaySliderToDelay, 0.0, 1.0, {0.0, 0.0639, 0.85, 1.2, 2.685});
DEFTABLE(curveFromLfoDelaySliderToAttack, 0.0, 1.0, {0.001, 0.053, 0.188, 0.348, 1.15});

// HPF potential approximation
//  Freq     68.53*exp(4.065*amount)+72.629

DEFTABLE(curveFromHpfSliderToFreq, 0.0, 1.0, {140, 250, 520, 1220});

// ENV potential approximation
//  Attack   0.0099*exp(5.8*x)
//  Decay    0.0495*exp(6*x)
//  Release  0.0495*exp(6*x)

DEFTABLE(curveFromAttackSliderToDuration, 0.0, 1.0, {0.001, 0.03, 0.24, 0.65, 3.25});
DEFTABLE(curveFromDecaySliderToDuration, 0.0, 1.0, {0.002, 0.096, 0.984, 4.449, 19.783});
DEFTABLE(curveFromReleaseSliderToDuration, 0.0, 1.0, {0.002, 0.096, 0.984, 4.449, 19.783});
