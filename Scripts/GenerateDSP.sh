#!/bin/bash
set -e

FAUST="$(dirname "$0")/faust.py"

faust() {
    "$FAUST" -I Source "$@"
}

insert_at_tag() {
    local file="$1"
    local tag="$2"
    local text="$3"
    sed -r -i "s~(// ${tag} //)~${text}\n\1~g" "${file}"
}

format() {
    local file="$1"
    mv -f .clang-format .clang-format.orig
    (cat .clang-format.orig; echo "ColumnLimit: 0") > .clang-format
    clang-format -i "$file"
    mv -f .clang-format.orig .clang-format
}

faust -cn HeraDCO Source/HeraDCO.dsp > Source/HeraDCO.hxx
format Source/HeraDCO.hxx

faust -inpl -cn HeraHPF Source/HeraHPF.dsp > Source/HeraHPF.hxx
format Source/HeraHPF.hxx

faust -inpl -cn HeraVCA Source/HeraVCA.dsp > Source/HeraVCA.hxx
format Source/HeraVCA.hxx

faust -inpl -cn HeraVCF Source/HeraVCF.dsp > Source/HeraVCF.hxx
format Source/HeraVCF.hxx

faust -inpl -cn HeraChorus Source/HeraChorus.dsp > Source/HeraChorus.hxx
insert_at_tag Source/HeraChorus.hxx 'BEFORE class' '#include "bbd_line.h"'
insert_at_tag Source/HeraChorus.hxx 'END class' 'private:'
insert_at_tag Source/HeraChorus.hxx 'END class' 'BBD_Line fLine1;'
insert_at_tag Source/HeraChorus.hxx 'END class' 'BBD_Line fLine2;'
insert_at_tag Source/HeraChorus.hxx 'END class' 'float AnalogDelay1(float d, float x) { return fLine1.process_single(x, BBD_Line::hz_rate_for_delay(d, 256)*(1.0f/getSampleRate())); }'
insert_at_tag Source/HeraChorus.hxx 'END class' 'float AnalogDelay2(float d, float x) { return fLine2.process_single(x, BBD_Line::hz_rate_for_delay(d, 256)*(1.0f/getSampleRate())); }'
insert_at_tag Source/HeraChorus.hxx 'END instanceClear' 'fLine1.clear();'
insert_at_tag Source/HeraChorus.hxx 'END instanceClear' 'fLine2.clear();'
insert_at_tag Source/HeraChorus.hxx 'BEGIN instanceInit' 'fLine1.setup(sample_rate, 256, bbd_fin_j60, bbd_fout_j60);'
insert_at_tag Source/HeraChorus.hxx 'BEGIN instanceInit' 'fLine2.setup(sample_rate, 256, bbd_fin_j60, bbd_fout_j60);'
format Source/HeraChorus.hxx
