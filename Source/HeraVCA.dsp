// SPDX-License-Identifier: GPL-3.0-or-later

declare author "Jean Pierre Cimalando";
declare license "GPL-3.0-or-later";
// Converted from original at pendragon-andyh/junox

import("stdfaust.lib");
import("HeraCommon.dsp");

process = *(gain) with {
  amount = hslider("[1] Amount", 0.5, 0.0, 1.0, 0.01);
  gain = 0.1*pow(1.2589, 10.0*amount) : tsmooth(10e-3);
};
