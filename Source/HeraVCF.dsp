// SPDX-License-Identifier: ISC

declare license "ISC";
declare author "Jean Pierre Cimalando";

// Virtual analog Moog VCF, with unity gain compensation
// Based on following sources
//   - D'Angelo, Stefano & Välimäki, Vesa. (2013). An improved virtual analog model of the Moog ladder filter. Acoustics, Speech, and Signal Processing, 1988. ICASSP-88., 1988 International Conference on. 729-733. 10.1109/ICASSP.2013.6637744.
//   - https://github.com/ddiakopoulos/MoogLadders/blob/master/src/ImprovedModel.h

import("stdfaust.lib");

process(in,fc,res) = in : vcf(fc,res) with {
  // fc = hslider("[1] cutoff", 2000.0, 0.0, 5000.0, 1.0) : si.smoo;
  // res = hslider("[2] resonance", 1.0, 0.0, 1.0, 0.001) : si.smoo;
};

vcf(fc_,res) = ladder with {
  fc = fc_:min(0.49*44100.0);

  ladder = ((+:*(v):Sig:ma.neg:s123:s123:s123:s4)~(*(k))):*(kcorr);
  gcomp = 0; // set to 1 for unity gain compensation
  kcorr = ba.if(gcomp,k+1.0,1.0);

  s123 = (+:*(g):integ:*(v):Sig)~ma.neg;
  s4(x) = next(z') letrec { 'z = next(z):*(v):Sig; } with { next(z) = g*(x-z):integ; };
  integ(x) = (+((x+x')*(0.5/ma.SR)))~_;

  //Sig = ma.tanh;
  Sig = max(-1.0):min(1.0):cubic with { cubic(x) = x-x*x*x*(1.0/3.0); };

  g = fc*(4.0*ma.PI*Vt)*((1.0-x)/(1.0+x)) with { x = fc*(ma.PI/ma.SR); };
  k = 2.0*res;
  v = 1.0/(2.0*Vt);
  Vt = 0.312; //Vt = 26e-3;
  //Vt = hslider("[3] thermal voltage", 0.026, 0.01, 0.5, 0.001);
};
