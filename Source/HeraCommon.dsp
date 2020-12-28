// SPDX-License-Identifier: ISC

declare author "Jean Pierre Cimalando";
declare license "ISC";

import("stdfaust.lib");

tsmooth(tau) = si.smooth(ba.tau2pole(tau));

fsmooth(tau) = si.smooth(pole) with {
  pole = ba.if(off, 0.0, ba.tau2pole(tau));
  off = button("[1000] smooth disabled");
};
