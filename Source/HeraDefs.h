// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define HERA_OVERSAMPLING 2

enum {
    kHeraMaxVoices = 6,
};

enum {
    kHeraParamVCA,
    kHeraParamVCAType,
    kHeraParamPWMDepth,
    kHeraParamPWMMod,
    kHeraParamSawLevel,
    kHeraParamPulseLevel,
    kHeraParamSubLevel,
    kHeraParamNoiseLevel,
    kHeraParamPitchRange,
    kHeraParamPitchModDepth,
    kHeraParamVCFCutoff,
    kHeraParamVCFResonance,
    kHeraParamVCFEnvelopeModDepth,
    kHeraParamVCFLFOModDepth,
    kHeraParamVCFKeyboardModDepth,
    kHeraParamVCFBendDepth,
    kHeraParamAttack,
    kHeraParamDecay,
    kHeraParamSustain,
    kHeraParamRelease,
    kHeraParamLFOTriggerMode,
    kHeraParamLFORate,
    kHeraParamLFODelay,
    kHeraParamHPF,
    kHeraParamChorusI,
    kHeraParamChorusII,
    kHeraParamArpEnable,
    kHeraParamArpMode,
    kHeraParamArpStep,
    kHeraParamArpRange,
    kHeraNumParameters,
};

enum {
    kHeraVCATypeEnvelope,
    kHeraVCATypeGate,
};

enum {
    kHeraPWMManual,
    kHeraPWMLFO,
    kHeraPWMEnvelope,
};

enum {
    kHeraLFOManual,
    kHeraLFOAuto,
};

enum HeraParameterDomain {
    kHeraParameterIsForSynth,
    kHeraParameterIsForVoice,
};

enum {
    kHeraStepNoire, // 1/4
    kHeraStepCroche, // 1/8
    kHeraStepDoubleCroche, // 1/16
    kHeraStepNoireTriolet, // 1/4T
    kHeraStepCrocheTriolet, // 1/8T
    kHeraStepDoubleCrocheTriolet, // 1/16T
    kHeraNumSteps,
};

static const double kHeraStepDurations[kHeraNumSteps] = { // in beats
    1.0, // 1/4
    0.5, // 1/8
    0.25, // 1/16
    2.0/3.0, // 1/4T
    1.0/3.0, // 1/8T
    1.0/6.0, // 1/16T
};

enum kHeraArpMode {
    kHeraArpModeUp,
    kHeraArpModeUpDown,
    kHeraArpModeDown,
};
