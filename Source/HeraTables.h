// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "LerpTable.h"

extern const LerpTable curveFromLfoRateSliderToFreq;
extern const LerpTable curveFromLfoDelaySliderToDelay;
extern const LerpTable curveFromLfoDelaySliderToAttack;

extern const LerpTable curveFromHpfSliderToFreq;

extern const LerpTable curveFromAttackSliderToDuration;
extern const LerpTable curveFromDecaySliderToDuration;
extern const LerpTable curveFromReleaseSliderToDuration;

extern const LerpTable curveSoftClipTanh3;
extern const LerpTable curveSoftClipCubic;

extern const LerpTable curveSineLFO;
