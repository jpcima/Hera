// SPDX-License-Identifier: GPL-3.0-or-later

declare author "Jean Pierre Cimalando";
declare license "GPL-3.0-or-later";
// Converted from original at pendragon-andyh/junox

import("stdfaust.lib");
import("HeraCommon.dsp");

process(xin) = ba.if(fc>0.0, xin-g*lpf(fc, xin), xin) with {
  amount = hslider("[1] Amount", 0.0, 0.0, 1.0, 0.01);
  fc = cutoff(amount);
  g = (amount*4.0) : min(1.0) : tsmooth(10e-3);
};

lpf(fc, xin) = xin*a0+z1'
letrec {
  'z1 = -b1*(xin*a0+z1);
}
with {
  b1 = ma.neg(exp(fc*(-2.0*ma.PI/ma.SR))) : tsmooth(10e-3);
  a0 = 1.0+b1;
};

cutoff = ffunction(float curveFromHpfSliderToFreq(float), "HeraTables.h", "");
