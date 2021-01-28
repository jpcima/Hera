// SPDX-License-Identifier: GPL-3.0-or-later

declare author "Jean Pierre Cimalando";
declare license "GPL-3.0-or-later";
// Converted from original at pendragon-andyh/junox

import("stdfaust.lib");
import("HeraCommon.dsp");

process(det, pw) = dco(f*det, pw, lsaw*mix, lpulse*mix, lsub*mix, lnoise*mix) with {
  f = hslider("[1] frequency", 0.0, 0.0, 20000.0, 1.0) : fsmooth(10e-3);
  lsaw = hslider("[2] saw level", 0.5, 0.0, 1.0, 0.01) : *(0.2) : fsmooth(10e-3);
  lpulse = hslider("[3] pulse level", 0.0, 0.0, 1.0, 0.01) : *(0.2) : fsmooth(10e-3);
  lsub = hslider("[4] sub level", 0.0, 0.0, 1.0, 0.01) : *(0.195) : fsmooth(10e-3);
  lnoise = hslider("[5] noise level", 0.0, 0.0, 1.0, 0.01) : *(0.21) : max(ba.db2linear(-120)) : fsmooth(10e-3);
  lsum = lsaw+lpulse+lsub+lnoise;
  mix = 0.26/(0.26+(max(lsum, 0.26)-0.26)*0.3);
};

///
dco(frequency, pulseWidth, sawLevel, pulseLevel, subLevel, noiseLevel) =
  sawOut+pulseOut+subOut+noiseOut
with {
  sawOut = saw(frequency)*sawLevel;
  pulseOut = pulse(pulseWidth, frequency)*pulseLevel;
  subOut = sub(frequency)*subLevel;
  noiseOut = no.pink_noise_m*noiseLevel;
};

///
saw(f) = (2*wrap(phase)-1.0)-polyBLEP2(wrap(phase), inc(f), 1.0)
letrec {
  'phase = wrap(phase)+inc(f);
};

///
pulse(w, f) = ba.if(pos, ppos, pneg)-nblep+pblep
with {
  nblep = polyBLEP2(wrap(phase), inc(f), ph);
  pblep = polyBLEP2(ba.if(x<0.0, x+1.0, x), inc(f), ph) with { x = wrap(phase)-pw; };
}
letrec {
  'ppos = ba.if(tr, 1.0-w*0.95, ba.if(pos, ppos*pole, ppos));
  'pneg = ba.if(tr, -1.0, ba.if(pos, pneg, pneg*pole));
  'ph = ba.if(tr, 0.45*(2.0-w*0.95), ph);
}
with {
  pos = wrap(phase)>pw;
  pole = ba.tau2pole(10e-3);
}
letrec {
  'pw = ba.if(tr, 0.5-0.45*w, pw);
}
with {
  tr = phase>=1.0;
}
letrec {
  'phase = wrap(phase)+inc(f);
};

///
sub(f) = out-blep
with {
  blep = polyBLEP2(y, inc(f), out'*pole);
}
letrec {
  'out = ba.if(tr, ba.if(out>0.0, -1.0, 1.0), out*pole);
}
with {
  y = ba.if(z<0.0, z+1.0, z) with { z = wrap(phase)-0.5; };
  pole = ba.tau2pole(10e-3);
  tr = (wrap(phase')<0.5)&(wrap(phase)>=0.5);
}
letrec {
  'phase = wrap(phase)+inc(f);
};

///
polyBLEP2(phase, inc, height) =
  height*ba.if(phase<inc, right, ba.if(phase+inc>1.0, left, 0.0))
with {
  right = (t+t-t*t-1.0) with { t = phase/inc; };
  left = (t*t+(t+t)+1.0) with { t = (phase-1.0)/inc; };
};
/* -gnuplot-
r(t) = t+t-t*t-1.0
l(t) = t*t+(t+t)+1.0
polyBLEP2(p, i) = (p<i)?r(p/i):(p+i>1.0)?l((p-1.0)/i):0.0
*/

///
inc(f) = f*(1.0/ma.SR);
wrap(x) = x-int(x);
