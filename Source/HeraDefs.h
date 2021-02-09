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
